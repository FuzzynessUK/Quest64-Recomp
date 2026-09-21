#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

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

    enum class CustomMusic {
        Off,
        Custom,
    };

    struct Options {
        MusicShuffle music_shuffle = MusicShuffle::Off;
        // A random permutation of the 70 sound effects, applied where every
        // effect is finally queued (func_80025B8C), so each sound is
        // consistently some other sound for the session. Every effect is
        // cut three seconds after it starts, so a looping ambience that
        // lands on a common effect cannot run on.
        bool sfx_shuffle = false;
        // Replacement music. `<exe dir>/custom_music` is a library of
        // `.seq` files (compact sequences in the game's own format;
        // tools/mid2cseq.pl writes one from a MIDI), named freely. Custom
        // plays the file chosen for each track in `custom_tracks` (the
        // menu's Randomise all fills every looping track from the library).
        // The menu shows this and music_shuffle as one control, Randomize
        // Music: Off / Towns / All / Custom. A chosen file is appended to the ROM at boot
        // (the free tail from 0xF94348, growing the ROM past 16MB if the
        // library needs it) and the sequence bank's entry pointed at it,
        // so the game DMAs it like any other track. What was loaded, and
        // what was rejected and why, goes to custom_music.txt beside the
        // settings.
        CustomMusic custom_music = CustomMusic::Off;
        // Track number -> library file name without `.seq`.
        std::map<int, std::string> custom_tracks;
    };

    // The game's 44 sequences, by the number the map music table and the
    // play requests use, with the place or event each is heard.
    constexpr int game_track_count = 44;
    const char* track_label(int track);
    // The short one-shot jingles (level up, death, victory); Randomise all
    // leaves them alone, since a looping song on the jingle player would
    // never end.
    bool track_is_jingle(int track);
    // The order the menu lists the tracks in: events first, then areas in
    // story order, then the rest.
    const int* track_menu_order();

    // What plays as a track this session: the library file's name where
    // custom music replaced it, else the game's own label.
    std::string song_name(int track);

    // Live changes from the menu, applied on the game thread next frame.
    // The library was placed in the ROM at boot (when the folder had any
    // files), so a track can be pointed at any of its files, or back at the
    // game's own, without a relaunch; a file added since is read in then.
    void apply_tracks_live(const std::map<int, std::string>& tracks);
    // Start a track from the menu (the way UpdateBGM would); -1 goes back
    // to what was playing before the first preview.
    void preview_track(int track);
    bool library_loaded();

    // The library folder and the file names (no extension) in it, sorted.
    std::filesystem::path library_folder();
    std::vector<std::string> library_files();

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
