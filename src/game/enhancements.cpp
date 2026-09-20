#include <algorithm>
#include <filesystem>
#include <fstream>
#include <span>
#include <type_traits>
#include <vector>

#include "enhancements.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
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

    // Every monster's stat row, as used by the randomizer: six halfwords per
    // monster with HP first, and HP is stored twice in a row.
    constexpr int monster_count = 75;

    // Magic Barrier is spell 27; its effect parameter sits at +0x3A of the
    // 68-byte entry. Looked up by name so a change to the table cannot
    // silently point this at a different spell.
    constexpr int spell_entry_size = 68;
    constexpr int magic_barrier_param = 0x3A;

    uint32_t magic_barrier_param_address() {
        for (size_t i = 0; i + 3 < data::spells.size(); i += 4) {
            if (data::spells[i] == "Magic Barrier") {
                return static_cast<uint32_t>(std::stoul(data::spells[i + 1], nullptr, 16)) + magic_barrier_param;
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
        if (options.long_magic_barrier) {
            uint32_t address = magic_barrier_param_address();
            if (address != 0) {
                // One byte, so only the low half of the halfword write is used;
                // write it as a byte value in the high position of its own pair.
                writes.push_back({ address, static_cast<uint16_t>(std::clamp(options.magic_barrier_turns, 1, 255)), true });
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
    get("long_magic_barrier", o.long_magic_barrier);
    get("magic_barrier_turns", o.magic_barrier_turns);
    o.magic_barrier_turns = std::clamp(o.magic_barrier_turns, 1, 255);
    return o;
}

void zelda64::enhancements::save_options(const Options& o) {
    nlohmann::json j;
    j["one_hit_ko"] = o.one_hit_ko;
    j["long_magic_barrier"] = o.long_magic_barrier;
    j["magic_barrier_turns"] = o.magic_barrier_turns;
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
