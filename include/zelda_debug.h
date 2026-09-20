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
    // `from_cheats` warps are dropped when the cheats master switch is off;
    // enhancements pass false so their warps are independent of it.
    void do_map_warp(int map, int submap, int entrance, bool from_cheats = true);
    // The map the player is currently in, or -1 before the game has loaded one.
    int current_map();
    // True only while the player is walking around the field: false in a
    // battle, a menu or a map transition.
    bool in_field();

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

    // Master switch for the cheats tab. With it off nothing on the tab
    // writes to the game: stat edits, the speed scale, queued warps and
    // item grants are all dropped, while the live readouts keep updating.
    void set_cheats_enabled(bool enabled);
    bool cheats_enabled();

    // Every item the game has, indexed by item id (0x00-0x19).
    const std::vector<std::string>& item_names();
    // Queue `item_id` into the first free inventory slot on the next frame.
    void give_item(int item_id);
}

#endif
