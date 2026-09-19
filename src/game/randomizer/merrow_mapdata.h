#ifndef __MERROW_MAPDATA_H__
#define __MERROW_MAPDATA_H__

#include <cstdint>
#include <string>
#include <vector>

// The encounter tables behind Merrow's enemy shuffling, taken from the
// encounter/pack objects in its DataStore.cs by
// tools/convert_merrow_mapdata.pl. See merrow_mapdata.cpp.
//
// Each area points at one of six global enemy tables, holds a list of monster
// packs (what a single encounter is made of) and a list of encounter regions
// (which packs a patch of the map can roll). Areas that share a pack array in
// Merrow get their own copy here, which is what its AreaEncounterData.Copy()
// produces at runtime anyway.
namespace merrow::mapdata {
    // One slot of a pack: which enemy from the area's table, how many always
    // appear, and how many more may.
    struct PackMember {
        int enemy_id;
        int min_count;
        int extra_count;
    };

    // Written as packCount * 3 big-endian words at rom_address.
    struct MonsterPack {
        uint32_t rom_address;
        std::vector<PackMember> members;
    };

    // The preset list is written as four big-endian halfword pairs starting
    // 8 bytes past the region's own address.
    struct Region {
        uint32_t rom_address;
        int preset_count;
        std::vector<int> presets;
    };

    // The area's 0x18-byte map header. Only table_index is ever changed; the
    // rest is rewritten unchanged so the whole struct can be written in one go.
    struct AreaMap {
        uint32_t rom_address;
        uint32_t unk0;
        uint32_t ptr_door_data;
        uint32_t door_count;
        uint32_t unk8;
        uint16_t unk10;
        uint16_t table_index;
        uint32_t unk14;
    };

    struct EnemyTable {
        uint32_t rom_address;
        std::vector<std::string> enemies;
    };

    struct Area {
        std::string name;
        AreaMap map;
        int pack_start;
        int pack_count;
        int region_start;
        int region_count;
    };

    extern const std::vector<EnemyTable> monster_tables;
    extern const std::vector<MonsterPack> packs;
    extern const std::vector<Region> regions;
    extern const std::vector<Area> areas;
}

#endif
