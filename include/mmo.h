#ifndef __MMO_H__
#define __MMO_H__

#include <cstdint>
#include "recomp.h"

// Real Time Combat (Enhancements, Fun): real time battles instead of turn based -
// no turn order, Brian acts whenever he likes and every enemy acts on a timer
// of its own. See
// src/game/mmo.cpp for how the vanilla turns work and what is changed.
namespace zelda64::mmo {
    bool active();
    // Game thread, every frame.
    void on_frame(uint8_t* rdram, recomp_context* ctx);
}

#endif
