#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <cstdint>

// The Audio tab: settings that change what the game plays rather than how
// loud it is (volumes are the Sound tab, in the general config). Stored in
// audio.json next to the other settings and applied to the in-memory ROM at
// boot, after the randomizer and the enhancements.
namespace zelda64::audio {
    struct Options {
        // Every background-music slot plays a track drawn at random. This is
        // Merrow's music shuffle, moved out of the randomizer so it works in
        // any mode; the draw is fresh on every launch rather than seeded.
        bool music_shuffle = false;
    };

    // Options as saved on disk; a missing file gives defaults.
    Options load_options();
    void save_options(const Options& options);

    // The options the running game booted with. Changes made in the menu
    // are saved straight away but only take effect on the next launch.
    const Options& active_options();

    void apply_at_boot(uint8_t* rdram);
}

#endif
