#ifndef __ENEMY_PROGRESSION_H__
#define __ENEMY_PROGRESSION_H__

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "randomizer.h"

// The Enemy Randomizer: the design in DOCS/enemyrandologic.xlsx.
//
// Two halves. At boot, each area is given one of the game's six monster
// files; any file may serve any area. The files are the unit of placement
// because every file loads to the same RAM base (the file table at
// 0x80054160: rom start, rom end, three pointers into the loaded file), so
// two files' monsters can never be in memory at once and a roster cannot mix
// them. Then, whenever an area's monster file is loaded, every entry of its
// table is rewritten in place for that area (sheet rules 5-6):
//
//     new = AreaAvg(here, stat) * (own / AreaAvg(home, stat)) ^ s * guard(here)
//
// i.e. the monster is moved to the destination's average and keeps its own
// deviation from its home average, compressed by the shape exponent s (0.5:
// a monster three times its home average is 1.7 times the destination's).
// guard(here) = budget / avg_power, budget = MAX(own avg power, previous
// area's budget), so the numbers never fall as the story advances. Spell
// damage follows ATK inside the game (the base passed to the damage routine
// already tracks the attacker's ATK), so nothing else needs scaling.
namespace zelda64::randomizer::progression {
    struct Slot {
        int home = -1;   // index into progression data areas, -1 = leave alone
        int dest = -1;
    };

    struct Plan {
        bool enabled = false;
        // The file each of the 27 raw areas (mapdata::areas order) uses.
        std::array<int, 27> table_index{};
        // Per game map: home/destination areas for each entry of the file.
        std::array<std::vector<Slot>, 36> by_map{};
    };

    // Files an area may use (all of them, with the sheet's default spread).
    std::vector<int> candidate_tables(int area, const Options& options);

    // Builds the plan for the chosen files and appends the spoiler text.
    Plan make_plan(const std::vector<int>& table_per_area, const Options& options, std::string& spoiler);

    // What the game booted with, for the hooks.
    void set_active(const Plan& plan);
    bool active();

    enum class Stat { HP, ATK, DEF, AGI, EXP };
    // The value a stat should have for the monster in table entry `entry` of
    // the file the given map uses, from what the file holds (`own`). Returns
    // `own` unchanged when the plan has nothing for that map or entry.
    int32_t scaled_value(int map_id, int entry, Stat stat, int32_t own);
}

#endif
