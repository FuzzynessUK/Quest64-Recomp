#ifndef __ENEMY_PROGRESSION_DATA_H__
#define __ENEMY_PROGRESSION_DATA_H__

#include <vector>

// Data for the enemy progression option, GENERATED into
// enemy_progression_data.cpp by tools/enemyrandologic.pl (which also writes
// DOCS/enemyrandologic.xlsx, the design this implements). Edit the tool, not
// the .cpp.
namespace merrow::progression {
    // One row of the sheet's Areas tab. Submaps that share a roster
    // (Baragoon Moor, Brannoch Castle, Mammon's World) are one area here;
    // first_raw..last_raw are their indices into mapdata::areas.
    struct AreaInfo {
        const char* name;
        int map_id;         // gCurrentMap value while in the area
        int tier;           // 1-8, story order (sheet: Areas column C)
        int vanilla_table;  // which of the six monster files it uses
        int first_raw;
        int last_raw;
        double avg[5];      // vanilla roster averages: HP, ATK, DEF, AGI, EXP
        double avg_power;   // sheet: Areas column L
    };

    struct MonsterInfo {
        const char* name;
        int home_area;      // index into areas (first area it appears in)
        bool dangerous;     // sheet: Monsters column L
        bool flying;        // the ROM's flag (Monsters column K)
        int stat[5];        // vanilla HP, ATK, DEF, AGI, EXP (for the spoiler log)
    };

    extern const std::vector<AreaInfo> areas;          // progression order
    extern const std::vector<MonsterInfo> monsters;    // ids 0-66
    // Monster id for each entry of each of the six files, in file order
    // (the entry's own index halfword at +2 matches its position here).
    extern const std::vector<std::vector<int>> table_monsters;
}

#endif
