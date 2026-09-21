#include <span>
#include <string>

#include "spellnotice.h"
#include "notify.h"
#include "enhancements.h"
#include "randomizer/merrow_data.h"
#include "librecomp/game.hpp"
#include "recomp.h"

namespace data = merrow::data;

namespace {
    // Brian's element levels, in the order the four bytes sit in memory.
    constexpr int32_t gPlayerMainData = 0x8007BA80;
    constexpr int32_t element_levels = gPlayerMainData + 0x24;   // Fire, Earth, Wind, Water
    constexpr int element_count = 4;

    // The levels change on the element-choice screen a spirit opens, which
    // is neither the field (gGameMode 1) nor a battle (3), so the watch runs
    // in every mode except the file select (2) and the title (4), where the
    // baseline is reset so loading a save never reads as learning.
    constexpr int32_t gGameMode = 0x8007B2E0;
    constexpr int32_t gNextMap = 0x80084EE4;

    // The 60 player spells of merrow::data::spells: name, ROM address of the
    // 68-byte entry (unlock level is its first halfword), in Fire, Earth,
    // Water, Wind order, fifteen each.
    constexpr int player_spells = 60;

    struct Spell {
        std::string name;
        int element;    // index into the level bytes
        int level;
    };
    std::vector<Spell> spells;
    bool spells_loaded = false;

    int previous[element_count] = {};
    bool primed = false;

    int element_index(const std::string& name) {
        if (name == "Fire") return 0;
        if (name == "Earth") return 1;
        if (name == "Wind") return 2;
        return 3;   // Water
    }

    void load_spells() {
        std::span<const uint8_t> rom = recomp::get_rom();
        for (int i = 0; i < player_spells && (i * 4 + 3) < static_cast<int>(data::spells.size()); i++) {
            uint32_t address = static_cast<uint32_t>(std::stoul(data::spells[i * 4 + 1], nullptr, 16));
            int level = address + 1 < rom.size() ? (rom[address] << 8) | rom[address + 1] : 0;
            std::string element = (i * 6 + 1) < static_cast<int>(data::spelldatatable.size()) ? data::spelldatatable[i * 6 + 1] : "Water";
            spells.push_back({ data::spells[i * 4], element_index(element), level });
        }
        spells_loaded = true;
    }
}

void zelda64::spellnotice::on_frame(uint8_t* rdram) {
    if (!zelda64::enhancements::active_options().spell_notice) {
        return;
    }
    int mode = MEM_HU(0, gGameMode);
    bool in_game = mode != 2 && mode != 4 && static_cast<int32_t>(MEM_W(0, gNextMap)) != -1;
    if (!in_game) {
        primed = false;
        return;
    }
    if (!spells_loaded) {
        load_spells();
    }

    int current[element_count];
    for (int e = 0; e < element_count; e++) {
        current[e] = MEM_BU(0, element_levels + e);
    }
    if (primed) {
        for (int e = 0; e < element_count; e++) {
            if (current[e] <= previous[e]) {
                continue;
            }
            for (const Spell& spell : spells) {
                if (spell.element == e && spell.level > previous[e] && spell.level <= current[e]) {
                    zelda64::notify::post("You have learnt " + spell.name);
                }
            }
        }
    }
    for (int e = 0; e < element_count; e++) {
        previous[e] = current[e];
    }
    primed = true;
}
