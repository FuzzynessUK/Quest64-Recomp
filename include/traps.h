#ifndef __TRAPS_H__
#define __TRAPS_H__

#include <cstdint>

#include "recomp.h"

// Traps, Ship of Harkinian / OoT Archipelago style: bad things that can sit
// in the item pool in place of an item. Each is queued from any thread and
// lands on the game thread as soon as Brian is somewhere it can, in the
// field or in a battle: a save loaded, no menu, door or text box, and Brian
// standing, walking or moving in battle (player states 0-2).
// None lands within 0.8 seconds of a door or map load. An Ice Trap that
// meets a door lets Brian go through and starts its five seconds again
// 0.8 seconds after he arrives. A frozen Brian cannot walk, so he cannot
// walk into a door either.
//
//   Death  Brian falls as a death from DeathLink makes him (the game's own
//          lost-battle path), without telling the room.
//   Ice    Brian cannot move or act (A, B, Z) for five seconds, in the ice
//          block Ice Knife freezes him in (the look only: not the frozen
//          status).
//   Mp     MP drops by half of max MP (not below 0).
//   Hp     HP drops by half of max HP (not below 1: killing is Death's job).
//
// They reach the game as Archipelago items (archipelago.h, item_trap) with
// the yaml's traps options. Each trap is logged to traps.txt.
namespace zelda64::traps {
    enum class Trap { Death, Ice, Mp, Hp };

    const char* name(Trap trap);

    // Any thread.
    void queue(Trap trap);

    // Game thread, every frame.
    void on_frame(uint8_t* rdram, recomp_context* ctx);

    // Game thread: true while an Ice Trap holds Brian.
    bool frozen();

    // Any thread: true while an Ice Trap holds Brian, so the controller
    // callback hides A, B, Z and the stick (no talking, chests or swings).
    bool input_locked();
}

#endif
