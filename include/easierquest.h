#ifndef __EASIERQUEST_H__
#define __EASIERQUEST_H__

#include <cstdint>
#include "randomizer.h"

// Easier Quest: the vanilla game with a fixed set of conveniences, switched
// from the Mods tab like Hard Mode (the setting is `easier_quest` in
// enhancements.json, applied on the next launch). What it turns on:
//
//   - Extra Healing, Walking MP regen at its fastest, combat EXP tripled
//     and Guilty's element fixed to Earth: all randomizer options, so the
//     randomizer runs at boot with a fixed preset in place of the user's
//     settings (randomizer::active_options() hands the preset out while
//     this is active, so the native hooks agree with the ROM writes).
//   - Stat experience tripled: the 54-entry table of experience each
//     HP/MP/AGI/DEF level needs (0x80053ECC, read by func_800074A0 and the
//     status screen) is divided by three, as are the two walking-distance
//     thresholds (D_800710F8/D_80071100) that pay out agility experience.
//   - Wings are not used up: func_80021524 removes a used item only when
//     its handler returns non-zero (0x800220DC); a hook there answers zero
//     for the six wings.
//   - JP Healing Amounts and JP Magic Barrier: forced on in the
//     enhancements' boot copy.
//
// Hard Mode wins when both are on, exactly as it does over the randomizer.
namespace zelda64::easierquest {
    // Easier Quest was on at boot and Hard Mode was not.
    bool active();

    // The randomizer settings Easier Quest plays with.
    const zelda64::randomizer::Options& preset();

    // The ROM/RAM writes that are not randomizer options. Runs after the
    // randomizer and the enhancements.
    void apply_at_boot(uint8_t* rdram);
}

#endif
