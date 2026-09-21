#ifndef __STATFX_H__
#define __STATFX_H__

#include <cstdint>
#include <vector>

// The JP stat-up effect (Options::stat_up_effect). Eltale Monsters bursts a
// colour over Brian when a stat rises - red HP, blue MP, yellow defense,
// green agility - and the US build has no such routine at all (see
// DOCS/HANDOFF.md), so it is drawn here instead: the game thread watches
// the four stats each frame and projects Brian through the game's own
// camera matrices, and the UI thread sprays particles at that screen
// position from its own overlay context.
namespace zelda64::statfx {
    enum class Stat {
        HP,
        MP,
        Defense,
        Agility,
    };

    struct Event {
        Stat stat;
        int amount;
    };

    // Brian's feet and head on screen as normalised device coordinates
    // (-1..1, y up), valid only while he is in front of the camera.
    struct Anchor {
        bool valid = false;
        float feet_x = 0.0f;
        float feet_y = 0.0f;
        float head_x = 0.0f;
        float head_y = 0.0f;
    };

    // Game thread, once per frame from the cheats frame hook.
    void on_frame(uint8_t* rdram);

    // UI thread: the rises seen since the last call, and where Brian is.
    std::vector<Event> take_events();
    Anchor anchor();
}

#endif
