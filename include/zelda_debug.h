#ifndef __ZELDA_DEBUG_H__
#define __ZELDA_DEBUG_H__

#include <vector>
#include <string>

namespace zelda64 {
    struct SceneWarps {
        int index;
        std::string name;
        std::vector<std::string> entrances;
    };

    struct AreaWarps {
        std::string name;
        std::vector<SceneWarps> scenes;
    };

    extern std::vector<AreaWarps> game_warps;

    void do_warp(int area, int scene, int entrance);
    void set_time(uint8_t day, uint8_t hour, uint8_t minute);

    // Queue a field warp to entrance `entrance` of `submap` in `map`. It fires
    // on the next frame the game is in the field and not already transitioning.
    void do_map_warp(int map, int submap, int entrance);

    // Layout of the game's maps (see src/game/map_table.cpp). Indices past
    // these counts read outside the game's tables.
    int map_count();
    int submap_count(int map);
    int entrance_count(int map, int submap);
    const char* map_name(int map);
    // "N: to A, B" / "N: from A" descriptions built from the game's exit data.
    std::string submap_label(int map, int submap);
    std::string entrance_label(int map, int submap, int entrance);

    // Fields of gPlayerMainData that the cheats menu edits. The order matches
    // the element bytes in memory.
    enum class PlayerStat {
        HP,
        MaxHP,
        MP,
        MaxMP,
        Agility,
        Defense,
        Fire,
        Earth,
        Wind,
        Water,
        Count
    };

    // False until the game has run a frame and the stats have been read.
    bool player_stats_available();
    // Value read from the game on its most recent frame.
    int get_player_stat(PlayerStat stat);
    // Queue a value to be written on the game's next frame.
    void set_player_stat(PlayerStat stat, int value);

    // Multiplier applied to Brian's field/battle movement each frame.
    void set_player_speed_scale(float scale);
    float get_player_speed_scale();
}

#endif
