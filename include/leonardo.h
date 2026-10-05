#ifndef __LEONARDO_H__
#define __LEONARDO_H__

#include <cstdint>

// Play as Leonardo (Enhancements, Quality of Life): src/game/leonardo.cpp.
namespace zelda64::leonardo {
    // Boot, with the ROM patches: while Brian is Leonardo, the Leonardo you
    // meet is Brian, so his name - the speech box's name line, in both NPC
    // files that carry him, and "I'm Leonardo" in what he says - is Brian's.
    void apply_at_boot();
}

#endif
