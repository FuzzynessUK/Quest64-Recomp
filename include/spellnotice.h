#ifndef __SPELLNOTICE_H__
#define __SPELLNOTICE_H__

#include <cstdint>
#include <string>
#include <vector>

// "Show spell learnt" (Options::spell_notice). The game says nothing when an
// element level reaches a spell's unlock level; this watches the four
// element levels each frame and, for every spell whose unlock level was
// just passed, posts "You have learnt <spell>" to the notification stack
// (include/notify.h). Unlock levels are read out of the (patched) ROM, so
// the randomizer's level options are respected.
namespace zelda64::spellnotice {
    // Game thread, once per frame from the cheats frame hook.
    void on_frame(uint8_t* rdram);
}

#endif
