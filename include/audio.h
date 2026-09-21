#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <cstdint>

// The Audio tab: settings that change what the game plays rather than how
// loud it is (volumes are the Sound tab, in the general config). Stored in
// audio.json next to the other settings and applied at boot, after the
// randomizer and the enhancements. Every draw is fresh on each launch.
namespace zelda64::audio {
    enum class MusicShuffle {
        Off,
        // The 73 area slots of the map music table (Merrow's music shuffle):
        // a ROM write, so it reaches func_8002684C's table lookup.
        Towns,
        // Towns plus everything the game starts by track number instead of
        // by map - title screen, battles and bosses, the death jingle, the
        // credits, saving - remapped as they are requested (UpdateBGM and
        // func_800267F8), each source track to one random track for the
        // session.
        All,
    };

    struct Options {
        MusicShuffle music_shuffle = MusicShuffle::Off;
        // A random permutation of the 70 sound effects, applied where every
        // effect is finally queued (func_80025B8C), so each sound is
        // consistently some other sound for the session. Every effect is
        // cut three seconds after it starts, so a looping ambience that
        // lands on a common effect cannot run on.
        bool sfx_shuffle = false;
        // Replacement music: every `track_NN.seq` in `<exe dir>/custom_music`
        // replaces track NN (decimal, the number the map music table and
        // the play requests use) for the session. A file is a compact
        // sequence in the game's own format - tools/mid2cseq.pl writes one
        // from a MIDI - and is appended to the free tail of the ROM at
        // boot, with the sequence bank's entry pointed at it, so the game
        // DMAs it like any other track. What was loaded, and what was
        // rejected and why, goes to custom_music.txt beside the settings.
        bool custom_music = false;
    };

    // Options as saved on disk; a missing file gives defaults.
    Options load_options();
    void save_options(const Options& options);

    // The options the running game booted with. Changes made in the menu
    // are saved straight away but only take effect on the next launch.
    const Options& active_options();

    void apply_at_boot(uint8_t* rdram);

    // Game thread, once per frame: with the sound effect shuffle on, stops
    // any effect three seconds after it last started, so a shuffled loop
    // cannot run on.
    void on_frame(uint8_t* rdram);
}

#endif
