#ifndef __EASIERQUEST_H__
#define __EASIERQUEST_H__

#include <cstdint>
#include "randomizer.h"

// Easier Quest: the vanilla game with a fixed set of conveniences, switched
// from the Mods tab like Hard Mode (the setting is `easier_quest` in
// enhancements.json, applied on the next launch). What it turns on:
//
//   - Extra Healing, Walking MP regen at its fastest, Guilty's element
//     fixed to Earth and wings that are never used up: all randomizer
//     options, so the randomizer runs at boot with a fixed preset in place
//     of the user's settings (randomizer::active_options() hands the preset
//     out while this is active, so the native hooks agree with the ROM
//     writes).
//   - Experience tripled, combat and stat alike, by dividing the level
//     requirement tables (0x80053ECC for HP/MP/AGI/DEF, 0x80053D3C for
//     combat experience) by three, so the amounts the game hands out and
//     the status screen's percentages stay its own.
//   - JP Healing Amounts and JP Magic Barrier: forced on in the
//     enhancements' boot copy.
//
// Hard Mode wins when both are on, exactly as it does over the randomizer.
namespace zelda64::easierquest {
    // Easier Quest was on at boot and Hard Mode was not.
    bool active();

    // The randomizer settings Easier Quest plays with.
    const zelda64::randomizer::Options& preset();

    // The table writes that are not randomizer options. Runs after the
    // randomizer and the enhancements.
    void apply_at_boot(uint8_t* rdram);
}

#endif
