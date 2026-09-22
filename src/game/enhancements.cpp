#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <span>
#include <type_traits>
#include <vector>

#include "enhancements.h"
#include "hardmode.h"
#include "zelda_render.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
#include "zelda_debug.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"

namespace data = merrow::data;
using zelda64::enhancements::Options;

namespace {
    Options active;
    bool active_loaded = false;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "enhancements.json";
    }

    // Brian's live stats, the same block the cheats menu edits.
    constexpr int32_t gPlayerMainData = 0x8007BA80;
    constexpr int32_t player_hp = gPlayerMainData + 0x04;
    constexpr int32_t player_max_hp = gPlayerMainData + 0x06;

    // Magic Barrier, as found in RAM: a byte counting how much barrier is
    // left. The value is one more than the number of enemy attacks it will
    // absorb, so the stock 4 is three attacks and 6 is five.
    constexpr int32_t magic_barrier_timer = 0x8007BB42;
    constexpr int magic_barrier_bonus = 2;
    constexpr int magic_barrier_min = 3;
    constexpr int magic_barrier_max = 6;

    // Every monster's stat row, as used by the randomizer: six halfwords per
    // monster with HP first, and HP is stored twice in a row.
    constexpr int monster_count = 75;

    // Healing Lv2 potency, the one field that differs between the US and
    // Japanese spell tables.
    constexpr int healing_potency = 0x0C;
    constexpr int jp_healing_lv2 = 16;

    // Spell entries are found by name so a change to the table cannot
    // silently point a patch at a different spell.
    uint32_t spell_entry_address(const std::string& name) {
        for (size_t i = 0; i + 3 < data::spells.size(); i += 4) {
            if (data::spells[i] == name) {
                return static_cast<uint32_t>(std::stoul(data::spells[i + 1], nullptr, 16));
            }
        }
        return 0;
    }

    struct Write {
        uint32_t rom_offset;
        uint16_t value;
        // Monster HP is a halfword; the spell effect parameter is one byte.
        bool single_byte = false;
    };

    std::vector<Write> build_writes(const Options& options) {
        std::vector<Write> writes;
        if (options.one_hit_ko) {
            // Bosses included: the point is that nothing survives a hit.
            for (int monster = 0; monster < monster_count; monster++) {
                uint32_t address = static_cast<uint32_t>(
                    std::stoul(data::monsterstatlocations[monster][0], nullptr, 16));
                writes.push_back({ address, 1, false });
                writes.push_back({ address + 2, 1, false });
            }
        }
        if (options.jp_healing) {
            uint32_t entry = spell_entry_address("Healing Lv2");
            if (entry != 0) {
                writes.push_back({ entry + healing_potency, jp_healing_lv2, false });
            }
        }
        return writes;
    }
}

zelda64::enhancements::Options zelda64::enhancements::load_options() {
    Options o;
    std::ifstream in(options_path());
    if (!in.good()) {
        return o;
    }
    nlohmann::json j;
    try {
        in >> j;
    }
    catch (nlohmann::json::parse_error&) {
        return o;
    }
    auto get = [&j](const char* key, auto& out) {
        auto it = j.find(key);
        if (it != j.end()) {
            try {
                out = it->get<std::remove_reference_t<decltype(out)>>();
            }
            catch (nlohmann::json::type_error&) {}
        }
    };
    get("one_hit_ko", o.one_hit_ko);
    get("n64_mode", o.n64_mode);
    get("saved_resolution", o.saved_resolution);
    get("saved_aspect", o.saved_aspect);
    get("saved_antialiasing", o.saved_antialiasing);
    get("saved_hud_ratio", o.saved_hud_ratio);
    get("speedrun_timer", o.speedrun_timer);
    get("timer_position", o.timer_position);
    o.timer_position = std::clamp(o.timer_position, 0, 5);
    get("jp_healing", o.jp_healing);
    get("exit_from_anywhere", o.exit_from_anywhere);
    get("longer_magic_barrier", o.longer_magic_barrier);
    get("faster_walk", o.faster_walk);
    get("remove_borders", o.remove_borders);
    get("hud_hp_custom", o.hud_hp_custom);
    get("hud_hp_x", o.hud_hp_x);
    get("hud_hp_y", o.hud_hp_y);
    get("hud_sp_custom", o.hud_sp_custom);
    get("hud_sp_x", o.hud_sp_x);
    get("hud_sp_y", o.hud_sp_y);
    get("stat_up_effect", o.stat_up_effect);
    get("spell_notice", o.spell_notice);
    get("song_notice", o.song_notice);
    get("item_notice", o.item_notice);
    get("notifications", o.notifications);
    get("notify_never_expire", o.notify_never_expire);
    get("notify_max", o.notify_max);
    o.notify_max = std::clamp(o.notify_max, 1, 10);
    get("notify_position", o.notify_position);
    o.notify_position = std::clamp(o.notify_position, 0, 8);
    get("hard_mode", o.hard_mode);
    get("easier_quest", o.easier_quest);
    return o;
}

void zelda64::enhancements::save_options(const Options& o) {
    nlohmann::json j;
    j["one_hit_ko"] = o.one_hit_ko;
    j["n64_mode"] = o.n64_mode;
    j["saved_resolution"] = o.saved_resolution;
    j["saved_aspect"] = o.saved_aspect;
    j["saved_antialiasing"] = o.saved_antialiasing;
    j["saved_hud_ratio"] = o.saved_hud_ratio;
    j["speedrun_timer"] = o.speedrun_timer;
    j["timer_position"] = o.timer_position;
    j["jp_healing"] = o.jp_healing;
    j["exit_from_anywhere"] = o.exit_from_anywhere;
    j["longer_magic_barrier"] = o.longer_magic_barrier;
    j["faster_walk"] = o.faster_walk;
    j["remove_borders"] = o.remove_borders;
    j["hud_hp_custom"] = o.hud_hp_custom;
    j["hud_hp_x"] = o.hud_hp_x;
    j["hud_hp_y"] = o.hud_hp_y;
    j["hud_sp_custom"] = o.hud_sp_custom;
    j["hud_sp_x"] = o.hud_sp_x;
    j["hud_sp_y"] = o.hud_sp_y;
    j["stat_up_effect"] = o.stat_up_effect;
    j["spell_notice"] = o.spell_notice;
    j["song_notice"] = o.song_notice;
    j["item_notice"] = o.item_notice;
    j["notifications"] = o.notifications;
    j["notify_never_expire"] = o.notify_never_expire;
    j["notify_max"] = o.notify_max;
    j["notify_position"] = o.notify_position;
    j["hard_mode"] = o.hard_mode;
    j["easier_quest"] = o.easier_quest;
    std::ofstream out(options_path());
    out << j.dump(4);
}

const zelda64::enhancements::Options& zelda64::enhancements::active_options() {
    if (!active_loaded) {
        active = load_options();
        // Easier Quest's two JP options. Checked against the saved
        // hard_mode flag rather than hardmode::active(), which is not known
        // yet the first time this runs; Hard Mode ignores both anyway.
        if (active.easier_quest && !active.hard_mode) {
            active.jp_healing = true;
            active.longer_magic_barrier = true;
        }
        active_loaded = true;
    }
    return active;
}

void zelda64::enhancements::apply_at_boot(uint8_t* rdram) {
    const Options& options = active_options();
    zelda64::renderer::set_hud_layout(options.hud_hp_custom, options.hud_hp_x, options.hud_hp_y,
        options.hud_sp_custom, options.hud_sp_x, options.hud_sp_y);
    zelda64::renderer::set_borders_removed(options.remove_borders);
    std::vector<Write> writes = build_writes(options);
    if (writes.empty()) {
        return;
    }

    // Reads back whatever the randomizer left, so the two stack rather than
    // one overwriting the other.
    std::span<const uint8_t> rom = recomp::get_rom();
    std::vector<uint8_t> patched(rom.begin(), rom.end());

    for (const Write& write : writes) {
        if (write.single_byte) {
            if (write.rom_offset < patched.size()) {
                patched[write.rom_offset] = static_cast<uint8_t>(write.value & 0xFF);
            }
            continue;
        }
        if (write.rom_offset + 1 >= patched.size()) {
            continue;
        }
        patched[write.rom_offset] = static_cast<uint8_t>((write.value >> 8) & 0xFF);
        patched[write.rom_offset + 1] = static_cast<uint8_t>(write.value & 0xFF);
    }

    recomp::set_rom_contents(std::move(patched));
}

void zelda64::enhancements::on_frame(uint8_t* rdram) {
    const Options& options = active_options();

    if (options.longer_magic_barrier) {
        // The counter only rises when the spell is cast and falls as the
        // barrier is used, so acting on a rise applies this once per cast
        // rather than every frame.
        //
        // Each cast is extended by two and then held inside 3-6, so the
        // stock top roll of 4 (three absorbed attacks) becomes 6 (five) and
        // nothing can come out shorter than 3 or longer than 6.
        static int previous = 0;
        int current = MEM_BU(0, magic_barrier_timer);
        if (current > previous && current > 0) {
            int extended = std::clamp(current + magic_barrier_bonus,
                magic_barrier_min, magic_barrier_max);
            MEM_B(0, magic_barrier_timer) = static_cast<int8_t>(extended);
            current = extended;
        }
        previous = current;
    }

    if (!options.one_hit_ko) {
        return;
    }
    // Monsters are handled in the ROM, but Brian's HP lives in the save, so a
    // game already in progress needs it held down here. Only ever lowered, so
    // a death in progress is never undone.
    if (MEM_HU(0, player_max_hp) != 1) {
        MEM_H(0, player_max_hp) = 1;
    }
    if (MEM_HU(0, player_hp) > 1) {
        MEM_H(0, player_hp) = 1;
    }
}

void zelda64::enhancements::cast_exit() {
    if (!active_options().exit_from_anywhere) {
        return;
    }
    // What the Exit spell does: drop the player back at the start of the area
    // they are in. Goes through the same queued warp the cheats menu uses, so
    // the game runs its own fade and spawn, and it waits for a safe moment.
    // Passing from_cheats = false keeps it working with cheats turned off.
    int map = zelda64::current_map();
    if (map < 0 || map >= zelda64::map_count()) {
        return;
    }
    // drop_if_busy: pressed in a battle, a menu or a transition this does
    // nothing at all. Queuing it instead would fire the warp the moment the
    // battle ended, which is worse than ignoring the press.
    zelda64::do_map_warp(map, 0, 0, false, true);
}

// Faster walking (Options::faster_walk). The movement handlers leave the
// frame's velocity in the player struct at +0x18 (x) and +0x20 (z), and
// func_80005748 then moves Brian by it, resolving collisions against
// position + velocity. The velocity is scaled on entry to that call and
// restored on its return, so the game's own wall test sees the longer step
// while the handlers read back next frame exactly what they wrote. (Leaving
// it scaled fed the scaled value into the walk handler's speed lerp, which
// ran away to the cap, and into the skid's 0.68-a-frame decay, which then
// barely decayed at all: that was the long slide after letting go of the
// stick.) Every state moves through this call - the field walk
// (func_8000534C), battle movement (func_80004E58) and the eight-frame skid
// after the stick is released (func_80003F98, D_80070F50) - so the slide
// keeps its vanilla frame count and covers 1.5x the distance, the same
// shape as Hard Mode's 2.75 target speed gives. Hard Mode keeps its own
// pace: the scale is not stacked on top of it.
namespace {
    constexpr float walk_speed_scale = 1.5f;
    // Walls are about 3.5 units thick and the collision test is on position +
    // velocity rather than swept, so a single step must stay under that or
    // Brian ends up on the far side. Vanilla walks 2 units a frame, so the
    // 1.5x step sits exactly on the cap.
    constexpr float max_step_units = 3.0f;

    // Left by the entry hook for the exit hook: the struct that was scaled
    // and the factor actually applied (smaller than walk_speed_scale when
    // the step cap bit). Both hooks run on the game thread.
    int32_t scaled_player = 0;
    float applied_scale = 1.0f;

    float read_f32(uint8_t* rdram, int32_t addr) {
        int32_t bits = MEM_W(0, addr);
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    void write_f32(uint8_t* rdram, int32_t addr, float value) {
        int32_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        MEM_W(0, addr) = bits;
    }
}

extern "C" void quest64_enh_walk_scale(uint8_t* rdram, recomp_context* ctx) {
    applied_scale = 1.0f;
    if (!zelda64::enhancements::active_options().faster_walk || zelda64::hardmode::active()) {
        return;
    }
    int32_t player = static_cast<int32_t>(ctx->r5);
    float vx = read_f32(rdram, player + 0x18);
    float vz = read_f32(rdram, player + 0x20);
    float scale = walk_speed_scale;
    float step = std::sqrt(vx * vx + vz * vz) * scale;
    if (step > max_step_units) {
        scale *= max_step_units / step;
    }
    write_f32(rdram, player + 0x18, vx * scale);
    write_f32(rdram, player + 0x20, vz * scale);
    scaled_player = player;
    applied_scale = scale;
}

extern "C" void quest64_enh_walk_unscale(uint8_t* rdram, recomp_context*) {
    if (applied_scale == 1.0f) {
        return;
    }
    // func_80005748 may have zeroed or projected the velocity on a wall;
    // dividing keeps whatever it decided in the game's own units.
    write_f32(rdram, scaled_player + 0x18, read_f32(rdram, scaled_player + 0x18) / applied_scale);
    write_f32(rdram, scaled_player + 0x20, read_f32(rdram, scaled_player + 0x20) / applied_scale);
    applied_scale = 1.0f;
}
