#ifndef __HARDMODE_H__
#define __HARDMODE_H__

#include <cstdint>
#include <string>

// Landmine36's Quest 64 Hard Mode (github.com/Landmine36/Quest64-Hack-HM),
// switched on from the Enhancements tab and applied at boot.
//
// The hack ships as a Ninja 2 .rup patch. Applied to the US ROM it changes
// about 60 KB of game data (monster stats, spells, spirits, items, text, the
// title and intro overlays), rewrites ~60 instructions in the code segment
// (most of them jumps into new code) and appends a 118 KB payload that the
// hack's boot code DMAs to RAM 0x80400000: 64 KB of replacement text, then
// hand-written asm shims, C code and tables.
//
// The recomp never executes ROM code, so the port has three parts:
//
//   - The patch's data records are applied to the ROM in memory here, the
//     same way the randomizer does it, and the payload is copied into RDRAM.
//   - The payload's C functions are recompiled by N64Recomp out of the
//     patched ROM (tools/hardmode/, output in src/game/hardmode/recompiled/).
//   - Every instruction the hack changes in the game's own code becomes a
//     native hook in hooks.cpp, registered in us.rev0.toml and gated on
//     active(). Function-entry jumps call the recompiled payload function
//     and return; mid-function shims are reproduced on the register context.
//
// Saves go to their own folder: a Hard Mode save has 121 spirits and stats
// up to 999, which a vanilla game would misread.
namespace zelda64::hardmode {
    // Hard Mode was on at boot and the patch applied cleanly. Every hook
    // checks this, so a failed patch leaves the vanilla game untouched.
    bool active();

    // Version string from the embedded patch header, e.g. "0.9.8.3A".
    std::string patch_version();

    // Applies the patch to the ROM in memory and copies the payload into
    // RDRAM. Runs before the other boot-time patches so they stack on top.
    void apply_at_boot(uint8_t* rdram);

    // Once per frame from the cheats hook: switches the save folder the
    // first time it runs, which is after the saving thread exists.
    void on_frame(uint8_t* rdram);

    // The element level Hard Mode allows right now: it rises with each boss
    // beaten (a table in its payload). Only meaningful while active().
    int element_cap(uint8_t* rdram);
}

#endif
