#ifndef __SPELLNOTICE_H__
#define __SPELLNOTICE_H__

#include <cstdint>
#include <string>
#include <vector>

// "Show spell learnt" (Options::spell_notice). The game says nothing when an
// element level reaches a spell's unlock level; this watches the four
// element levels each frame and, for every spell whose unlock level was
// just passed, queues "You have learnt <spell>" for the UI overlay to show
// top-left and fade after three seconds. Unlock levels are read out of the
// (patched) ROM, so the randomizer's level options are respected.
namespace zelda64::spellnotice {
    // Game thread, once per frame from the cheats frame hook.
    void on_frame(uint8_t* rdram);

    // UI thread: the notices queued since the last call, oldest first.
    std::vector<std::string> take_notices();
}

#endif
