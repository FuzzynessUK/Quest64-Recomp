#ifndef __ENEMY_PROGRESSION_H__
#define __ENEMY_PROGRESSION_H__

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "randomizer.h"

// Progression-aware enemy randomizer: the design in DOCS/enemyrandologic.xlsx.
//
// Two halves. At boot, each area is given one of the game's six monster
// files, chosen from those whose monsters sit within the tier spread of the
// area (sheet: Settings B4/B5, Monsters Q-R). The files are the unit of
// placement because every file loads to the same RAM base (the file table at
// 0x80054160: rom start, rom end, three pointers into the loaded file), so
// two files' monsters can never be in memory at once and a roster cannot mix
// them. Then, when a battle starts, every monster's stats are multiplied on
// the way into the battle struct by the area's factors (sheet rules 5-6):
//
//     stat * (AreaAvg(here, stat) / AreaAvg(home, stat)) ^ k * guard(here)
//
// guard(here) = budget / avg_power, budget = MAX(own avg power, previous
// area's budget), so the numbers never fall as the story advances. EXP and
// Stones use k = 1. Scaling at battle time rather than in the ROM is what
// lets one file serve two areas of different tiers (as it does in vanilla).
namespace zelda64::randomizer::progression {
    struct Factors {
        double hp = 1.0;
        double atk = 1.0;
        double def = 1.0;
        double agi = 1.0;
        double exp = 1.0;
    };

    struct Plan {
        bool enabled = false;
        // The file each of the 27 raw areas (mapdata::areas order) uses.
        std::array<int, 27> table_index{};
        // Per game map: factors for each entry of the file the area uses.
        std::array<std::vector<Factors>, 36> by_map{};
    };

    // Files an area may use under the options' spread (sheet rule 3, plus the
    // dangerous pin and the flier limits of rules 6-7). Never empty: with no
    // candidate the area's vanilla file is returned.
    std::vector<int> candidate_tables(int area, const Options& options);

    // Builds the factors for the chosen files and appends the spoiler text.
    Plan make_plan(const std::vector<int>& table_per_area, const Options& options, std::string& spoiler);

    // What the game booted with, for the hooks.
    void set_active(const Plan& plan);
    bool active();
    // Multiplier for one stat of the monster in table entry `entry` of the
    // file the current map uses. 1.0 when nothing applies.
    enum class Stat { HP, ATK, DEF, AGI, EXP };
    double factor(int map_id, int entry, Stat stat);
}

#endif
