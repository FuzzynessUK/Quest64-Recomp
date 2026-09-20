#include <algorithm>
#include <filesystem>
#include <fstream>
#include <span>
#include <type_traits>
#include <vector>

#include "enhancements.h"
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
    get("jp_healing", o.jp_healing);
    get("exit_from_anywhere", o.exit_from_anywhere);
    get("longer_magic_barrier", o.longer_magic_barrier);
    return o;
}

void zelda64::enhancements::save_options(const Options& o) {
    nlohmann::json j;
    j["one_hit_ko"] = o.one_hit_ko;
    j["jp_healing"] = o.jp_healing;
    j["exit_from_anywhere"] = o.exit_from_anywhere;
    j["longer_magic_barrier"] = o.longer_magic_barrier;
    std::ofstream out(options_path());
    out << j.dump(4);
}

const zelda64::enhancements::Options& zelda64::enhancements::active_options() {
    if (!active_loaded) {
        active = load_options();
        active_loaded = true;
    }
    return active;
}

void zelda64::enhancements::apply_at_boot(uint8_t* rdram) {
    const Options& options = active_options();
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
    // Refused outright in a battle, a menu or a transition. Without this the
    // warp would sit in the queue and fire the moment the battle ended, which
    // is worse than doing nothing: the press appears to be ignored and then
    // takes effect much later.
    if (!zelda64::in_field()) {
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
    zelda64::do_map_warp(map, 0, 0, false);
}
