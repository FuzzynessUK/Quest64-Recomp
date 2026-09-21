#include <algorithm>
#include <cmath>
#include <cstring>
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include "zelda_debug.h"
#include "randomizer/merrow_data.h"
#include "enhancements.h"
#include "hardmode.h"
#include "speedrun.h"
#include "statfx.h"
#include "spellnotice.h"
#include "audio.h"
#include "librecomp/helpers.hpp"

namespace {
    // Master switch for everything on the cheats tab; defined with the
    // inventory code at the bottom of this file.
    extern std::atomic<bool> cheats_on;
    extern std::atomic<int32_t> live_current_map;
    // gCurrentMap, per Quest64Syms/data_dump.toml.
    constexpr int32_t gCurrentMap = 0x80084EEC;
    void apply_pending_item(uint8_t* rdram);
}

// #include "../patches/input.h"

std::atomic<uint16_t> pending_warp = 0xFFFF;
std::atomic<uint32_t> pending_set_time = 0xFFFF;

void zelda64::do_warp(int area, int scene, int entrance) {
    const zelda64::SceneWarps game_scene = zelda64::game_warps[area].scenes[scene];
    int game_scene_index = game_scene.index;
    pending_warp.store(((game_scene_index & 0xFF) << 8) | ((entrance & 0x0F) << 4));
}

extern "C" void recomp_get_pending_warp(uint8_t* rdram, recomp_context* ctx) {
    // Return the current warp value and reset it.
    _return(ctx, pending_warp.exchange(0xFFFF));
}

void zelda64::set_time(uint8_t day, uint8_t hour, uint8_t minute) {
    pending_set_time.store((day << 16) | (uint16_t(hour) << 8) | minute);
}

extern "C" void recomp_get_pending_set_time(uint8_t* rdram, recomp_context* ctx) {
    // Return the current set time value and reset it.
    _return(ctx, pending_set_time.exchange(0xFFFF));
}

// Quest 64 map warp.
//
// Every field exit goes through one record. When Brian touches an exit trigger
// the game fills D_80085368 with the destination {map, submap, entrance}, sets
// bit 0x80 of gGameState and starts the countdown in D_8007B2EC. When that
// reaches zero the field loop (func_80001800, 0x80001CD8) copies the record
// into gCurrentMap/gCurrentSubmap, sets bit 0x40, and func_8000BD10 loads the
// map while func_80012780 spawns Brian at the chosen entrance. Filling the
// record ourselves, the way func_80016B18 does, lets the game run its own fade
// and spawn logic rather than us synthesising one.
namespace {
    // Names follow Quest64Syms/data_dump.toml where a symbol exists.
    constexpr int32_t gGameMode = 0x8007B2E0;  // u16: 1 while in the field
    constexpr int32_t gGameState = 0x8007B2E4; // u32 flags
    constexpr int32_t D_8007B2EC = 0x8007B2EC; // s32 frames until the pending exit fires
    constexpr int32_t D_8007BA4C = 0x8007BA4C; // s32 entry type used when spawning
    constexpr int32_t D_8007BABE = 0x8007BABE; // u16 player animation timer
    constexpr int32_t D_80085368 = 0x80085368; // s32 destination map
    constexpr int32_t D_8008536C = 0x8008536C; // s32 destination submap
    constexpr int32_t D_80085370 = 0x80085370; // s32 entrance index within the submap
    // gBattleState, per Quest64Syms. Bit 0 is set while a battle is running:
    // func_8001C5F4 checks exactly this before counting down to the next
    // encounter, and skips when it is set.
    constexpr int32_t gBattleState = 0x8008C592;
    constexpr int32_t battle_running = 0x1;

    constexpr uint16_t game_mode_field = 1;
    // Any of these set means a transition or battle is already under way; the
    // game's own exit check (0x80001BEC) refuses to start another one.
    constexpr int32_t game_state_busy = 0x87;
    constexpr int32_t game_state_exiting = 0x80;
    constexpr int32_t exit_countdown_frames = 0x1E;
    // Plain walk-in, as used by func_80016B18 and func_8000BB68.
    constexpr int32_t entry_type_walk_in = 0x108;

    struct MapWarp {
        int32_t map;
        int32_t submap;
        int32_t entrance;
        bool from_cheats;
        // Warps that must happen now or not at all, rather than waiting for the
        // field to settle. Pressing the Exit button in a battle should do
        // nothing, not fire the moment the battle ends.
        bool drop_if_busy;
        // When the request was made. A drop_if_busy warp that has not become
        // applicable within a few frames is discarded, so pressing Exit in a
        // battle does nothing at all rather than firing once it ends.
        std::chrono::steady_clock::time_point requested_at;
    };

    std::mutex pending_map_warp_mutex;
    std::optional<MapWarp> pending_map_warp;
    // Lets the per-frame hook skip the mutex when nothing is queued.
    std::atomic<bool> map_warp_queued = false;
}

void zelda64::do_map_warp(int map, int submap, int entrance, bool from_cheats, bool drop_if_busy) {
    // The spawner indexes the map's tables unchecked, so never queue a
    // location the map doesn't have.
    map = std::clamp(map, 0, map_count() - 1);
    submap = std::clamp(submap, 0, submap_count(map) - 1);
    entrance = std::clamp(entrance, 0, entrance_count(map, submap) - 1);

    std::lock_guard lock{pending_map_warp_mutex};
    pending_map_warp = MapWarp{map, submap, entrance, from_cheats, drop_if_busy,
        std::chrono::steady_clock::now()};
    map_warp_queued.store(true);
}

// The request stays queued until the game is in the field and idle, so a warp
// asked for mid-transition fires once that transition has settled.
static void apply_map_warp(uint8_t* rdram) {
    if (!map_warp_queued.load()) {
        return;
    }

    bool ready = MEM_HU(0, gGameMode) == game_mode_field
        && (MEM_W(0, gGameState) & game_state_busy) == 0
        && (MEM_HU(0, gBattleState) & battle_running) == 0;

    MapWarp warp;
    {
        std::lock_guard lock{pending_map_warp_mutex};
        if (!pending_map_warp) {
            return;
        }
        // An immediate warp is dropped as soon as it is not applicable, and
        // also if it has simply gone stale: the per-frame hook does not
        // necessarily run in every game mode, so a request made during a
        // battle could otherwise sit untouched and fire when the field came
        // back. Wall-clock time covers both cases.
        if (pending_map_warp->drop_if_busy) {
            bool stale = std::chrono::steady_clock::now() - pending_map_warp->requested_at
                > std::chrono::milliseconds(250);
            if (!ready || stale) {
                pending_map_warp.reset();
                map_warp_queued.store(false);
                return;
            }
        }
        if (!ready) {
            return;
        }
        warp = *pending_map_warp;
        pending_map_warp.reset();
        map_warp_queued.store(false);
    }

    if (warp.from_cheats && !cheats_on.load()) {
        return;
    }

    MEM_W(0, D_80085368) = warp.map;
    MEM_W(0, D_8008536C) = warp.submap;
    MEM_W(0, D_80085370) = warp.entrance;
    MEM_W(0, D_8007BA4C) = entry_type_walk_in;
    MEM_H(0, D_8007BABE) = 0;
    MEM_W(0, gGameState) |= game_state_exiting;
    MEM_W(0, D_8007B2EC) = exit_countdown_frames;
}

// Brian's stats.
//
// gPlayerMainData (0x8007BA80) is the live PlayerMainStats block laid out in
// patches/main.c; the status screen (func_80026F34, func_80029448) reads these
// fields straight from it. The element bytes run Fire, Earth, Water, Wind in
// memory: +0x26 is Water and +0x27 is Wind. Having those two the wrong way
// round had the Wind and Water sliders editing each other.
namespace {
    constexpr int32_t gPlayerMainData = 0x8007BA80;

    struct StatField {
        int32_t addr;
        bool is_byte;
    };

    constexpr size_t stat_count = static_cast<size_t>(zelda64::PlayerStat::Count);

    constexpr StatField stat_fields[stat_count] = {
        { gPlayerMainData + 0x04, false }, // HP
        { gPlayerMainData + 0x06, false }, // MaxHP
        { gPlayerMainData + 0x08, false }, // MP
        { gPlayerMainData + 0x0A, false }, // MaxMP
        { gPlayerMainData + 0x0C, false }, // Agility
        { gPlayerMainData + 0x0E, false }, // Defense
        { gPlayerMainData + 0x24, true },  // Fire
        { gPlayerMainData + 0x25, true },  // Earth
        { gPlayerMainData + 0x27, true },  // Wind
        { gPlayerMainData + 0x26, true },  // Water
    };

    constexpr int32_t no_pending_stat = -1;

    // Latest values read from the game, one per PlayerStat.
    std::atomic<int32_t> live_stats[stat_count] = {};
    std::atomic<bool> stats_valid = false;

    // Values waiting to be written, or no_pending_stat.
    struct PendingStats {
        std::atomic<int32_t> values[stat_count];

        PendingStats() {
            for (auto& value : values) {
                value.store(no_pending_stat);
            }
        }
    };

    PendingStats pending_stats;

    int32_t read_stat(uint8_t* rdram, const StatField& field) {
        return field.is_byte ? MEM_BU(0, field.addr) : MEM_HU(0, field.addr);
    }

    void write_stat(uint8_t* rdram, const StatField& field, int32_t value) {
        if (field.is_byte) {
            MEM_B(0, field.addr) = static_cast<int8_t>(std::clamp(value, 0, 0xFF));
        }
        else {
            MEM_H(0, field.addr) = static_cast<int16_t>(std::clamp(value, 0, 0xFFFF));
        }
    }

    void sync_player_stats(uint8_t* rdram) {
        for (size_t i = 0; i < stat_count; i++) {
            int32_t pending = pending_stats.values[i].exchange(no_pending_stat);
            if (pending != no_pending_stat && cheats_on.load()) {
                write_stat(rdram, stat_fields[i], pending);
            }
            live_stats[i].store(read_stat(rdram, stat_fields[i]));
        }
        stats_valid.store(true);
    }
}

bool zelda64::player_stats_available() {
    return stats_valid.load();
}

int zelda64::get_player_stat(PlayerStat stat) {
    return live_stats[static_cast<size_t>(stat)].load();
}

void zelda64::set_player_stat(PlayerStat stat, int value) {
    pending_stats.values[static_cast<size_t>(stat)].store(value);
}

// Called from a hook at func_80026658, which every game-mode loop (field,
// battle, menus) runs once per frame.
extern "C" void quest64_cheats_frame(uint8_t* rdram) {
    apply_map_warp(rdram);
    sync_player_stats(rdram);
    apply_pending_item(rdram);
    live_current_map.store(static_cast<int32_t>(MEM_W(0, gCurrentMap)));
    zelda64::enhancements::on_frame(rdram);
    zelda64::statfx::on_frame(rdram);
    zelda64::spellnotice::on_frame(rdram);
    zelda64::audio::on_frame(rdram);
    zelda64::hardmode::on_frame(rdram);
    zelda64::speedrun::update(false);
}

// Master switch, inventory and the kill button.
//
// gInventory (0x8008CF78) is a flat list of 150 item ids with 0xFF for an
// empty slot. func_8000FFE8 walks the whole list looking for a match rather
// than indexing it, so a granted item only has to land in some free slot.
// Item ids run 0x00-0x19 and the names come from the randomizer's table, which
// was extracted from the ROM.
namespace {
    constexpr int32_t gInventory = 0x8008CF78;
    constexpr int inventory_slots = 150;
    constexpr int32_t inventory_empty = 0xFF;
    constexpr int32_t no_pending_item = -1;

    std::atomic<bool> cheats_on = true;
    // Cached each frame so the menu can read it without touching RDRAM off
    // the game thread.
    std::atomic<int32_t> live_current_map = -1;
    std::atomic<int32_t> pending_item = no_pending_item;

    void apply_pending_item(uint8_t* rdram) {
        int32_t item = pending_item.exchange(no_pending_item);
        if (item == no_pending_item || !cheats_on.load()) {
            return;
        }
        for (int slot = 0; slot < inventory_slots; slot++) {
            if (MEM_BU(0, gInventory + slot) == inventory_empty) {
                MEM_B(0, gInventory + slot) = static_cast<int8_t>(item);
                return;
            }
        }
    }
}

void zelda64::set_cheats_enabled(bool enabled) {
    cheats_on.store(enabled);
}

bool zelda64::cheats_enabled() {
    return cheats_on.load();
}

const std::vector<std::string>& zelda64::item_names() {
    return merrow::data::itemcapitalcase;
}

void zelda64::give_item(int item_id) {
    if (item_id < 0 || static_cast<size_t>(item_id) >= merrow::data::itemcapitalcase.size()) {
        return;
    }
    pending_item.store(item_id);
}


int zelda64::current_map() {
    return live_current_map.load();
}
