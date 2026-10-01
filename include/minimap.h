#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include <cstdint>
#include <vector>

// The minimap (Enhancements > Quality of Life, placed and sized under Layout).
// The game side reads the current submap's walkable outline and what is on
// it; ui_config.cpp draws it on the notification overlay. What is read, after
// vbhayden's Quest64_MapCheck.lua:
//
//   0x80084F2C  pointer to the map's navigation data, 16 bytes per submap:
//               { vertices, vertex indices, groups, u32 group count }.
//               A vertex is { f32 x, f32 z }; a group { u32 first, u32 count }
//               is one outline, indices first .. first + count inclusive.
//   0x80084F1C  pointer to the submap header: +4 exits, +8 exit count; an
//               exit is 0x24 bytes starting { f32 x, f32 z }, with the
//               destination map and submap as u16 at +0x1E and +0x20.
//   0x800862E0  chests, 0x6C bytes each { f32 x, f32 y, f32 z, ... }, the
//               chest's id at +0x62; count at 0x800869A0.
//   0x80086A00  spirits: u32 count, then from +8, 0x18 each { f32 x, y, z }.
//   0x8007BACC  Brian: +0 x, +8 z, +0x10 heading.
//   0x800905E0  the four text windows, 0x89C bytes each (func_8002E768
//               opens one): u32 flags at +0, 0x20000000 in use and
//               0x10000000 just opened; s16 x, y, width, height at +4 in the
//               320 x 240 picture, from presets at 0x8005F9B8 (dialogue at
//               the bottom is 34, 148, 252 x 68).
//   0x80084EE4 / 0x80084EE8  the map and submap being played (gNextMap /
//               gNextSubmap in Quest64Syms, but they hold where Brian is:
//               the field loop copies the exit record D_80085368 into them
//               when the transition fires). D_80085368 itself changes the
//               moment Brian touches an exit, before the new map is loaded,
//               so reading it put the old map's outline under the new
//               submap's number.
//
// Doors with checks: an exit is marked when the part of the game it leads
// into - its destination submap and every submap of the same map reachable
// from there without coming back through here - holds a chest not yet opened
// or a spirit not yet taken (save flags 0x800869D8 / 0x80086AE8, placements
// from the randomizer when it moved them). While an Archipelago seed is being
// played, a gift NPC whose location is in the seed and not yet checked counts
// too (giftsanity, or the Book's Shannon with the portal on). If the door sits on an outline
// other than the submap's outer edge, that outline is the building's walls
// and is lit up too.
namespace zelda64::minimap {
    struct Point { float x, z; };
    struct Marker {
        float x, z;
        bool done;
        // Exits only: leads to a check, leads to a gift NPC (any, check or
        // not - one not yet given in an Archipelago seed), and the outline the
        // door is on (-1 for the outer edge or none near).
        bool checks = false;
        bool giver = false;
        int group = -1;
    };

    struct Snapshot {
        bool valid = false;        // in the field, with an outline to draw
        uint32_t mesh_version = 0; // changes whenever the outline does
        std::vector<std::vector<Point>> outline;
        float min_x = 0, min_z = 0, max_x = 1, max_z = 1;
        float brian_x = 0, brian_z = 0, heading = 0;
        std::vector<Marker> chests, spirits, exits, givers;
        // Text boxes on screen, in the game's 320 x 240 picture: the minimap
        // is drawn over the finished frame, so it gets out of their way.
        struct Box { float x, y, w, h; };
        std::vector<Box> text_boxes;
    };

    // Game thread, every frame.
    void on_frame(uint8_t* rdram);
    // UI thread: copies the latest state (the outline only when `mesh_have`
    // is out of date). False before the first frame.
    bool snapshot(Snapshot& out, uint32_t mesh_have);

    // The Minimap Zoom In / Out controls (controls.cpp, input thread): +1 or -1
    // a press. The UI takes what has built up each time it draws.
    void zoom(int direction);
    int take_zoom_steps();
}

#endif
