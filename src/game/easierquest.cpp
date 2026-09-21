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
    // Experience is tripled by dividing the level requirements, so the
    // amounts the game hands out (monster EXP, the per-hit stat gains) and
    // the percentages the status screen shows stay the game's own:
    //
    // - 0x80053ECC: 54 u16 entries, the experience each HP/MP/AGI/DEF level
    //   needs (10, 11, 11, 11, 12, ...). func_800074A0 subtracts an entry
    //   and raises the stat when its counter passes it; every stat reads
    //   the same table with its own multiple. Levels cap at 0x36.
    // - 0x80053D3C: 98 u32 entries, the combat experience each level needs
    //   (8, 10, 13, 17, 22, ...), read at 0x80007920; the level caps at 98.
    //
    // Each entry becomes the nearest whole number to a third of itself,
    // never below one.
    constexpr int32_t stat_exp_table = 0x80053ECC;
    constexpr int stat_exp_levels = 54;
    constexpr int32_t combat_exp_table = 0x80053D3C;
    constexpr int combat_exp_levels = 98;
    constexpr int exp_multiplier = 3;

    // The boot segment: ROM 0x1000.. is already in RAM at 0x80000400 when
    // the boot patches run, so anything in it is written to both.
    constexpr int32_t boot_ram_start = 0x80000400;
    constexpr uint32_t boot_rom_start = 0x1000;

    uint32_t rom_of(int32_t vram) {
        return static_cast<uint32_t>(vram - boot_ram_start) + boot_rom_start;
    }

    uint32_t divided(uint32_t value) {
        uint32_t scaled = (value + exp_multiplier / 2) / exp_multiplier;
        return scaled < 1 ? 1 : scaled;
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
        o.boss_element = true;
        o.guilty_element = 1;      // Earth
        o.wings_never_expire = true;
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
    auto write_u32 = [&](int32_t vram, uint32_t value) {
        uint32_t off = rom_of(vram);
        for (int i = 0; i < 4; i++) {
            patched[off + i] = static_cast<uint8_t>(value >> (24 - i * 8));
        }
        MEM_W(0, vram) = static_cast<int32_t>(value);
    };

    for (int level = 0; level < stat_exp_levels; level++) {
        int32_t entry = stat_exp_table + level * 2;
        write_u16(entry, static_cast<uint16_t>(divided(MEM_HU(0, entry))));
    }
    for (int level = 0; level < combat_exp_levels; level++) {
        int32_t entry = combat_exp_table + level * 4;
        write_u32(entry, divided(static_cast<uint32_t>(MEM_W(0, entry))));
    }

    recomp::set_rom_contents(std::move(patched));
}
