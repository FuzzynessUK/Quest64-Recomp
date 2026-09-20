#ifndef __RANDOMIZER_H__
#define __RANDOMIZER_H__

#include <cstdint>
#include <string>
#include <vector>

// In-app port of Merrow, the Quest 64 randomizer by Jonah Davidson
// (https://github.com/hangedmandesign/merrow, MIT). The options below mirror
// Merrow's, the shuffle logic is ported from its Shuffle.cs and the ROM
// writes from QuestPatchBuild.cs. Seeds are not compatible with Merrow's.
namespace zelda64::randomizer {
    enum class Mode {
        Vanilla,
        Randomizer,
    };

    // How a list of item locations (chests, drops, gifts, wingsmiths) is
    // treated: left alone, its vanilla items shuffled among its locations, or
    // every location rolled from the whole item pool and then shuffled.
    enum class ListMode {
        Off,
        Shuffle,
        Random,
    };

    struct Options {
        Mode mode = Mode::Vanilla;
        // Free text. Numbers are used directly, anything else is hashed.
        std::string seed;

        // Spells
        bool spell_shuffle = true;
        bool hinted_spell_names = true;
        bool linear_spell_names = false;
        bool early_healing = false;
        bool extra_healing = false;
        bool distribute_spells = false;
        bool spell_overrides = false;
        bool spell_rebalance = false;
        bool bubble = false;
        bool level_1_spells = false;
        bool max_accuracy = false;
        bool max_accuracy_all = false;
        bool soul_search = false;
        bool invalidity = false;

        // Items
        ListMode chests = ListMode::Shuffle;
        ListMode drops = ListMode::Shuffle;
        ListMode gifts = ListMode::Off;
        ListMode wingsmiths = ListMode::Off;
        bool shuffle_shannon = false;
        // Keep wings (item ids 14-19) out of the random item pool, so the
        // only place they appear is a wingsmith. Affects the Random list
        // mode; Shuffle only moves items that were already in a list. Lost
        // Keys places wings deliberately and ignores this.
        bool wingsmith_wings_only = false;
        // The four gems, the Eletale Book and the Dark Gaol Key. Off keeps
        // them out of the random pool and leaves them in their vanilla
        // places, so only ordinary items move. Lost Keys places them
        // deliberately and ignores this.
        bool boss_rewards_shuffler = true;

        // Monsters
        bool monster_stats = false;
        int variance = 0;          // 0-5, Merrow's "extremity" tenths
        bool monster_scale = false;
        int scale_percent = 100;   // 50-150 in steps of 10
        bool exp_by_bst = false;
        int exp_boost = 4;         // quarters: 4 = x1, 0 = no EXP
        bool boss_order = false;
        bool boss_element = false;
        // Merrow's 5-tier encounter slider: 0 Halved, 1 Reduced, 2 Default
        // (vanilla, no-op), 3 Increased, 4 Doubled. Needs native hooks since
        // the step/roll/max values are baked into the recompiled code as C
        // literals, not read from RAM. See native_hooks.cpp.
        int encounter_rate = 2;

        // World / mechanics (Merrow options that patch code, Stage 2).
        // Walking MP regen, using Merrow's own trackbar encoding: 7 = off,
        // 8-12 = speed tiers (10 is vanilla), anything else = vanilla. The
        // speed tiers are a data byte so they go through the ROM writes; only
        // "off" needs a hook.
        int mp_regain = 10;
        // MP restored per staff hit. Vanilla is 1; Merrow allows 1-9.
        int staff_hit_mp = 1;
        // Raise the element level cap from 50 to 99.
        bool element_uncap = false;
        // Let enemies keep dropping items past the vanilla limit.
        bool drop_limit_disabled = false;
        // Wings usable indoors / on the Isle of Skye.
        bool wing_unlock_indoors = false;
        bool wing_unlock_skye = false;

        // Cosmetics. Text palette: 0 off, 1 random, 2 red, 3 blue, 4 white,
        // 5 black. Unlike Merrow these roll off the seed, so a seed always
        // gives the same colours.
        int text_palette = 0;
        bool staff_palette = false;
        // A single random colour for Brian's cloak, a hue rotation of each
        // of his two clothing texture pages, a random palette index for every
        // spell effect, and a reshuffle of the 73 background tracks.
        bool cloak_palette = false;
        bool brian_palette = false;
        bool spell_palette = false;
        bool music_shuffle = false;

        // Brian
        int start_hp = 50;
        int start_mp = 15;
        int start_agility = 5;
        int start_defense = 4;

        // World
        bool fast_monastery = false;
        bool fast_blue_cave = false;
        bool fast_shamwood = false;
        bool fast_mammon = false;
        // Open every gem/book/key progression lock, or instead move the gem
        // locks onto the final staircase.
        bool unlock_doors = false;
        bool locked_endgame = false;
        // Return warps out of Crystal Valley and Brannoch, and the useless
        // backward door in Mammon's World locked off.
        bool crystal_return = false;
        bool brannoch_return = false;
        bool mammon_door = false;
        // Quality of life: wandering NPCs, instant text, a HUD that stays put,
        // looser movement friction, and base spells from level 2.
        bool restless_npcs = false;
        bool max_message_speed = false;
        bool hud_lock = false;
        bool celtland_drift = false;
        bool level_2_spells = false;
        // Move the two hidden spirits somewhere findable, make the Dew Drop
        // a full revive, and pull the camera back a step at a time (0 off,
        // 1-4 increasingly far out).
        bool reveal_spirits = false;
        bool better_dew_drop = false;
        int zoom_out = 0;
        // White Wings become the Ivory Wings, handed over by Lavaar, and
        // Merrow's clearer save prompt replaces the vanilla one.
        bool ivory_wings = false;
        bool text_improvements = false;
        // Lost Keys: the gems and the Eletale Book come off their bosses and
        // are scattered. 0 off, 1 progressive (each gem stays inside its own
        // region so the run is completable in order), 2 open world (anything
        // anywhere, meant to be played with the progression locks opened).
        // Fire Book widens the book's pool to take in the fire region, and
        // Shannon hints tell you roughly where each gem went.
        int lost_keys = 0;
        bool fire_book = false;
        bool shannon_hints = false;
        // Merrow's table shuffle: swaps which monster set each area draws from,
        // with no regard for difficulty. Superseded by enemy_randomizer.
        bool enemy_tables = false;
        // Enemy Randomizer (DOCS/enemyrandologic.xlsx): any monster set can land
        // in any area, the encounter packs are rerolled, and every monster's
        // stats and spell damage are scaled to the area it appears in (spread
        // any/any, scaling full, as the sheet's Settings tab). One switch.
        bool enemy_randomizer = false;
    };

    // Options as saved on disk; a missing file gives defaults.
    Options load_options();
    void save_options(const Options& options);

    // Named settings presets, kept in randomizer_presets.json next to the
    // options. A preset stores every setting except the seed, so loading one
    // leaves whatever seed the menu currently has alone.
    std::vector<std::string> preset_names();
    bool save_preset(const std::string& name, const Options& options);
    bool load_preset(const std::string& name, Options& out);
    bool delete_preset(const std::string& name);
    // The options the game booted with. What the menu edits only takes effect
    // on the next launch, so the UI can show whether a restart is pending.
    const Options& active_options();

    struct Write {
        uint32_t rom_offset;
        std::vector<uint8_t> data;
    };

    struct Result {
        std::vector<Write> writes;
        std::string spoiler;
        uint32_t seed_value = 0;
        // Enemy progression: the files chosen per area and the per-area stat
        // factors the battle hooks apply (progression::Plan, kept opaque here).
        std::vector<int> progression_tables;
        // Boss order moved Beigis out of his own arena, which the native hook
        // for his map check needs to know about.
        bool beigis_moved = false;
    };

    // What the Stage 2 native hooks need to know that isn't in Options,
    // because it comes out of the shuffle rather than the menu. Filled in when
    // the patch is generated at boot; all false before that.
    struct NativeState {
        bool beigis_moved = false;
    };
    const NativeState& native_state();

    // Runs the shuffles for the given options and returns the ROM writes.
    Result generate(const Options& options);

    // Called once at boot with the ROM loaded; applies the generated patch to
    // the in-memory ROM (and to the already-copied boot segment).
    void apply_at_boot(uint8_t* rdram);

    // Summary for the UI, e.g. "Randomizer, seed 12345" or "Vanilla".
    std::string describe(const Options& options);
}

#endif
