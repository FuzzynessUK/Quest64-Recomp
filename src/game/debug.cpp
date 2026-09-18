#include <algorithm>
#include <atomic>
#include <mutex>
#include <optional>
#include "zelda_debug.h"
#include "librecomp/helpers.hpp"
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
    };

    std::mutex pending_map_warp_mutex;
    std::optional<MapWarp> pending_map_warp;
    // Lets the per-frame hook skip the mutex when nothing is queued.
    std::atomic<bool> map_warp_queued = false;
}

void zelda64::do_map_warp(int map, int submap, int entrance) {
    // The spawner indexes the map's tables unchecked, so never queue a
    // location the map doesn't have.
    map = std::clamp(map, 0, map_count() - 1);
    submap = std::clamp(submap, 0, submap_count(map) - 1);
    entrance = std::clamp(entrance, 0, entrance_count(map, submap) - 1);

    std::lock_guard lock{pending_map_warp_mutex};
    pending_map_warp = MapWarp{map, submap, entrance};
    map_warp_queued.store(true);
}

// The request stays queued until the game is in the field and idle, so a warp
// asked for mid-transition fires once that transition has settled.
static void apply_map_warp(uint8_t* rdram) {
    if (!map_warp_queued.load()) {
        return;
    }

    if (MEM_HU(0, gGameMode) != game_mode_field || (MEM_W(0, gGameState) & game_state_busy) != 0) {
        return;
    }

    MapWarp warp;
    {
        std::lock_guard lock{pending_map_warp_mutex};
        if (!pending_map_warp) {
            return;
        }
        warp = *pending_map_warp;
        pending_map_warp.reset();
        map_warp_queued.store(false);
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
// fields straight from it. The element bytes are drawn top/left/bottom/right on
// the spirit diamond, which is Fire, Earth, Wind, Water.
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
        { gPlayerMainData + 0x26, true },  // Wind
        { gPlayerMainData + 0x27, true },  // Water
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
            if (pending != no_pending_stat) {
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
}
