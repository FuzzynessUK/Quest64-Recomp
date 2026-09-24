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

        // Renders the way the console did: native resolution, 4:3, no
        // antialiasing and the original HUD ratio. The graphics settings in
        // effect when it was switched on are kept so turning it off puts
        // them back rather than guessing at defaults.
        bool n64_mode = false;
        int saved_resolution = -1;
        int saved_aspect = -1;
        int saved_antialiasing = -1;
        int saved_hud_ratio = -1;

        // Run timer, shown as hh:mm:ss. Position is 0 top-left, 1 top-right,
        // 2 bottom-left, 3 bottom-right, 4 top-middle, 5 bottom-middle.
        bool speedrun_timer = false;
        int timer_position = 1;

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

        // Magic Barrier holds two turns longer. The duration is a live value
        // at 0x8007BB42, reached through a pointer rather than written by any
        // instruction we can patch, so it is topped up each frame instead: a
        // fresh cast is the only time the counter goes up, and that is when
        // the two extra turns are added.
        bool longer_magic_barrier = false;

        // Brian moves 50% faster. The per-frame velocity every movement state
        // hands to func_80005748 is scaled on the way in and restored on the
        // way out, so the game's own collision test still runs and nothing
        // the handlers read back next frame is changed. The slide after the
        // stick is released keeps its vanilla eight frames and covers 1.5x
        // the distance, as Hard Mode's faster pace does. Not stacked on Hard
        // Mode, which sets its own walk speed.
        bool faster_walk = false;

        // Stack items, Hard Mode's item menu ported to the vanilla game:
        // several of the same consumable take one place in the bag, and its
        // description ends with how many there are, "(03)". Names and the
        // item tables are the vanilla ones; see itemstack in enhancements.cpp.
        // Forced on in Archipelago. Not on top of Hard Mode, which stacks by
        // itself.
        bool stack_items = false;

        // HUD layout (the HUD group): the HP/MP block and the four spirits,
        // dragged in the tab's preview. Frame pixels, x from the window's
        // left edge; custom false is the game's own place. Applied live.
        // The eight-pixel black border the game leaves around the frame is
        // drawn over (widescreen.cpp). Applied live.
        bool remove_borders = false;
        bool hud_hp_custom = false;
        float hud_hp_x = 0.0f;
        float hud_hp_y = 0.0f;
        bool hud_sp_custom = false;
        float hud_sp_x = 0.0f;
        float hud_sp_y = 0.0f;

        // The Japanese release bursts a colour over Brian when a stat rises
        // (red HP, blue MP, yellow defense, green agility); the US build has
        // no such routine, so it is drawn by the UI overlay from what
        // statfx.cpp reports each frame.
        bool stat_up_effect = false;

        // "You have learnt <spell>" top-left for three seconds when an
        // element level reaches a spell's unlock level (include/spellnotice.h).
        bool spell_notice = false;

        // "Received <item>" when anything is added to the bag
        // (include/itemnotice.h).
        bool item_notice = false;

        // "Now playing: <song>" whenever the main music player starts a
        // track (zelda64::audio::on_frame; custom music by file name).
        // song_notice_one keeps one of them on screen at a time: a new song
        // takes the place of the one showing instead of stacking under it,
        // which is what keeps a walk through several areas - or Never
        // Expire - from filling the corner with song names. Two settings
        // behind one control, the way the timer's are.
        bool song_notice = false;
        bool song_notice_one = false;

        // Archipelago traffic, when the connector is on: 0 nothing, 1 only
        // what this slot is sent, 2 that and what is found here for someone
        // else, 3 all of that and every item passing between other players.
        // The overlay drops what the setting does not want, so this is live
        // like the rest of the Notifications group.
        int ap_notice = 1;

        // The notification stack itself (all three above). Off hides and
        // silences them; never_expire keeps messages until pushed out;
        // notify_max is how many show at once (1-10). All live.
        bool notifications = false;
        bool notify_never_expire = false;
        int notify_max = 5;
        // Where the stack sits: 0 top left, 1 top middle, 2 top right,
        // 3 left, 4 right, 5 middle, 6 bottom left, 7 bottom middle,
        // 8 bottom right. Top left shares a column with the timer.
        int notify_position = 0;

        // Hacks.
        //

        // Landmine36's Quest 64 Hard Mode, applied at boot from the embedded
        // .rup patch: its data goes into the ROM, its code payload into RAM at
        // 0x80400000, and src/game/hardmode/ supplies the hooks the hack's
        // code patches would otherwise have made. Uses its own save folder.
        bool hard_mode = false;

        // Easier Quest (include/easierquest.h): the vanilla game with a
        // fixed set of conveniences. Switched from the Mods tab. Hard Mode
        // wins when both are on. Forces jp_healing and longer_magic_barrier
        // in the boot copy.
        bool easier_quest = false;
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

    // The highest an element can be raised right now: Hard Mode's current
    // cap, or 99 with the randomizer's element uncap, or the game's 50.
    int element_cap(uint8_t* rdram);
    // Every one of Brian's four elements is at element_cap().
    bool elements_all_maxed(uint8_t* rdram);

    // How many of each item id (0-255) the bag holds, counting a stack as
    // all of its items. Works on a plain bag and a stacked one.
    void bag_counts(uint8_t* rdram, int (&counts)[256]);

    // An item list is on screen: the field item menu (menu mask bit 0) or
    // the pause screen, whose item page lists the bag (gGameMode 2, which
    // closing it turns back to 1). Nothing puts items in the bag or
    // rearranges it while this holds - Archipelago items and the Get item
    // cheats wait, and Stack items waits - so the list cannot change under
    // the cursor; it all happens the frame the menu closes.
    bool item_menu_open(uint8_t* rdram);

    // Warps the player to the start of the area they are in, the way the
    // Exit spell does, without needing the spell or the MP. Takes effect on
    // the next frame the game is in the field and not already transitioning.
    void cast_exit();
}

#endif
