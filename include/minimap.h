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
//               exit is 0x24 bytes starting { f32 x, f32 z }.
//   0x800862E0  chests, 0x6C bytes each { f32 x, f32 y, f32 z, ... }, the
//               chest's id at +0x62; count at 0x800869A0.
//   0x80086A00  spirits: u32 count, then from +8, 0x18 each { f32 x, y, z }.
//   0x8007BACC  Brian: +0 x, +8 z, +0x10 heading.
//   0x8008536B / 0x8008536F  the map and submap being played.
namespace zelda64::minimap {
    struct Point { float x, z; };
    struct Marker { float x, z; bool done; };

    struct Snapshot {
        bool valid = false;        // in the field, with an outline to draw
        uint32_t mesh_version = 0; // changes whenever the outline does
        std::vector<std::vector<Point>> outline;
        float min_x = 0, min_z = 0, max_x = 1, max_z = 1;
        float brian_x = 0, brian_z = 0, heading = 0;
        std::vector<Marker> chests, spirits, exits;
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
