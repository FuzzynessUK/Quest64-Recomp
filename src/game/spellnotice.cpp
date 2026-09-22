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
    constexpr int32_t element_levels = gPlayerMainData + 0x24;   // Fire, Earth, Water, Wind (the spell table order; seen in game)
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
        if (name == "Water") return 2;
        return 3;   // Wind
    }

    // The name the menu shows for spell i, from the patched ROM: the
    // spell-name pointer table (merrow shuffleNames2[i][5], a RAM address
    // per spell into the text overlay that loads at 0x800C3910 from ROM
    // 0xD4ECD0) followed to its NUL-terminated string. The randomizer's
    // hinted names repoint that table to a 16-byte grid, so this is right
    // in every mode; Extra Healing's "Mending" rename lands here too.
    std::string menu_name(int i) {
        std::span<const uint8_t> rom = recomp::get_rom();
        if (i >= static_cast<int>(data::shuffleNames2.size()) || data::shuffleNames2[i].size() < 7) {
            return {};
        }
        constexpr uint32_t text_ram = 0x800C3910;
        constexpr uint32_t text_rom = 0xD4ECD0;
        uint32_t table = static_cast<uint32_t>(std::stoul(data::shuffleNames2[i][5], nullptr, 16));
        if (table + 4 > rom.size()) {
            return {};
        }
        uint32_t pointer = (static_cast<uint32_t>(rom[table]) << 24) | (rom[table + 1] << 16) | (rom[table + 2] << 8) | rom[table + 3];
        if (pointer < text_ram) {
            return {};
        }
        uint32_t at = pointer - text_ram + text_rom;
        std::string text;
        for (int k = 0; k < 24 && at + k < rom.size() && rom[at + k] != 0; k++) {
            text.push_back(static_cast<char>(rom[at + k]));
        }
        // The menu is upper case; the notice reads better in title case
        // with the level kept as "Lv1".
        bool start = true;
        for (char& c : text) {
            if (start && c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
            else if (!start && c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
            start = (c == ' ');
        }
        size_t lv = text.rfind(" Lv");
        if (lv != std::string::npos && lv + 3 < text.size()) {
            text[lv + 2] = 'v';
        }
        return text;
    }

    void load_spells() {
        std::span<const uint8_t> rom = recomp::get_rom();
        for (int i = 0; i < player_spells && (i * 4 + 3) < static_cast<int>(data::spells.size()); i++) {
            uint32_t address = static_cast<uint32_t>(std::stoul(data::spells[i * 4 + 1], nullptr, 16));
            int level = address + 1 < rom.size() ? (rom[address] << 8) | rom[address + 1] : 0;
            std::string element = (i * 6 + 1) < static_cast<int>(data::spelldatatable.size()) ? data::spelldatatable[i * 6 + 1] : "Water";
            std::string name = menu_name(i);
            if (name.empty()) {
                name = data::spells[i * 4];
            }
            spells.push_back({ name, element_index(element), level });
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
