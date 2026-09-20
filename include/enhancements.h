#ifndef __ENHANCEMENTS_H__
#define __ENHANCEMENTS_H__

#include <cstdint>

// Optional changes to how the game plays that are not part of the randomizer.
// Like the randomizer these are read once at boot and the ROM is patched in
// memory; the per-frame part only covers values the game keeps in RAM.
namespace zelda64::enhancements {
    struct Options {
        // For fun.
        //
        // Everything in the game dies to a single hit: every monster's HP is
        // set to 1 in the ROM, and Brian's max HP is held at 1 each frame so
        // it applies to a save that is already in progress.
        bool one_hit_ko = false;

        // Quality of life.
        //

        // Healing Lv2 restores what it does in the Japanese release. Its
        // potency halfword at entry +0x0C is 8 in the US ROM and 16 in the
        // Japanese one; this is the only byte that differs across the whole
        // 60-entry spell table, so it is an exact match rather than a guess.
        bool jp_healing = false;

        // Lets the bound "Exit Spell" control warp out of an area without
        // owning the spell or paying the MP. There is no menu button: the
        // binding under Controls is how it is used.
        bool exit_from_anywhere = false;
    };

    // Options as saved on disk; a missing file gives defaults.
    Options load_options();
    void save_options(const Options& options);
    // What the game booted with. The menu edits a separate copy, so ROM-level
    // changes only take effect on the next launch.
    const Options& active_options();

    // Applies the ROM-level enhancements. Called at boot after the randomizer,
    // so it patches whatever the randomizer produced.
    void apply_at_boot(uint8_t* rdram);
    // The RAM-level part, run once per frame from the cheats hook.
    void on_frame(uint8_t* rdram);

    // Warps the player to the start of the area they are in, the way the
    // Exit spell does, without needing the spell or the MP. Takes effect on
    // the next frame the game is in the field and not already transitioning.
    void cast_exit();
}

#endif
