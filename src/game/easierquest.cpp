#include <cstring>
#include <span>
#include <vector>

#include "easierquest.h"
#include "enhancements.h"
#include "hardmode.h"
#include "librecomp/game.hpp"
#include "recomp.h"

using zelda64::randomizer::ListMode;
using zelda64::randomizer::Mode;
using zelda64::randomizer::Options;

namespace {
    // Experience needed for each HP/MP/AGI/DEF level, 54 u16 entries at
    // 0x80053ECC (func_800074A0 subtracts an entry and raises the stat
    // when the counter passes it; the status screen reads the same table
    // for its percentage). Dividing it by three triples the rate.
    constexpr int32_t stat_exp_table = 0x80053ECC;
    constexpr int stat_exp_levels = 54;
    // The distance walked (field, battle) before a point of agility
    // experience, doubles read by the same routine.
    constexpr int32_t walk_exp_field = 0x800710F8;
    constexpr int32_t walk_exp_battle = 0x80071100;
    constexpr int exp_multiplier = 3;

    // Item ids: White, Yellow, Blue, Green, Red, Black Wings.
    constexpr int first_wing = 14;
    constexpr int last_wing = 19;
    // The item menu's page offset and cursor, and the inventory they index.
    constexpr int32_t item_menu_page = 0x8008C760;
    constexpr int32_t item_menu_cursor = 0x8008C764;
    constexpr int32_t gInventory = 0x8008CF78;

    // The boot segment: ROM 0x1000.. is already in RAM at 0x80000400 when
    // the boot patches run, so anything in it is written to both.
    constexpr int32_t boot_ram_start = 0x80000400;
    constexpr uint32_t boot_rom_start = 0x1000;

    uint32_t rom_of(int32_t vram) {
        return static_cast<uint32_t>(vram - boot_ram_start) + boot_rom_start;
    }

    bool preset_built = false;
    Options preset_options;
}

bool zelda64::easierquest::active() {
    return zelda64::enhancements::active_options().easier_quest && !zelda64::hardmode::active();
}

const Options& zelda64::easierquest::preset() {
    if (!preset_built) {
        Options o;
        o.mode = Mode::Randomizer;
        o.seed = "Easier Quest";
        // Nothing moves: the lists are left alone.
        o.chests = ListMode::Off;
        o.drops = ListMode::Off;
        o.gifts = ListMode::Off;
        o.wingsmiths = ListMode::Off;
        o.extra_healing = true;
        o.mp_regain = 12;          // fastest tier
        o.exp_boost = 12;          // quarters: x3
        o.boss_element = true;
        o.guilty_element = 1;      // Earth
        preset_options = o;
        preset_built = true;
    }
    return preset_options;
}

void zelda64::easierquest::apply_at_boot(uint8_t* rdram) {
    if (!active()) {
        return;
    }
    std::span<const uint8_t> rom = recomp::get_rom();
    std::vector<uint8_t> patched(rom.begin(), rom.end());

    auto write_u16 = [&](int32_t vram, uint16_t value) {
        uint32_t off = rom_of(vram);
        patched[off] = static_cast<uint8_t>(value >> 8);
        patched[off + 1] = static_cast<uint8_t>(value & 0xFF);
        MEM_H(0, vram) = static_cast<int16_t>(value);
    };
    auto write_f64 = [&](int32_t vram, double value) {
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        uint32_t off = rom_of(vram);
        for (int i = 0; i < 8; i++) {
            patched[off + i] = static_cast<uint8_t>(bits >> (56 - i * 8));
        }
        MEM_W(0, vram) = static_cast<int32_t>(bits >> 32);
        MEM_W(0, vram + 4) = static_cast<int32_t>(bits & 0xFFFFFFFFu);
    };
    auto read_f64 = [&](int32_t vram) {
        uint64_t bits = (static_cast<uint64_t>(static_cast<uint32_t>(MEM_W(0, vram))) << 32)
            | static_cast<uint32_t>(MEM_W(0, vram + 4));
        double value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    };

    for (int level = 0; level < stat_exp_levels; level++) {
        int32_t entry = stat_exp_table + level * 2;
        int needed = MEM_HU(0, entry);
        int scaled = (needed + exp_multiplier - 1) / exp_multiplier;
        write_u16(entry, static_cast<uint16_t>(scaled < 1 ? 1 : scaled));
    }
    write_f64(walk_exp_field, read_f64(walk_exp_field) / exp_multiplier);
    write_f64(walk_exp_battle, read_f64(walk_exp_battle) / exp_multiplier);

    recomp::set_rom_contents(std::move(patched));
}

// func_80021524 at 0x800220DC, just after `jal func_800212E4` used the item
// under the cursor: v0 non-zero means "consumed, take it out of the bag".
extern "C" void quest64_easier_wings(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::easierquest::active()) {
        return;
    }
    int32_t index = static_cast<int32_t>(MEM_W(0, item_menu_page)) + static_cast<int32_t>(MEM_W(0, item_menu_cursor));
    if (index < 0 || index >= 150) {
        return;
    }
    int item = MEM_BU(0, gInventory + index);
    if (item >= first_wing && item <= last_wing) {
        ctx->r2 = 0;
    }
}
