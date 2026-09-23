#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <set>
#include <numeric>
#include <optional>
#include <random>

#include "randomizer.h"
#include "easierquest.h"
#include "merrow_data.h"
#include "merrow_mapdata.h"
#include "boss_spells.h"
#include "spirit_data.h"
#include "chest_data.h"
#include "zelda_debug.h"
#include "enemy_progression.h"
#include "enemy_progression_data.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"

// The shuffle and patch logic below follows Merrow's Shuffle.cs and
// QuestPatchBuild.cs section by section; comments name the section they come
// from so the two can be compared. Merrow is copyright (c) 2021 Jonah Davidson
// (Hangedman), MIT licence.

namespace data = merrow::data;
namespace mapdata = merrow::mapdata;
namespace spirits = merrow::spirits;
namespace chests_data = merrow::chests;
using zelda64::randomizer::ListMode;
using zelda64::randomizer::Mode;
using zelda64::randomizer::Options;
using zelda64::randomizer::Write;
namespace progression = zelda64::randomizer::progression;

namespace {
    constexpr int player_spells = 60;
    constexpr int spell_start = 13941344; // ROM 0xD4BA60
    constexpr int spell_offset = 68;
    constexpr int boss_count = 7;
    constexpr int item_count = 26;
    constexpr int chest_count = 88;
    constexpr int drop_count = 67;
    constexpr int wing_count = 6;

    // The six items that cast a spell, and the spell each one casts.
    constexpr int spell_item_ids[6] = { 58, 52, 38, 57, 17, 22 };

    // Boss spells (SpellReplacement.cs, Merrow PR #6 by vbhayden). How a boss
    // spell chooses the player spell whose slot it takes over.
    enum class Replace { Specific, Buff, Debuff, Status, Damage, Any };

    struct BossSpellPlan {
        int boss;    // index into data::bossSpells
        Replace how;
        int target;  // the player spell, for Replace::Specific
    };

    // Narrower rules pick first, so a small category is not emptied by a rule
    // that would have been happy with anything.
    int plan_priority(Replace how) {
        switch (how) {
            case Replace::Specific: return 0;
            case Replace::Buff: return 1;
            case Replace::Debuff: return 2;
            case Replace::Status: return 3;
            case Replace::Damage: return 4;
            default: return 5;
        }
    }

    // The PR's own recommended mix.
    const std::vector<BossSpellPlan> boss_spells_recommended = {
        { 2, Replace::Debuff, -1 },    // Zelse's Wind Razor
        { 3, Replace::Damage, -1 },    // Zelse's Wind Zipper
        { 4, Replace::Specific, 33 },  // Nepty's Bubble Shot over Soul Search Lv1
        { 9, Replace::Damage, -1 },    // Shilf's Dove Razor
        { 6, Replace::Status, -1 },    // Fargo's Lava Ball
        { 7, Replace::Damage, -1 },    // Fargo's Explosion
        { 12, Replace::Any, -1 },      // Beigis' Spirit Sword
        { 15, Replace::Buff, -1 },     // Mammon's Flame Waves
        { 16, Replace::Any, -1 },      // Mammon's Fire Arrows
    };

    // Each boss spell over the player spell it most resembles.
    const std::vector<BossSpellPlan> boss_spells_similar = {
        { 2, Replace::Specific, 3 },   // Wind Razor over Homing Arrow Lv1
        { 3, Replace::Specific, 18 },  // Wind Zipper over Rolling Rock Lv1
        { 4, Replace::Specific, 33 },  // Bubble Shot over Soul Search Lv1
        { 9, Replace::Specific, 46 },  // Dove Razor over Wind Cutter Lv2
        { 6, Replace::Specific, 0 },   // Lava Ball over Fire Ball Lv1
        { 7, Replace::Specific, 10 },  // Explosion over Fire Bomb
        { 12, Replace::Specific, 55 }, // Spirit Sword over Cyclone
        { 15, Replace::Specific, 12 }, // Flame Waves over Magma Ball
        { 16, Replace::Specific, 9 },  // Fire Arrows over Homing Arrow Lv2
    };

    // Spirits -----------------------------------------------------------
    //
    // Where each map sits in the story, 1 (leaving Melrode) to 8 (Mammon's
    // World), for the Spirit Randomizer's balanced mode. The sixteen maps
    // the Enemy Randomizer covers keep the tier its area table gives them
    // (enemy_progression_data.cpp); the towns, castles and building sets it
    // has no entry for are placed by when the story reaches them.
    constexpr uint8_t map_story_tier[36] = {
        1, // 0  Melrode
        2, // 1  Dondoran
        1, // 2  Holy Plains
        2, // 3  Dondoran Flats
        3, // 4  Larapool
        2, // 5  West Carmagh (Greenoch)
        3, // 6  Normoon
        3, // 7  West Limelin
        3, // 8  Limelin
        6, // 9  Dindom Dries
        6, // 10 Shamwood
        7, // 11 Brannoch / Baragoon Moor
        4, // 12 Isle of Skye
        1, // 13 Melrode Monastery
        2, // 14 Dondoran Castle
        1, // 15 Melrode buildings
        2, // 16 Dondoran buildings
        3, // 17 Larapool buildings
        3, // 18 Larapool & Greenoch buildings
        3, // 19 Normoon buildings (small set)
        3, // 20 Normoon buildings
        3, // 21 Limelin Castle
        3, // 22 Limelin buildings
        6, // 23 Dindom Dries & Brannoch buildings
        6, // 24 Shamwood buildings
        4, // 25 Hidden rooms & shrines
        4, // 26 Blue Cave
        3, // 27 Cull Hazard
        5, // 28 Baragoon Tunnel (Shilf)
        6, // 29 Boil Hole (Fargo)
        7, // 30 Brannoch Castle
        1, // 31 Connor Forest (Solvaring)
        2, // 32 Glencoe Forest
        4, // 33 Windward Forest (Zelse)
        8, // 34 Mammon's World
        4, // 35 Nepty's arena (Isle of Skye)
    };

    // Exit, Escape and Return: the only way out of a battle or a dungeon, and
    // the three Merrow's crashlock table keeps out of most slots anyway. A
    // boss spell never takes one over. PR #6 does not exclude them.
    bool is_travel_spell(int spell) { return spell >= 37 && spell <= 39; }

    // Merrow's Bubble option is already Nepty's Bubble Shot over Soul Search
    // Lv1, so with it on that boss spell and that slot are both spoken for.
    constexpr int nepty_bubble_shot = 4;
    constexpr int bubble_spell = 33;

    // Merrow uses System.Random; this only needs to be reproducible with itself.
    class Rng {
    public:
        explicit Rng(uint32_t seed) : engine(seed) {}

        // [0, max)
        int next(int max) {
            if (max <= 0) {
                return 0;
            }
            return static_cast<int>(engine() % static_cast<uint32_t>(max));
        }

        // [min, max)
        int next(int min, int max) {
            return min + next(max - min);
        }

        // [0, 1)
        double next_double() {
            return std::uniform_real_distribution<double>(0.0, 1.0)(engine);
        }

        // Merrow's in-place Fisher-Yates, used on every list it shuffles.
        template <typename T>
        void shuffle(std::vector<T>& list) {
            int d = static_cast<int>(list.size());
            while (d > 1) {
                d--;
                int k = next(d + 1);
                std::swap(list[k], list[d]);
            }
        }

        // CountAndShuffleArray: 0..n-1, shuffled.
        std::vector<int> count_and_shuffle(int n) {
            std::vector<int> out(n);
            for (int i = 0; i < n; i++) {
                out[i] = i;
            }
            shuffle(out);
            return out;
        }

    private:
        std::mt19937 engine;
    };

    uint32_t seed_value_from_text(const std::string& text) {
        if (!text.empty() && std::all_of(text.begin(), text.end(), [](unsigned char c) { return std::isdigit(c); })) {
            return static_cast<uint32_t>(std::stoull(text) & 0xFFFFFFFFu);
        }
        // FNV-1a
        uint32_t hash = 2166136261u;
        for (unsigned char c : text) {
            hash ^= c;
            hash *= 16777619u;
        }
        return hash;
    }

    // C# Math.Round rounds half to even.
    int round_even(double value) {
        return static_cast<int>(std::nearbyint(value));
    }

    std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
        std::vector<uint8_t> out;
        out.reserve(hex.size() / 2);
        for (size_t i = 0; i + 1 < hex.size(); i += 2) {
            out.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
        }
        return out;
    }

    std::string hex4(int value) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%04X", value & 0xFFFF);
        return buf;
    }

    // VarFunctions.cs colour helpers, used by the cosmetic palettes. The game
    // stores colours as RGBA5551; Merrow widens them to 8 bits per channel,
    // rotates the hue and narrows them again, so the same rounding is kept
    // here to match its output.
    struct Rgba {
        int r = 0, g = 0, b = 0, a = 255;
    };

    int clamp_255(double value) {
        return std::min(255, std::max(0, static_cast<int>(value)));
    }

    Rgba rgba5551_to_rgba(const std::string& hex) {
        unsigned int packed = static_cast<unsigned int>(std::stoul(hex, nullptr, 16)) & 0xFFFF;
        Rgba out;
        out.r = static_cast<int>(std::lround(((packed >> 11) & 0x1F) / 31.0 * 255.0));
        out.g = static_cast<int>(std::lround(((packed >> 6) & 0x1F) / 31.0 * 255.0));
        out.b = static_cast<int>(std::lround(((packed >> 1) & 0x1F) / 31.0 * 255.0));
        out.a = (packed & 1) ? 255 : 0;
        return out;
    }

    std::string rgba_to_rgba5551(const Rgba& col) {
        int r = static_cast<int>(std::lround(col.r / 255.0 * 31.0));
        int g = static_cast<int>(std::lround(col.g / 255.0 * 31.0));
        int b = static_cast<int>(std::lround(col.b / 255.0 * 31.0));
        int packed = (r << 11) | (g << 6) | (b << 1) | (col.a == 0 ? 0 : 1);
        return hex4(packed);
    }

    // Merrow's HueShift: rotate about the grey axis by the given degrees.
    Rgba hue_shift(const Rgba& col, double degrees) {
        double radians = degrees * std::acos(-1.0) / 180.0;
        double cos_a = std::cos(radians);
        double sin_a = std::sin(radians);
        double third = 1.0 / 3.0;
        double root = std::sqrt(third);
        double m0 = cos_a + (1.0 - cos_a) * third;
        double m1 = third * (1.0 - cos_a) - root * sin_a;
        double m2 = third * (1.0 - cos_a) + root * sin_a;

        Rgba out;
        out.r = clamp_255(col.r * m0 + col.g * m1 + col.b * m2);
        out.g = clamp_255(col.r * m2 + col.g * m0 + col.b * m1);
        out.b = clamp_255(col.r * m1 + col.g * m2 + col.b * m0);
        out.a = clamp_255(col.a);
        return out;
    }

    // Merrow only ever rotates the hue, which preserves each texture's
    // saturation and lightness: a muted palette stays muted and the result
    // only ever reaches colours that sit on the source's own cylinder. With
    // the full-spectrum option a saturation multiplier and a lightness offset
    // are rolled alongside the hue, so a palette can land anywhere in the RGB
    // cube. All three are rolled once per palette, not per colour, so the
    // texture's internal shading survives.
    struct ColourRoll {
        double hue = 0.0;
        double saturation = 1.0;
        double lightness = 0.0;

        bool plain_hue() const {
            return saturation == 1.0 && lightness == 0.0;
        }

        std::string describe() const {
            std::string out = std::to_string(static_cast<int>(std::lround(hue)));
            if (!plain_hue()) {
                out += ", sat x" + std::to_string(static_cast<int>(std::lround(saturation * 100.0))) + "%";
                out += ", light " + std::to_string(static_cast<int>(std::lround(lightness * 100.0))) + "%";
            }
            return out;
        }
    };

    Rgba apply_roll(const Rgba& col, const ColourRoll& roll) {
        Rgba out = hue_shift(col, roll.hue);
        if (roll.plain_hue()) {
            return out;
        }
        // Saturation is scaled about the colour's own luma so black and white
        // stay put, then the whole thing is slid up or down in lightness.
        double luma = 0.299 * out.r + 0.587 * out.g + 0.114 * out.b;
        auto adjust = [&](int channel) {
            double value = luma + (channel - luma) * roll.saturation + roll.lightness * 255.0;
            return clamp_255(std::lround(value));
        };
        out.r = adjust(out.r);
        out.g = adjust(out.g);
        out.b = adjust(out.b);
        return out;
    }

    // Merrow's TranslateString: ASCII to the game's text encoding. Digits,
    // upper and lower case each live on a row selected by a prefix byte;
    // punctuation and the #/$/% controls come from two lookup tables.
    std::optional<std::vector<uint8_t>> translate_string(const std::string& text) {
        std::vector<uint8_t> out;
        int last_row = -1;
        auto row = [&](int r) {
            if (last_row != r) {
                last_row = r;
                out.push_back(static_cast<uint8_t>(r));
            }
        };
        for (unsigned char c : text) {
            if (c >= '0' && c <= '9') {
                row(0x80);
                out.push_back(c - '0');
            }
            else if (c >= 'A' && c <= 'Z') {
                row(0x81);
                out.push_back(c - 'A');
            }
            else if (c >= 'a' && c <= 'z') {
                row(0x82);
                out.push_back(c - 'a');
            }
            else {
                bool ok = false;
                for (size_t i = 0; i + 2 < data::punctuationvals.size(); i += 3) {
                    if (c == data::punctuationvals[i]) {
                        // The row here is written as its decimal digits ("80"),
                        // which is the hex byte 0x80.
                        row(std::stoi(std::to_string(data::punctuationvals[i + 1]), nullptr, 16));
                        out.push_back(static_cast<uint8_t>(data::punctuationvals[i + 2]));
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    for (size_t i = 0; i + 1 < data::specialvals.size(); i += 2) {
                        if (c == data::specialvals[i]) {
                            out.push_back(static_cast<uint8_t>(data::specialvals[i + 1]));
                            ok = true;
                            break;
                        }
                    }
                }
                if (!ok) {
                    return std::nullopt;
                }
            }
        }
        return out;
    }

    struct Builder {
        const Options& options;
        Rng rng;
        std::vector<Write> writes;
        std::string spoiler;

        // Working copies of the tables Merrow edits in place.
        std::vector<std::string> spells = data::spells;
        std::vector<std::vector<std::string>> spell_names = data::shuffleNames2;
        std::vector<int> crashlock = data::crashlock;
        std::vector<std::string> item_capital_case = data::itemcapitalcase;
        std::vector<std::string> new_spell_item_desc = data::newSpellItemDesc;

        // Shuffle results.
        std::vector<int> shuffles = std::vector<int>(player_spells, -1);
        std::vector<int> new_item_spells = std::vector<int>(6, 0);
        std::vector<int> item_spell_fix = std::vector<int>(6, 0);
        // Which boss spell (an index into data::bossSpells) took over each
        // player spell's slot, -1 where the slot is still the player's own.
        std::vector<int> boss_spell_slots = std::vector<int>(player_spells, -1);
        std::vector<std::string> hint_names = std::vector<std::string>(player_spells);
        std::vector<int> chests = std::vector<int>(chest_count);
        std::vector<int> drops = std::vector<int>(drop_count);
        std::vector<int> gifts;
        std::vector<int> wings = std::vector<int>(wing_count);
        std::vector<int> monster_stats = data::monsterstatvanilla;
        std::vector<int> boss_order = std::vector<int>(boss_count);
        int guilty_element = 4;
        // Lost Keys: what each boss now carries, the gem order the hints
        // name, where each gem landed, and the ten coin flips the hint
        // wording uses.
        std::vector<int> lk_boss_items = std::vector<int>(boss_count, 255);
        std::vector<int> gem_ids = { 20, 21, 22, 23, 24 };
        std::vector<int> hints = std::vector<int>(5, -1);
        std::vector<int> hint_coins = std::vector<int>(10, 0);
        // Working copies of the encounter tables, and what the shared-pack
        // clamps had to do.
        std::vector<mapdata::Area> enemy_areas;
        std::vector<mapdata::MonsterPack> enemy_packs;
        std::vector<mapdata::Region> enemy_regions;
        std::string enemy_group_notes;
        // Enemy progression: the file each of the 16 merged areas was given.
        std::vector<int> progression_tables;
        // The Spirit Randomizer's plan, one list per table slot.
        std::vector<std::vector<zelda64::randomizer::SpiritPlacement>> spirit_slots;
        // The Chest Randomizer's plan, in chest id order.
        std::vector<zelda64::randomizer::ChestPlacement> chest_placements;
        bool beigis_moved = false;

        Builder(const Options& opts, uint32_t seed) : options(opts), rng(seed) {}

        double difficulty_scale() const {
            return options.scale_percent / 100.0;
        }

        double extremity() const {
            return options.variance * 0.1;
        }

        // One roll per palette, across the whole RGB cube: hue, then a
        // saturation multiplier (0.4x-2.2x) and a lightness offset (+/-25%),
        // which together reach pastels, neons and near-monochrome. Merrow
        // only ever rotated hue, which could not leave the source palette's
        // own saturation and lightness.
        ColourRoll roll_colour() {
            ColourRoll roll;
            roll.hue = rng.next_double() * 360.0;
            roll.saturation = 0.4 + rng.next_double() * 1.8;
            roll.lightness = (rng.next_double() - 0.5) * 0.5;
            return roll;
        }

        void add(uint32_t rom_offset, std::vector<uint8_t> bytes) {
            writes.push_back({ rom_offset, std::move(bytes) });
        }

        static uint32_t hex_addr(const std::string& rom_offset_hex) {
            return static_cast<uint32_t>(std::stoul(rom_offset_hex, nullptr, 16));
        }

        void add_hex(const std::string& rom_offset_hex, const std::string& data_hex) {
            add(hex_addr(rom_offset_hex), hex_to_bytes(data_hex));
        }

        void add_u16(uint32_t rom_offset, int value) {
            add(rom_offset, { static_cast<uint8_t>((value >> 8) & 0xFF), static_cast<uint8_t>(value & 0xFF) });
        }

        void add_u8(uint32_t rom_offset, int value) {
            add(rom_offset, { static_cast<uint8_t>(value & 0xFF) });
        }

        void log(const std::string& line) {
            spoiler += line;
            spoiler += '\n';
        }

        const std::string& item_name(int id) const {
            // 255 is the game's "no drop".
            static const std::string nothing = "nothing";
            size_t index = static_cast<size_t>(id) * 3;
            return index < data::items.size() ? data::items[index] : nothing;
        }

        // Shuffle.cs -------------------------------------------------------

        // Shuffle.cs's Lost Keys rulesets.
        //
        // Lost Keys takes the four gems and the Eletale Book off their bosses
        // and scatters them across the world along with the six pairs of
        // wings. A placement is an index into one of the area_* tables: the
        // low values mean a boss, the middle a chest and the top a gift, with
        // the exact ranges differing per region. Progressive keeps each gem
        // inside its own region so the run stays completable in order; Open
        // World puts anything anywhere and moves the progression locks to the
        // endgame instead.
        void place_lost_keys() {
            if (options.lost_keys == 0) {
                return;
            }

            auto put = [&](const std::vector<int>& area, int value, int item, bool is_gift) {
                size_t slot = static_cast<size_t>(area[static_cast<size_t>(value)]);
                if (is_gift) {
                    if (slot < gifts.size()) {
                        gifts[slot] = item;
                    }
                }
                else if (slot < chests.size()) {
                    chests[slot] = item;
                }
            };

            if (options.lost_keys == 1) {
                // Progressive. Each gem travels with the wings of its own
                // region. Ivory Wings moves the White Wings from earth to
                // fire, which changes both lists' lengths.
                std::vector<int> earth_items = { 20, 14, 15 };
                std::vector<int> wind_items = { 21, 16, 17 };
                std::vector<int> fire_items = { 23, 18, 19 };
                if (options.ivory_wings) {
                    earth_items = { 20, 15 };
                    fire_items = { 23, 14, 18, 19 };
                }

                // Shuffled so the items within a region can never collide.
                std::vector<int> earth_locs = rng.count_and_shuffle(19);
                std::vector<int> wind_locs = rng.count_and_shuffle(18);
                std::vector<int> fire_locs = rng.count_and_shuffle(37);

                std::vector<int> wind_vals(wind_items.size());
                for (size_t i = 0; i < wind_vals.size(); i++) {
                    wind_vals[i] = wind_locs[i];
                }
                std::vector<int> fire_vals(fire_items.size());
                for (size_t i = 0; i < fire_vals.size(); i++) {
                    fire_vals[i] = fire_locs[i];
                }

                // Water and the book are single items, so they are rolled
                // directly and re-rolled off anything they would land on. The
                // water pool's first entries are the wind region's.
                int water_val = rng.next(26);
                while (std::find(wind_vals.begin(), wind_vals.end(), water_val) != wind_vals.end()) {
                    water_val = rng.next(26);
                }
                int book_val = 0;
                if (!options.fire_book) {
                    book_val = rng.next(22);
                }
                else {
                    book_val = rng.next(58);
                    while (std::find(fire_vals.begin(), fire_vals.end(), book_val) != fire_vals.end()) {
                        book_val = rng.next(58);
                    }
                }

                for (size_t i = 0; i < earth_items.size(); i++) {
                    int value = earth_locs[i];
                    int item = earth_items[i];
                    if (value == 0) { lk_boss_items[0] = item; }          // Solvaring
                    else if (value <= 15) { put(data::area_earth, value, item, false); }
                    else if (value <= 18) { put(data::area_earth, value, item, true); }
                    if (hints[0] == -1) { hints[0] = value; }
                }
                for (size_t i = 0; i < wind_items.size(); i++) {
                    int value = wind_vals[i];
                    int item = wind_items[i];
                    if (value == 0) { lk_boss_items[1] = item; }          // Zelse
                    else if (value <= 15) { put(data::area_wind, value, item, false); }
                    else if (value <= 17) { put(data::area_wind, value, item, true); }
                    if (hints[1] == -1) { hints[1] = value; }
                }
                {
                    int value = water_val;
                    if (value == 0) { lk_boss_items[1] = 22; }            // Zelse
                    else if (value <= 15) { put(data::area_water_nowings, value, 22, false); }
                    else if (value <= 17) { put(data::area_water_nowings, value, 22, true); }
                    else if (value == 18) { lk_boss_items[2] = 22; }      // Nepty
                    else if (value <= 25) { put(data::area_water_nowings, value, 22, false); }
                    hints[2] = value;
                }
                for (size_t i = 0; i < fire_items.size(); i++) {
                    int value = fire_vals[i];
                    int item = fire_items[i];
                    if (value == 0) { lk_boss_items[3] = item; }          // Shilf
                    else if (value == 1) { lk_boss_items[4] = item; }     // Fargo
                    else if (value <= 34) { put(data::area_fire, value, item, false); }
                    else if (value <= 36) { put(data::area_fire, value, item, true); }
                    if (hints[3] == -1) { hints[3] = value; }
                }
                {
                    int value = book_val;
                    if (!options.fire_book) {
                        if (value == 0) { lk_boss_items[5] = 24; }        // Guilty
                        else if (value == 1) { lk_boss_items[6] = 24; }   // Beigis
                        else if (value <= 19) { put(data::area_book, value, 24, false); }
                        else if (value <= 21) { put(data::area_book, value, 24, true); }
                        hints[4] = value == 1 ? 1 : value;
                    }
                    else {
                        if (value == 0) { lk_boss_items[3] = 24; }        // Shilf
                        else if (value == 1) { lk_boss_items[4] = 24; }   // Fargo
                        else if (value <= 34) { put(data::area_fire, value, 24, false); }
                        else if (value <= 36) { put(data::area_fire, value, 24, true); }
                        else if (value == 37) { lk_boss_items[5] = 24; }  // Guilty
                        else if (value == 38) { lk_boss_items[6] = 24; }  // Beigis
                        else if (value <= 56) { put(data::area_bookf_beigis_nowings, value, 24, false); }
                        else if (value <= 58) { put(data::area_bookf_beigis_nowings, value, 24, true); }
                        hints[4] = value == 38 ? 1 : value;
                    }
                }
            }
            else {
                // Open World. One pool covers every location in the game; the
                // wings take the first six entries and each gem then takes the
                // next entry inside its own region's range, which is what keeps
                // the Shannon hints meaningful.
                std::vector<int> pool = rng.count_and_shuffle(104);
                std::vector<int> wing_ids = { 14, 15, 16, 17, 18, 19 };
                std::vector<int> wing_vals(pool.begin(), pool.begin() + 6);

                rng.shuffle(gem_ids);
                std::vector<int> gem_vals(5, -1);
                for (size_t i = 6; i < pool.size(); i++) {
                    int value = pool[i];
                    if (gem_vals[0] == -1 && value <= 18) {
                        gem_vals[0] = value;
                        hints[0] = value;
                    }
                    if (gem_vals[1] == -1 && value >= 19 && value <= 36 && value != gem_vals[2]) {
                        gem_vals[1] = value;
                        hints[1] = value - 19;
                    }
                    if (gem_vals[2] == -1 && value >= 19 && value <= 44 && value != gem_vals[1]) {
                        gem_vals[2] = value;
                        hints[2] = value - 19;
                    }
                    if (!options.fire_book) {
                        if (gem_vals[3] == -1 && value >= 45 && value <= 81) {
                            gem_vals[3] = value;
                            hints[3] = value - 45;
                        }
                        if (gem_vals[4] == -1 && value >= 82 && value <= 103) {
                            gem_vals[4] = value;
                            hints[4] = value - 82;
                        }
                    }
                    else {
                        if (gem_vals[3] == -1 && value >= 45 && value <= 81 && value != gem_vals[4]) {
                            gem_vals[3] = value;
                            hints[3] = value - 45;
                        }
                        if (gem_vals[4] == -1 && value >= 45 && value <= 103 && value != gem_vals[3]) {
                            gem_vals[4] = value;
                            hints[4] = value - 45;
                        }
                    }
                    if (std::find(gem_vals.begin(), gem_vals.end(), -1) == gem_vals.end()) {
                        break;
                    }
                }

                auto place_open = [&](int value, int item) {
                    if (value < 0) { return; }
                    if (value == 0) { lk_boss_items[0] = item; }          // Solvaring
                    else if (value <= 15) { put(data::area_open_beigis, value, item, false); }
                    else if (value <= 18) { put(data::area_open_beigis, value, item, true); }
                    else if (value == 19) { lk_boss_items[1] = item; }    // Zelse
                    else if (value <= 34) { put(data::area_open_beigis, value, item, false); }
                    else if (value <= 36) { put(data::area_open_beigis, value, item, true); }
                    else if (value == 37) { lk_boss_items[2] = item; }    // Nepty
                    else if (value <= 44) { put(data::area_open_beigis, value, item, false); }
                    else if (value == 45) { lk_boss_items[3] = item; }    // Shilf
                    else if (value == 46) { lk_boss_items[4] = item; }    // Fargo
                    else if (value <= 79) { put(data::area_open_beigis, value, item, false); }
                    else if (value <= 81) { put(data::area_open_beigis, value, item, true); }
                    else if (value == 82) { lk_boss_items[5] = item; }    // Guilty
                    else if (value == 83) { lk_boss_items[6] = item; }    // Beigis
                    else if (value <= 101) { put(data::area_open_beigis, value, item, false); }
                    else if (value <= 103) { put(data::area_open_beigis, value, item, true); }
                };

                for (size_t i = 0; i < wing_vals.size(); i++) {
                    place_open(wing_vals[i], wing_ids[i]);
                }
                for (size_t i = 0; i < gem_vals.size(); i++) {
                    place_open(gem_vals[i], gem_ids[i]);
                }
            }

            for (int& coin : hint_coins) {
                coin = rng.next(2);
            }
        }

        // SpellReplacement.cs (Merrow PR #6) --------------------------------

        // A boss spell's passive Invalidity, which strips buffs and debuffs,
        // would follow it to Brian: Merrow's own switch for it writes to the
        // boss's copy of the record, which nothing reads once the spell has
        // moved.
        static bool carries_invalidity(const data::BossSpell& boss) {
            for (const std::string& location : data::invalidityLocations) {
                if (hex_addr(location) == boss.data_address + 0x11) {
                    return true;
                }
            }
            return false;
        }

        // Bubble: Nepty's bubble spell over Soul Search Lv1. Merrow's own
        // hand-made version of the boss spell replacement below - the record
        // is Nepty's with Soul Search Lv1's unlock level, menu position and
        // rule byte, and the damage nerfed from 200 to 166 - so it goes in
        // the working table the same way and the shuffle carries it.
        void place_bubble() {
            if (!options.bubble) {
                return;
            }
            spells[(bubble_spell * 4) + 3] = data::ss1bubble[0];
            spells[bubble_spell * 4] = "Bubble";
            for (int word = 0; word < 4; word++) {
                spell_names[bubble_spell][word] = data::bossSpellShuffleNames[nepty_bubble_shot][word];
            }
        }

        // Give Brian the bosses' own spells. Each one takes over a player
        // spell's slot: its whole record, and its animation, are copied over
        // that spell's, keeping the slot's unlock level (bytes 0-1) and menu
        // position (bytes 4-9), so it turns up where the spell it replaced
        // did and that spell is gone. Everything after this point - the
        // shuffle, the hinted names, the spell-item rules - reads the slot as
        // though it had always held the boss spell.
        void place_boss_spells() {
            if (options.boss_spells == 0) {
                return;
            }

            std::vector<BossSpellPlan> plan;
            if (options.boss_spells == 1) {
                plan = boss_spells_recommended;
            }
            else if (options.boss_spells == 2) {
                plan = boss_spells_similar;
            }
            else {
                for (size_t i = 0; i < data::bossSpells.size(); i++) {
                    if (data::bossSpells[i].castable) {
                        plan.push_back({ static_cast<int>(i), Replace::Any, -1 });
                    }
                }
            }
            if (options.bubble) {
                plan.erase(std::remove_if(plan.begin(), plan.end(),
                    [](const BossSpellPlan& entry) { return entry.boss == nepty_bubble_shot; }), plan.end());
            }
            std::stable_sort(plan.begin(), plan.end(), [](const BossSpellPlan& a, const BossSpellPlan& b) {
                return plan_priority(a.how) < plan_priority(b.how);
            });

            std::vector<int> available;
            for (int i = 0; i < player_spells; i++) {
                if (is_travel_spell(i) || (options.bubble && i == bubble_spell)) {
                    continue;
                }
                available.push_back(i);
            }
            auto take = [&](int spell) {
                auto it = std::find(available.begin(), available.end(), spell);
                if (it == available.end()) {
                    return false;
                }
                available.erase(it);
                return true;
            };
            // The first spell of a category that is still free, in a random
            // order. Merrow falls back to the offensive spells when the
            // category is used up, and we fall back again to anything left.
            auto pick = [&](const std::vector<int>& pool) {
                std::vector<int> order = pool;
                rng.shuffle(order);
                for (int spell : order) {
                    if (take(spell)) {
                        return spell;
                    }
                }
                return -1;
            };

            for (const BossSpellPlan& entry : plan) {
                const data::BossSpell& boss = data::bossSpells[entry.boss];
                int target = -1;
                switch (entry.how) {
                    // Merrow leaves a named spell in the pool, so a later
                    // random pick can land on it too and the first boss spell
                    // is lost; we take it out.
                    case Replace::Specific: target = take(entry.target) ? entry.target : -1; break;
                    case Replace::Buff: target = pick(data::buffSpells); break;
                    case Replace::Debuff: target = pick(data::debuffSpells); break;
                    case Replace::Status: target = pick(data::statusspells); break;
                    case Replace::Damage: target = pick(data::offenseSpells); break;
                    default: target = pick(available); break;
                }
                if (target < 0) {
                    target = pick(data::offenseSpells);
                }
                if (target < 0) {
                    target = pick(available);
                }
                if (target < 0) {
                    break;  // every slot is spoken for
                }

                boss_spell_slots[target] = entry.boss;
                std::string record = boss.data;
                record.replace(0, 4, spells[(target * 4) + 3].substr(0, 4));
                record.replace(8, 12, spells[(target * 4) + 3].substr(8, 12));
                if (options.invalidity && carries_invalidity(boss)) {
                    record.replace(34, 2, "00");
                }
                spells[(target * 4) + 3] = record;
                // The spoiler and the hinted names follow the spell, not the
                // slot, so both are renamed to the boss spell here.
                spells[target * 4] = boss.name;
                for (int word = 0; word < 4; word++) {
                    spell_names[target][word] = data::bossSpellShuffleNames[entry.boss][word];
                }
            }
        }

        // Spirits -----------------------------------------------------------

        // Move the 98 spirits about. Only positions the game already stands
        // something on are used - a vanilla spirit spot, or the spot Brian
        // appears on when he walks into a submap - so every spirit is on
        // solid ground, in bounds, in a room something leads to. The game
        // drops each one onto the floor at its x and z itself, so no height
        // is chosen here.
        //
        // The table func_80012220 reads has 43 slots and the loop bound is a
        // code literal, so a plan may use at most 43 (map, submap) groups;
        // which map and submap each slot names is data, so they can be any
        // 43. The writing is quest64_randomizer_spirits, a native hook: the
        // records live in each map's own data, which a ROM patch could only
        // rewrite in place, and the point of this is to move them.
        void shuffle_spirits() {
            if (options.spirit_shuffle == 0) {
                return;
            }

            // Spots by submap, and the vanilla counts we have to match.
            std::map<std::pair<int, int>, std::vector<const spirits::Spot*>> by_submap;
            for (const spirits::Spot& spot : spirits::spots) {
                by_submap[{ spot.map, spot.submap }].push_back(&spot);
            }
            std::map<int, int> vanilla_by_map;
            std::map<int, int> slots_by_map;
            std::map<int, int> vanilla_by_tier;
            std::map<int, int> slots_by_tier;
            for (const spirits::Slot& slot : spirits::vanilla_slots) {
                vanilla_by_map[slot.map] += slot.count;
                slots_by_map[slot.map] += 1;
                int tier = map_story_tier[slot.map];
                vanilla_by_tier[tier] += slot.count;
                slots_by_tier[tier] += 1;
            }

            // Take `wanted` spots out of `pool`, using at most `submap_cap`
            // different submaps: the table has only 43 slots to name them in.
            //
            // The submaps are chosen before the spots, because most of them
            // hold one or two spots and a run of small ones picked at random
            // would not have room for the spirits. So: shuffle, take the
            // first `submap_cap`, then while they cannot hold `wanted`, trade
            // the smallest one in hand for the largest one left.
            auto draw = [&](const std::vector<const spirits::Spot*>& pool, int wanted, int submap_cap,
                            std::map<std::pair<int, int>, std::vector<const spirits::Spot*>>& into) {
                std::map<std::pair<int, int>, std::vector<const spirits::Spot*>> groups;
                for (const spirits::Spot* spot : pool) {
                    groups[{ spot->map, spot->submap }].push_back(spot);
                }
                std::vector<std::pair<int, int>> keys;
                for (const auto& group : groups) {
                    keys.push_back(group.first);
                }
                rng.shuffle(keys);
                if (static_cast<int>(keys.size()) > submap_cap) {
                    std::vector<std::pair<int, int>> chosen(keys.begin(), keys.begin() + submap_cap);
                    std::vector<std::pair<int, int>> rest(keys.begin() + submap_cap, keys.end());
                    auto capacity = [&](const std::vector<std::pair<int, int>>& list) {
                        int total = 0;
                        for (const std::pair<int, int>& key : list) {
                            total += static_cast<int>(groups[key].size());
                        }
                        return total;
                    };
                    auto bigger = [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                        return groups[a].size() < groups[b].size();
                    };
                    while (capacity(chosen) < wanted && !rest.empty()) {
                        auto smallest = std::min_element(chosen.begin(), chosen.end(), bigger);
                        auto largest = std::max_element(rest.begin(), rest.end(), bigger);
                        if (groups[*largest].size() <= groups[*smallest].size()) {
                            break;
                        }
                        std::swap(*smallest, *largest);
                    }
                    keys = chosen;
                }

                std::vector<const spirits::Spot*> order;
                for (const std::pair<int, int>& key : keys) {
                    order.insert(order.end(), groups[key].begin(), groups[key].end());
                }
                rng.shuffle(order);
                int taken = 0;
                for (const spirits::Spot* spot : order) {
                    if (taken >= wanted) {
                        break;
                    }
                    into[{ spot->map, spot->submap }].push_back(spot);
                    taken++;
                }
                return taken;
            };

            std::map<std::pair<int, int>, std::vector<const spirits::Spot*>> placed;
            if (options.spirit_shuffle == 1) {
                // Same map: a map keeps the spirits it had, spread over as
                // many of its submaps as it used before.
                for (const std::pair<const int, int>& entry : vanilla_by_map) {
                    std::vector<const spirits::Spot*> pool;
                    for (const spirits::Spot& spot : spirits::spots) {
                        if (spot.map == entry.first) {
                            pool.push_back(&spot);
                        }
                    }
                    draw(pool, entry.second, slots_by_map[entry.first], placed);
                }
            }
            else if (options.spirit_shuffle == 3) {
                // Balanced: a spirit can go anywhere, but each stretch of the
                // story keeps the number it had, so the run is never starved
                // early or back-loaded.
                for (const std::pair<const int, int>& entry : vanilla_by_tier) {
                    std::vector<const spirits::Spot*> pool;
                    for (const spirits::Spot& spot : spirits::spots) {
                        if (map_story_tier[spot.map] == entry.first) {
                            pool.push_back(&spot);
                        }
                    }
                    draw(pool, entry.second, slots_by_tier[entry.first], placed);
                }
            }
            else {
                // Anywhere.
                std::vector<const spirits::Spot*> pool;
                for (const spirits::Spot& spot : spirits::spots) {
                    pool.push_back(&spot);
                }
                draw(pool, spirits::spirit_count, spirits::slot_count, placed);
            }

            // A plan that came out short would lose spirits, which no option
            // is allowed to do; fall back to the vanilla placement.
            int total = 0;
            for (const std::pair<const std::pair<int, int>, std::vector<const spirits::Spot*>>& group : placed) {
                total += static_cast<int>(group.second.size());
            }
            if (total != spirits::spirit_count || static_cast<int>(placed.size()) > spirits::slot_count) {
                log("Spirits: the plan came out with " + std::to_string(total) + " of " +
                    std::to_string(spirits::spirit_count) + " in " + std::to_string(placed.size()) +
                    " groups; left where they were.");
                return;
            }

            for (const std::pair<const std::pair<int, int>, std::vector<const spirits::Spot*>>& group : placed) {
                std::vector<zelda64::randomizer::SpiritPlacement> slot;
                for (const spirits::Spot* spot : group.second) {
                    slot.push_back({ static_cast<uint8_t>(group.first.first), static_cast<uint8_t>(group.first.second),
                                     spot->x, spot->z });
                }
                spirit_slots.push_back(std::move(slot));
            }
        }

        // Chests -------------------------------------------------------------

        // Move the 88 chests. Unlike a spirit, a chest has a front: it needs
        // to face away from whatever is behind it and have floor in front for
        // Brian to stand on. chest_data.cpp only offers spots where both can
        // be justified - the game's own chest spots, spirit spots clear of a
        // doorway, and points along the line between two known-walkable
        // points outdoors - and carries the facing and Brian's position with
        // each one, so nothing is worked out here but which chest goes where.
        //
        // The outer table has 19 map slots and the loop bound is a code
        // literal, so a plan may use at most 19 maps; which map each slot
        // names is data. Writing is quest64_randomizer_chests.
        void shuffle_chests() {
            if (options.chest_shuffle == 0) {
                return;
            }
            bool outdoors = options.chest_shuffle == 2;

            std::map<int, std::vector<const chests_data::Spot*>> by_map;
            for (const chests_data::Spot& spot : chests_data::spots) {
                if (outdoors && !spot.outdoor) {
                    continue;
                }
                by_map[spot.map].push_back(&spot);
            }
            std::vector<int> maps;
            for (const std::pair<const int, std::vector<const chests_data::Spot*>>& group : by_map) {
                maps.push_back(group.first);
            }
            rng.shuffle(maps);

            // At most 19 maps, and between them they have to hold 88 chests,
            // so trade the smallest in hand for the largest left until they
            // do - the same problem the spirit slots have.
            if (static_cast<int>(maps.size()) > chests_data::map_slots) {
                std::vector<int> chosen(maps.begin(), maps.begin() + chests_data::map_slots);
                std::vector<int> rest(maps.begin() + chests_data::map_slots, maps.end());
                auto capacity = [&](const std::vector<int>& list) {
                    int total = 0;
                    for (int map : list) {
                        total += static_cast<int>(by_map[map].size());
                    }
                    return total;
                };
                auto bigger = [&](int a, int b) { return by_map[a].size() < by_map[b].size(); };
                while (capacity(chosen) < chests_data::chest_count && !rest.empty()) {
                    auto smallest = std::min_element(chosen.begin(), chosen.end(), bigger);
                    auto largest = std::max_element(rest.begin(), rest.end(), bigger);
                    if (by_map[*largest].size() <= by_map[*smallest].size()) {
                        break;
                    }
                    std::swap(*smallest, *largest);
                }
                maps = chosen;
            }

            std::vector<const chests_data::Spot*> pool;
            for (int map : maps) {
                pool.insert(pool.end(), by_map[map].begin(), by_map[map].end());
            }

            // Outdoors there are only 68 spots for 88 chests, so the ones
            // that do not fit stay exactly where they are. Which chests move
            // is drawn at random; a chest that stays keeps its own spot, so
            // that spot has to come out of the pool before anything else is
            // put on it.
            std::vector<int> order(chests_data::chest_count);
            std::iota(order.begin(), order.end(), 0);
            rng.shuffle(order);
            int movable = std::min(static_cast<int>(pool.size()), chests_data::chest_count);
            std::vector<bool> moves(chests_data::chest_count, false);
            for (int i = 0; i < movable; i++) {
                moves[order[i]] = true;
            }
            if (movable < chests_data::chest_count) {
                pool.erase(std::remove_if(pool.begin(), pool.end(), [&](const chests_data::Spot* spot) {
                    for (const chests_data::Chest& chest : chests_data::chests) {
                        if (moves[chest.id]) {
                            continue;
                        }
                        if (chest.map == spot->map && chest.submap == spot->submap &&
                            chest.x == spot->x && chest.z == spot->z) {
                            return true;
                        }
                    }
                    return false;
                }), pool.end());
                log("Chests: " + std::to_string(pool.size()) + " spots for " +
                    std::to_string(chests_data::chest_count) + " chests, so " +
                    std::to_string(chests_data::chest_count - static_cast<int>(pool.size())) +
                    " stay where they are.");
            }
            rng.shuffle(pool);

            size_t next = 0;
            for (int i = 0; i < chests_data::chest_count; i++) {
                const chests_data::Chest& chest = chests_data::chests[i];
                // The item comes from the item shuffle when it is on; its
                // list is rolled either way, so vanilla has to be read back
                // from the chest itself rather than taken from there.
                int item = options.chests != ListMode::Off ? chests[i] : chest.item;
                if (moves[i] && next < pool.size()) {
                    const chests_data::Spot* spot = pool[next++];
                    chest_placements.push_back({ spot->map, spot->submap, chest.id, static_cast<uint8_t>(item),
                                                 spot->x, spot->z, spot->facing, spot->ox, spot->oz });
                }
                else {
                    chest_placements.push_back({ chest.map, chest.submap, chest.id, static_cast<uint8_t>(item),
                                                 chest.x, chest.z, chest.facing, chest.ox, chest.oz });
                }
            }
        }

        void patch_chests() {
            if (chest_placements.empty()) {
                return;
            }
            log("");
            log(std::string("CHEST LOCATIONS (") +
                (options.chest_shuffle == 2 ? "outdoors only" : "anywhere") + "):");
            std::map<int, int> per_map;
            for (const zelda64::randomizer::ChestPlacement& chest : chest_placements) {
                per_map[chest.map]++;
            }
            for (const std::pair<const int, int>& entry : per_map) {
                log("  " + std::string(zelda64::map_name(entry.first)) + ": " + std::to_string(entry.second) +
                    (entry.second == 1 ? " chest" : " chests"));
            }
            for (const zelda64::randomizer::ChestPlacement& chest : chest_placements) {
                // Name the kind of spot it landed on: an interpolated one put
                // a chest inside a fence in Melrode, so which sort a chest is
                // standing on is the first thing worth knowing about it.
                const char* kind = "moved";
                const chests_data::Chest& home = chests_data::chests[chest.id];
                if (home.map == chest.map && home.submap == chest.submap &&
                    home.x == chest.x && home.z == chest.z) {
                    kind = "left alone";
                }
                else {
                    for (const chests_data::Spot& spot : chests_data::spots) {
                        if (spot.map == chest.map && spot.submap == chest.submap &&
                            spot.x == chest.x && spot.z == chest.z) {
                            kind = spot.kind == chests_data::Kind::Vanilla ? "a chest spot" : "a spirit spot";
                            break;
                        }
                    }
                }
                char buf[192];
                std::snprintf(buf, sizeof buf, "    chest %d (%s): %s, room %d at (%.0f, %.0f) - %s",
                              chest.id, item_name(chest.item).c_str(),
                              zelda64::map_name(chest.map), chest.submap, chest.x, chest.z, kind);
                log(buf);
            }
            if (options.lost_keys != 0) {
                log("  Note: Shannon's hints name the places chests are in vanilla.");
            }
        }

        void patch_spirits() {
            if (spirit_slots.empty()) {
                return;
            }
            static const char* const modes[] = { "", "within the same map", "anywhere", "anywhere, balanced by story" };
            log("");
            log(std::string("SPIRITS (") + modes[options.spirit_shuffle] + "):");
            std::map<int, int> per_map;
            for (const std::vector<zelda64::randomizer::SpiritPlacement>& slot : spirit_slots) {
                per_map[slot.front().map] += static_cast<int>(slot.size());
            }
            for (const std::pair<const int, int>& entry : per_map) {
                log("  " + std::string(zelda64::map_name(entry.first)) + ": " + std::to_string(entry.second) +
                    (entry.second == 1 ? " spirit" : " spirits"));
            }
            for (const std::vector<zelda64::randomizer::SpiritPlacement>& slot : spirit_slots) {
                for (const zelda64::randomizer::SpiritPlacement& spot : slot) {
                    char buf[128];
                    std::snprintf(buf, sizeof buf, "    %s, room %d at (%.0f, %.0f)",
                                  zelda64::map_name(spot.map), spot.submap, spot.x, spot.z);
                    log(buf);
                }
            }
        }

        // Spell items must keep rules that match what they now cast.
        void set_spell_item_rules() {
            for (int i = 0; i < 6; i++) {
                new_item_spells[i] = shuffles[spell_item_ids[i]];
                std::string rule = spells[(new_item_spells[i] * 4) + 3].substr(6, 2);
                if (rule == "12") {
                    item_spell_fix[i] = 1; // out of battle only
                }
                else if (rule == "03") {
                    item_spell_fix[i] = 2; // either
                }
                else {
                    item_spell_fix[i] = 0; // battle only
                }
            }
        }

        void shuffle_spells() {
            for (int i = 0; i < player_spells; i++) {
                shuffles[i] = -1;
            }
            std::vector<int> reorg = rng.count_and_shuffle(player_spells);

            if (!options.spell_shuffle) {
                for (int i = 0; i < player_spells; i++) {
                    shuffles[i] = i;
                }
                set_spell_item_rules();
                return;
            }

            // Reset the healing and power spell rows of the crashlock table.
            for (int i = 0; i < player_spells; i++) {
                crashlock[(i * player_spells) + 32] = data::noearlyhealing[i];
                crashlock[(i * player_spells) + 19] = data::noearlyhealing[i];
                crashlock[(i * player_spells) + 23] = data::defaultavalanche[i];
                crashlock[(i * player_spells) + 27] = data::defaultmagicbarrier[i];
                crashlock[(i * player_spells) + 34] = data::defaultwaterpillar3[i];
                crashlock[(i * player_spells) + 51] = data::defaultlargecutter[i];
            }

            // Spell combination fixes unlock crashlocked pairs that have a fix.
            if (options.spell_overrides) {
                for (size_t i = 0; i < crashlock.size(); i++) {
                    if (crashlock[i] >= 0 && data::spellfixes[i] >= 0) {
                        crashlock[i] = -1;
                    }
                }
            }

            // Early/extra healing: pick which elements get the healing spells.
            std::vector<int> heal_elements = rng.count_and_shuffle(4);
            if (options.early_healing) {
                for (int i = 0; i < player_spells; i++) {
                    crashlock[(i * player_spells) + 32] = data::earlyhealingmodifier[i];
                    if (options.extra_healing) {
                        crashlock[(i * player_spells) + 19] = data::earlyhealingmodifier[i];
                        crashlock[(i * player_spells) + 11] = data::earlyhealingmodifier[i];
                    }
                }
            }
            for (int i = 0; i < player_spells; i++) {
                int element = i / 15;
                if (heal_elements[0] != element) crashlock[(i * player_spells) + 32] = 32;
                if (heal_elements[1] != element && options.extra_healing) crashlock[(i * player_spells) + 19] = 19;
                if (heal_elements[2] != element && options.extra_healing) crashlock[(i * player_spells) + 11] = 11;
            }

            // Distribute powerful spells across elements.
            std::vector<int> power_elements = rng.count_and_shuffle(4);
            if (options.distribute_spells) {
                for (int i = 0; i < player_spells; i++) {
                    int element = i / 15;
                    if (power_elements[0] != element) crashlock[(i * player_spells) + 23] = 23;
                    if (power_elements[1] != element) crashlock[(i * player_spells) + 27] = 27;
                    if (power_elements[2] != element) crashlock[(i * player_spells) + 34] = 34;
                    if (power_elements[3] != element) crashlock[(i * player_spells) + 51] = 51;
                }
            }

            // Assign modifiers to spells, highest spell first so the powerful
            // ones tend to land late. Retry from scratch if a pass leaves
            // anything unplaced, as Merrow does.
            for (int attempt = 0; attempt < 10000; attempt++) {
                for (int i = player_spells - 1; i >= 0; i--) {
                    for (size_t j = 0; j < reorg.size(); j++) {
                        if (crashlock[(i * player_spells) + reorg[j]] == -1 && shuffles[i] == -1) {
                            shuffles[i] = reorg[j];
                            reorg.erase(reorg.begin() + j);
                            break;
                        }
                    }
                }
                if (reorg.empty()) {
                    break;
                }
                reorg = rng.count_and_shuffle(player_spells);
                for (int i = 0; i < player_spells; i++) {
                    shuffles[i] = -1;
                }
            }
            for (int i = 0; i < player_spells; i++) {
                if (shuffles[i] == -1) {
                    shuffles[i] = i;
                }
            }

            set_spell_item_rules();
        }

        void shuffle_spell_names() {
            for (int i = 0; i < player_spells; i++) {
                int pick = rng.next(4);
                if (options.linear_spell_names) {
                    pick = 0;
                }
                const auto& mine = spell_names[i];
                const auto& theirs = spell_names[shuffles[i]];
                switch (pick) {
                    case 1: hint_names[i] = mine[0] + " " + theirs[3]; break;
                    case 2: hint_names[i] = theirs[0] + " " + mine[2]; break;
                    case 3: hint_names[i] = theirs[0] + " " + mine[3]; break;
                    default: hint_names[i] = mine[0] + " " + theirs[2]; break;
                }
            }
        }

        // Fill a location list either with its vanilla items or with rolls
        // from the whole item pool, then shuffle it.
        // Wings are item ids 14-19. With wingsmith_wings_only on they are cut
        // out of the random pool everywhere except the wingsmith list itself,
        // so a wingsmith stays the only place they turn up. Shuffle mode is
        // unaffected: it only moves items that were already in the list.
        static bool is_wing(int id) { return id >= 14 && id <= 19; }
        // The four gems, the Eletale Book and the Dark Gaol Key.
        static bool is_boss_reward(int id) { return id >= 20 && id <= 25; }

        // What a Random roll may produce. Wings and boss rewards can each be
        // held back so they only turn up where vanilla puts them.
        int roll_item(bool allow_wings) {
            std::vector<int> pool;
            pool.reserve(item_count);
            for (int id = 0; id < item_count; id++) {
                if (is_wing(id) && !allow_wings && options.wingsmith_wings_only) {
                    continue;
                }
                if (is_boss_reward(id) && !options.boss_rewards_shuffler) {
                    continue;
                }
                pool.push_back(id);
            }
            if (pool.empty()) {
                return 0;
            }
            return pool[static_cast<size_t>(rng.next(static_cast<int>(pool.size())))];
        }

        // Fisher-Yates over only the entries not pinned in place.
        void shuffle_unpinned(std::vector<int>& list, const std::vector<bool>& pinned) {
            std::vector<int> movable;
            for (size_t i = 0; i < list.size(); i++) {
                if (!pinned[i]) {
                    movable.push_back(list[i]);
                }
            }
            rng.shuffle(movable);
            size_t next = 0;
            for (size_t i = 0; i < list.size(); i++) {
                if (!pinned[i]) {
                    list[i] = movable[next++];
                }
            }
        }

        void roll_list(std::vector<int>& list, ListMode mode, bool allow_wings = false) {
            if (mode == ListMode::Random) {
                for (int& slot : list) {
                    slot = roll_item(allow_wings);
                }
            }
            rng.shuffle(list);
        }

        void shuffle_items() {
            for (int j = 0; j < chest_count; j++) {
                chests[j] = data::chestdata[j * 4 + 1];
            }
            // Merrow rolls and shuffles the chest and drop lists even when
            // the option is off; the writes are what's gated. Keep the RNG
            // sequence the same by doing likewise.
            roll_list(chests, options.chests);

            // Lost Keys widens the drop list to take in the seven bosses, so
            // their items shuffle with everything else, then splits them back
            // off. The writes still only cover the first drop_count entries.
            int drop_slots = options.lost_keys != 0 ? drop_count + boss_count : drop_count;
            drops.assign(drop_slots, 0);
            for (int l = 0; l < drop_slots; l++) {
                drops[l] = data::dropdata[l * 2 + 1];
            }
            roll_list(drops, options.drops);
            if (options.lost_keys != 0) {
                for (int i = 0; i < boss_count; i++) {
                    lk_boss_items[i] = drops[drop_count + i];
                }
            }

            // Lost Keys needs all ten gift slots addressable, so the final
            // Shannons are only trimmed off when it is not running.
            gifts.assign(options.shuffle_shannon && options.lost_keys == 0 ? 8 : 10, 0);
            for (size_t l = 0; l < gifts.size(); l++) {
                gifts[l] = data::itemgranters[l * 2 + 1];
            }
            if (options.gifts != ListMode::Off) {
                // With the boss rewards held back, the gift slots that hold
                // them in vanilla (8 and 9, the final Shannons, with the
                // Eletale Book and Dark Gaol Key) keep them and sit out the
                // shuffle; only the other slots move.
                std::vector<bool> pinned(gifts.size(), false);
                if (!options.boss_rewards_shuffler) {
                    for (size_t l = 0; l < gifts.size(); l++) {
                        int vanilla = data::itemgranters[l * 2 + 1];
                        if (is_boss_reward(vanilla)) {
                            pinned[l] = true;
                            gifts[l] = vanilla;
                        }
                    }
                }
                if (options.gifts == ListMode::Random) {
                    for (size_t l = 0; l < gifts.size(); l++) {
                        if (!pinned[l]) {
                            gifts[l] = roll_item(false);
                        }
                    }
                    if (!options.shuffle_shannon && options.boss_rewards_shuffler) {
                        // Keep one book and one key in circulation.
                        gifts[8] = 24;
                        gifts[9] = 25;
                    }
                }
                shuffle_unpinned(gifts, pinned);
                if (!options.shuffle_shannon && options.boss_rewards_shuffler) {
                    // The final Shannons must never hold the book or key.
                    int newloc1 = 0;
                    int newloc2 = 0;
                    if (gifts[8] == 24 || gifts[8] == 25) {
                        newloc1 = rng.next(8);
                        std::swap(gifts[8], gifts[newloc1]);
                    }
                    if (gifts[9] == 24 || gifts[9] == 25) {
                        newloc2 = rng.next(8);
                        while (newloc2 == newloc1) {
                            newloc2 = rng.next(8);
                        }
                        std::swap(gifts[9], gifts[newloc2]);
                    }
                }
            }

            for (int l = 0; l < wing_count; l++) {
                wings[l] = data::itemgranters[20 + (l * 2 + 1)];
            }
            if (options.wingsmiths != ListMode::Off) {
                roll_list(wings, options.wingsmiths, true);
            }

            place_lost_keys();
        }

        void shuffle_monsters() {
            monster_stats = data::monsterstatvanilla;
            for (int i = 0; i < boss_count; i++) {
                boss_order[i] = i;
            }
            guilty_element = 4;

            if (options.boss_order) {
                rng.shuffle(boss_order);
            }
            if (options.boss_element) {
                guilty_element = options.guilty_element >= 0 ? options.guilty_element & 3 : rng.next(0, 4);
                monster_stats[437] = guilty_element;
            }

            double scale = difficulty_scale();
            double ext = extremity();

            if (options.monster_stats) {
                for (int i = 16; i > -1; i--) {
                    int locals = data::mon_enemycount[i];
                    int locale = data::mon_locationsindex[i];
                    for (int j = 0; j < locals; j++) {
                        int monster = data::mon_locations[locale + j];
                        for (int m = 0; m < 5; m++) {
                            double current = data::avg_monster[(i * 7) + m];
                            double high = ((data::avg_monster[(i * 7) + 5] + 10) / 2) * (1 + ext);
                            double low = ((data::avg_monster[(i * 7) + 6] + 10) / 2) * (1 - ext);
                            double variance = rng.next_double() * (high - low) + low;
                            int value = round_even(current * (variance / 10) * scale);
                            monster_stats[(monster * 6) + m] = value == 0 ? 1 : value;
                        }
                    }
                }
            }

            if (options.monster_stats || options.boss_order) {
                for (int i = 0; i < 8; i++) {
                    int monster = 67 + i;
                    int region = data::boss_regions[i];
                    for (int m = 0; m < 5; m++) {
                        double current = data::monsterstatvanilla[monster * 6 + m];
                        // ATK/DEF/AGI come from the old boss's stat total split
                        // by the new boss's ratios. Not for Mammon.
                        if (m > 0 && m < 4 && i < boss_count) {
                            current = data::bossbstratios[i * 4] * data::bossbstratios[(boss_order[i] * 4) + m];
                        }
                        int value;
                        if (options.monster_stats) {
                            double high = ((data::avg_monster[(region * 7) + 5] + 20) / 3) * (1 + ext);
                            double low = ((data::avg_monster[(region * 7) + 6] + 20) / 3) * (1 - ext);
                            double variance = rng.next_double() * (high - low) + low;
                            value = round_even(current * (variance / 10) * scale);
                        }
                        else {
                            value = round_even(current);
                        }
                        monster_stats[(monster * 6) + m] = value == 0 ? 1 : value;
                    }
                }
            }

            if (!options.monster_stats && options.monster_scale) {
                for (int i = 0; i < 75; i++) {
                    for (int j = 0; j < 5; j++) {
                        monster_stats[(i * 6) + j] = round_even(monster_stats[(i * 6) + j] * scale);
                    }
                }
            }

            if (options.exp_boost != 4) {
                for (int i = 0; i < 75; i++) {
                    if (options.exp_boost != 0) {
                        int value = round_even(monster_stats[(i * 6) + 4] * scale * (options.exp_boost * 0.25));
                        monster_stats[(i * 6) + 4] = value == 0 ? 1 : std::min(value, 65535);
                    }
                    else {
                        monster_stats[(i * 6) + 4] = 0;
                    }
                }
            }

            if (options.exp_by_bst && (options.monster_scale || options.monster_stats)) {
                for (int i = 17; i > -1; i--) {
                    int locals = data::mon_enemycount[i];
                    int locale = data::mon_locationsindex[i];
                    for (int j = 0; j < locals; j++) {
                        int monster = data::mon_locations[locale + j];
                        int bst = monster_stats[(monster * 6) + 1] + monster_stats[(monster * 6) + 2] + monster_stats[(monster * 6) + 3];
                        double vanilla_bst = data::monsterstatvanilla[(monster * 6) + 1] + data::monsterstatvanilla[(monster * 6) + 2] + data::monsterstatvanilla[(monster * 6) + 3];
                        double exp = data::monsterstatvanilla[(monster * 6) + 4] * (bst / vanilla_bst);
                        monster_stats[(monster * 6) + 4] = static_cast<int>(std::ceil(exp));
                    }
                }
            }
        }

        // QuestPatchBuild.cs -----------------------------------------------

        void write_spell_item_rules() {
            for (int i = 0; i < 6; i++) {
                uint32_t addr = std::stoul(data::items[25 + (i * 3)], nullptr, 16);
                int rule = item_spell_fix[i] == 0 ? 0x0002 : (item_spell_fix[i] == 1 ? 0x0001 : 0x0003);
                add_u16(addr, rule);
            }
        }

        void patch_spells() {
            // Spell Damage Rebalance
            int rebalanced = 0;
            if (options.spell_rebalance) {
                for (int i = 0; i < 18; i++) {
                    // A slot a boss spell took over keeps the boss spell's
                    // own damage: the rebalance figures are for the spell
                    // that used to be there.
                    if (boss_spell_slots[data::damageRebalance[i * 3]] >= 0) {
                        continue;
                    }
                    std::string dmg = hex4(data::damageRebalance[i * 3 + 2]);
                    std::string& spell = spells[data::damageRebalance[i * 3] * 4 + 3];
                    spell = spell.substr(0, 24) + dmg + spell.substr(28);
                }
                rebalanced = 2;
                log("Spell damage rebalanced.");
            }
            if (rebalanced != 0 && !options.spell_shuffle) {
                for (int i = 0; i < 18; i++) {
                    if (boss_spell_slots[data::damageRebalance[i * 3]] >= 0) {
                        continue;
                    }
                    uint32_t addr = std::stoul(spells[data::damageRebalance[i * 3] * 4 + 2]) + 12;
                    add_u16(addr, data::damageRebalance[i * 3 + rebalanced]);
                }
            }

            // Skelebat group in Blue Cave that can crash under lag.
            add_hex("667260", "000000060000000100000001");

            // Boss spells. The record and its animation go in whole; the
            // shuffle below then carries the slot's contents on like any
            // other spell's.
            if (options.boss_spells != 0) {
                log("");
                log("BOSS SPELLS (boss spell > the slot it took over):");
                for (int i = 0; i < player_spells; i++) {
                    if (boss_spell_slots[i] < 0) {
                        continue;
                    }
                    const data::BossSpell& boss = data::bossSpells[boss_spell_slots[i]];
                    add(hex_addr(spells[(i * 4) + 1]), hex_to_bytes(spells[(i * 4) + 3]));
                    add(data::spell_anim_start + i * data::spell_anim_size, hex_to_bytes(boss.anim));
                    log("  " + std::string(boss.name) + " > " + data::spells[i * 4]);
                }
            }

            if (!options.spell_shuffle) {
                // Extra Healing on its own. Merrow only offers it inside the
                // shuffle, where the shuffle map is what carries Healing
                // Lv1's data into Weakness Lv1's slot (its "W1"; the tooltip
                // saying Soul Search Lv1 is wrong), so that one slot is
                // written here the way the shuffle loop below would.
                if (options.extra_healing) {
                    constexpr int weakness_lv1 = 19;
                    constexpr int healing_lv1 = 32;
                    uint32_t base = std::stoul(spells[(weakness_lv1 * 4) + 2]);
                    add(base + 3, hex_to_bytes(spells[(healing_lv1 * 4) + 3].substr(6, 2)));
                    add(base + 11, hex_to_bytes(spells[(healing_lv1 * 4) + 3].substr(22)));
                    add_hex(data::mendingdata[0], data::mendingdata[2]);
                    log("Extra Healing: Weakness Lv1 is Mending Lv1, a second Healing Lv1.");
                }
                // Bubble on its own. With the shuffle on, the loop below is
                // what carries the record into the slot.
                if (options.bubble) {
                    add_hex(data::bubbledata[0], data::bubbledata[2]);
                    add_hex(data::bubblecode[0], data::bubblecode[2]);
                    add_hex(data::bubbleanim[0], data::bubbleanim[2]);
                    log("Soul Search Lv1 replaced with Bubble.");
                }
                // A boss spell only works in a battle, so an item that now
                // casts one has to say so or using it outside a battle locks
                // the game up.
                if (options.boss_spells != 0) {
                    write_spell_item_rules();
                }
                return;
            }

            // Spell Shuffle
            log("");
            log("SPELLS (spell > what it now casts):");
            for (int q = 0; q < player_spells; q++) {
                int tempq = shuffles[q];
                uint32_t base = std::stoul(spells[(q * 4) + 2]);
                add(base + 3, hex_to_bytes(spells[(tempq * 4) + 3].substr(6, 2)));
                add(base + 11, hex_to_bytes(spells[(tempq * 4) + 3].substr(22)));
                log("  " + spells[q * 4] + " > " + spells[tempq * 4]);
            }

            // Extra Healing / Bubble names when hinted names are off.
            if (options.extra_healing && !options.hinted_spell_names) {
                add_hex(data::mendingdata[0], data::mendingdata[2]);
            }
            if (options.bubble) {
                if (!options.hinted_spell_names) {
                    add_hex(data::bubbledata[0], data::bubbledata[2]);
                }
                add_hex(data::bubbleanim[0], data::bubbleanim[2]);
            }

            // Hinted Spell Names
            if (options.hinted_spell_names) {
                add_hex(data::shuffleBossSpellNames[0], data::shuffleBossSpellNames[2]);
                add_hex(data::shuffleBossSpellNames[3], data::shuffleBossSpellNames[5]);
                for (int i = 0; i < player_spells; i++) {
                    add_hex(spell_names[i][5], spell_names[i][6]);
                }
                for (int i = 0; i < player_spells; i++) {
                    // Spell names are plain ASCII, padded to 16 bytes.
                    std::vector<uint8_t> name(hint_names[i].begin(), hint_names[i].end());
                    name.resize(16, 0);
                    add(hex_addr(spell_names[i][4]), name);
                }
            }

            // Spell item softlock protection
            write_spell_item_rules();

            // Updated spell item names
            for (int i = 0; i < 6; i++) {
                int element = 0;
                if (new_item_spells[i] > 15) element = 1;
                if (new_item_spells[i] > 30) element = 2;
                if (new_item_spells[i] > 45) element = 3;
                auto encoded = translate_string(data::newSpellItemName[i][element]);
                if (encoded) {
                    add(std::stoul(data::spellItemNameAddr[i], nullptr, 16), *encoded);
                }
                item_capital_case[i + 8] = data::newSpellItemCapCase[i][element];
            }

            // Replacement spell item descriptions
            new_spell_item_desc[19] = data::specialSpellItemDesc[options.extra_healing ? 1 : 0];
            new_spell_item_desc[33] = data::specialSpellItemDesc[options.bubble ? 3 : 2];
            for (int i = 0; i < 6; i++) {
                auto encoded = translate_string(new_spell_item_desc[shuffles[data::spellItemIDs[i]]]);
                if (encoded) {
                    add(std::stoul(data::spellItemDescAddr[i], nullptr, 16), *encoded);
                }
            }

            // Spell Combination Fixes
            if (options.spell_overrides) {
                for (int i = 0; i < player_spells; i++) {
                    int fix = data::spellfixes[(i * player_spells) + shuffles[i]];
                    if (fix >= 0) {
                        add_hex(data::spellfixdata[fix * 2], data::spellfixdata[(fix * 2) + 1]);
                    }
                }
            }
        }

        void patch_items() {
            if (options.chests != ListMode::Off) {
                log("");
                log("CHESTS:");
                for (int i = 0; i < chest_count; i++) {
                    add_u8(data::chestdata[i * 4] + 33, chests[i]);
                    log("  " + std::to_string(i) + ": " + item_name(chests[i]));
                }
            }
            if (options.drops != ListMode::Off) {
                log("");
                log("MONSTER DROPS:");
                for (int i = 0; i < drop_count; i++) {
                    add_u8(data::dropdata[i * 2], drops[i]);
                    log("  " + data::monsternames[i * 2] + ": " + item_name(drops[i]));
                }
            }
            if (options.gifts != ListMode::Off) {
                log("");
                log("GIFTS:");
                for (size_t i = 0; i < gifts.size(); i++) {
                    add_u8(data::itemgranters[i * 2], gifts[i]);
                    log("  " + data::gifternames[i] + ": " + item_name(gifts[i]));
                }
            }
            if (options.wingsmiths != ListMode::Off) {
                log("");
                log("WINGSMITHS:");
                for (int i = 0; i < wing_count; i++) {
                    add_u8(data::itemgranters[20 + (i * 2)], wings[i]);
                    log("  " + data::gifternames[10 + i] + ": " + item_name(wings[i]));
                }
            }

            if (options.ivory_wings) {
                // The gifter text below has to name the new item.
                item_capital_case[14] = "Ivory Wings";
            }

            // Updated Gifter/Wingsmith text
            if (options.gifts != ListMode::Off || options.wingsmiths != ListMode::Off) {
                // Which gift slot each of the 28 text blocks refers to.
                constexpr int gifternum[28] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 10, 10 };
                for (int i = 0; i < 28; i++) {
                    std::string text;
                    std::string addr;
                    if (i <= 15) {
                        int gift_index = gifternum[i];
                        int item = gift_index < static_cast<int>(gifts.size()) ? gifts[gift_index] : data::itemgranters[gift_index * 2 + 1];
                        text = data::newgiftertext[(i * 3) + 1] + item_capital_case[item] + data::newgiftertext[(i * 3) + 2];
                        addr = data::newgiftertext[i * 3];
                    }
                    else if (i <= 25) {
                        text = data::newgiftertext[(i * 3) + 1] + item_capital_case[wings[gifternum[i] - 10]] + data::newgiftertext[(i * 3) + 2];
                        addr = data::newgiftertext[i * 3];
                    }
                    else {
                        text = data::newgiftertext[i == 26 ? 79 : 81];
                        addr = data::newgiftertext[i == 26 ? 78 : 80];
                    }
                    auto encoded = translate_string(text);
                    if (!encoded) {
                        continue;
                    }
                    std::vector<uint8_t> bytes = { 0xA0, 0xC0 };
                    bytes.insert(bytes.end(), encoded->begin(), encoded->end());
                    add(std::stoul(addr, nullptr, 16), bytes);
                }
            }
        }

        void patch_monsters() {
            bool write_stats = options.monster_stats || options.monster_scale || options.boss_order || options.exp_boost != 4;
            if (write_stats) {
                log("");
                log("MONSTER STATS (HP/ATK/DEF/AGI/EXP):");
                for (int i = 0; i < 75; i++) {
                    int row = i;
                    if (i >= 67 && i < 74) {
                        row = 67 + boss_order[i - 67];
                    }
                    for (int j = 0; j < 6; j++) {
                        uint32_t addr = std::stoul(data::monsterstatlocations[row][j], nullptr, 16);
                        add_u16(addr, monster_stats[i * 6 + j]);
                        if (j == 0) {
                            // HP is stored twice.
                            add_u16(addr + 2, monster_stats[i * 6 + j]);
                        }
                    }
                    std::string line = "  " + data::monsternames[std::min<size_t>(i, data::monsternames.size() / 2 - 1) * 2] + ":";
                    for (int j = 0; j < 5; j++) {
                        line += " " + std::to_string(monster_stats[i * 6 + j]);
                    }
                    log(line);
                }
            }

            // Boss Order Shuffle
            if (options.boss_order) {
                constexpr int vanilla_boss_items[boss_count] = { 20, 21, 22, 255, 23, 255, 255 };
                constexpr uint32_t boss_addresses[boss_count] = { 14186532, 14186588, 14186644, 14186700, 14186756, 14186812, 14186868 };
                static const char* const boss_names[boss_count] = { "Solvaring", "Zelse", "Nepty", "Shilf", "Fargo", "Guilty", "Beigis" };
                log("");
                log("BOSS ORDER (arena: boss):");
                for (int i = 0; i < boss_count; i++) {
                    add_hex(data::bosslocdata[boss_order[i] * 4], data::bosslocdata[(i * 4) + 1]);
                    add_hex(data::bosslocdata[(boss_order[i] * 4) + 2], data::bosslocdata[(i * 4) + 3]);
                    add_hex(data::rewardhpdata[boss_order[i] * 2], data::rewardhpdata[(i * 2) + 1]);
                    // Lost Keys replaces what every boss carries.
                    int carried = options.lost_keys != 0 ? lk_boss_items[i] : vanilla_boss_items[i];
                    add_u8(boss_addresses[boss_order[i]], carried);
                    log(std::string("  ") + boss_names[boss_order[i]] + "'s arena: " + boss_names[i]);
                }
                // Merrow also patches code at 0x01D4D7 to skip Beigis's map
                // check when he is moved. That is an instruction immediate, so
                // it goes through quest64_randomizer_beigis_map_check instead
                // of a ROM write; Beigis is boss 6.
                if (boss_order[6] != 6) {
                    beigis_moved = true;
                    log("  Beigis's map check disabled (he is not in his own arena).");
                }
            }

            if (options.boss_element) {
                std::string e = std::to_string(guilty_element);
                add_hex("D8792C", "000" + e + "000" + e);
                static const char* const elements[4] = { "Fire", "Earth", "Water", "Wind" };
                log("");
                log(std::string("Guilty's element: ") + elements[guilty_element & 3]);
            }
        }

        void patch_misc() {
            if (options.invalidity) {
                for (int i = 0; i < 8; i++) {
                    add_hex(data::invalidityLocations[i], "00");
                }
                log("Invalidity: on.");
            }

            if (options.max_accuracy) {
                if (!options.max_accuracy_all) {
                    for (int i = 0; i < 17; i++) {
                        uint32_t addr = std::stoul(spells[(data::statusspells[i] * 4) + 2]) + 15;
                        add_u8(addr, 0x64);
                    }
                    log("Status spells always hit.");
                }
                else {
                    for (int z = spell_start + 15; z < (spell_offset * player_spells) + spell_start; z += spell_offset) {
                        add_u8(z, 0x64);
                    }
                    log("All spells always hit.");
                }
            }

            if (options.level_1_spells) {
                for (int s = spell_start; s < (spell_offset * player_spells) + spell_start; s += spell_offset) {
                    add_u16(s, 1);
                }
                log("All spells unlocked at level 1.");
            }

            if (options.soul_search) {
                for (int z = spell_start + 57; z < (spell_offset * player_spells) + spell_start; z += spell_offset) {
                    add_u8(z, 1);
                }
                uint32_t addr = 0xD81C30;
                for (int i = 0; i < 16; i++) {
                    add(addr, hex_to_bytes(data::magnifier[i]));
                    addr += 128;
                }
                log("Soul Search: on.");
            }

            // Walking MP regen rate. Merrow's trackbar: 7 is off, 8-12 are the
            // speed tiers with 10 matching vanilla. Only the tiers are a data
            // byte; "off" is an instruction immediate and is handled by
            // quest64_randomizer_walk_mp_regain.
            if (options.mp_regain >= 8 && options.mp_regain <= 12 && options.mp_regain != 10) {
                // Merrow's values, chosen to track seconds spent running
                // rather than multiplying the rate.
                static constexpr int speeds[5] = { 0x58, 0x51, 0x41, 0x31, 0x28 };
                add_u8(0x071B39, speeds[options.mp_regain - 8]);
                log("Walking MP regen rate: tier " + std::to_string(options.mp_regain - 10) + ".");
            }
            if (options.mp_regain == 7) {
                log("Walking MP regen: off.");
            }
            if (options.staff_hit_mp != 1) {
                log("Staff hit MP regain: " + std::to_string(options.staff_hit_mp) + ".");
            }
            if (options.element_uncap) {
                log("Element level maximum raised to 99.");
            }
            if (options.drop_limit_disabled) {
                log("Enemy drop limit disabled.");
            }
            if (options.wing_unlock_indoors) {
                log("Wings enabled indoors.");
            }
            if (options.wing_unlock_skye) {
                log("Wings enabled on the Isle of Skye.");
            }
            if (options.encounter_rate != 2) {
                static const char* const names[5] = { "Halved", "Reduced", "Default", "Increased", "Doubled" };
                log(std::string("Encounter rate: ") + names[std::clamp(options.encounter_rate, 0, 4)] + ".");
            }

            if (options.fast_monastery) {
                add_hex("4361A0", "00090002");
                log("Fast Monastery: on.");
            }
            if (options.fast_blue_cave) {
                add_hex("65F1E3", "180018001A00000005");
                add_hex("65F273", "180018001A00000007");
                log("Fast Blue Cave: on.");
            }
            if (options.unlock_doors) {
                // The 18 progression locks (gems, book, key). Each pair is an
                // address and its replacement record; the record's own length
                // decides how many bytes are written.
                for (int i = 0; i < 18; i++) {
                    add_hex(data::unlockedDoorData[i * 2], data::unlockedDoorData[i * 2 + 1]);
                }
                log("All progression locks (gems/book/key) unlocked.");
            }
            if (options.locked_endgame) {
                // Entries 21-24 put the Elemental Gems back on the final
                // staircase instead.
                for (int i = 21; i < 25; i++) {
                    add_hex(data::unlockedDoorData[i * 2], data::unlockedDoorData[i * 2 + 1]);
                }
                log("Approach to Mammon's World locked by Elemental Gems.");
            }
            if (options.fast_shamwood) {
                add_hex(data::unlockedDoorData[19 * 2], data::unlockedDoorData[19 * 2 + 1]);
                log("Fast Shamwood: on.");
            }
            if (options.brannoch_return) {
                add_hex(data::unlockedDoorData[20 * 2], data::unlockedDoorData[20 * 2 + 1]);
                log("Brannoch return warp enabled.");
            }
            if (options.crystal_return) {
                // The Water Jewel requirement is dropped from the record when
                // the progression locks are already open.
                add_hex("206EB0", options.unlock_doors
                    ? "42020000C3BC0000BFC90FF940C0000040E0000001060000000000020000001A00020003"
                    : "42020000C3BC0000BFC90FF940C0000040E0000001160016000000020000001A00020003");
                log("Crystal Valley return warp enabled.");
            }
            if (options.fast_mammon) {
                add_hex("84EDFE", "000E000D");
                add_hex("607920", "0000000F000D0000");
                log("Fast Mammon's World: on.");
            }
            if (options.mammon_door) {
                add_hex(data::mammonbackwarddoordata[0], data::mammonbackwarddoordata[2]);
                log("Locked the useless backward door in Mammon's World.");
            }
            if (options.restless_npcs) {
                // Every NPC's movement byte becomes 02, which makes them wander.
                for (int address : data::npcmovement) {
                    add(static_cast<uint32_t>(address), { 0x02 });
                }
                log("NPCs are restless.");
            }
            if (options.max_message_speed) {
                add_hex("060600", "00");
                log("Message speed: max.");
            }
            if (options.hud_lock) {
                add_hex("01F0AF", "00");
                log("HUD onscreen lock enabled.");
            }
            if (options.celtland_drift) {
                add_hex("071B50", "3FF44444");
                log("Celtland Drift enabled.");
            }
            if (options.level_2_spells) {
                // The four base spells unlock at level 2 instead of 1.
                for (int i = 0; i < 60; i += 15) {
                    add_hex(data::spells[(i * 4) + 1], "0002");
                }
                log("Base spells unlocked at level 2.");
            }
            if (options.reveal_spirits) {
                add_hex("609970", "A20800003208");
                add_hex("56FAE0", "C210000041E8");
                log("Hidden spirits moved to clearer locations.");
            }
            if (options.better_dew_drop) {
                // Makes the Dew Drop a full revive, and rewrites its and the
                // Dragon Potion's descriptions to match.
                add_hex("D86B01", "14");
                add_hex(data::newdewdropdesc[0], data::newdewdropdesc[1]);
                add_hex(data::newdewdropdesc[2], data::newdewdropdesc[3]);
                add_hex(data::newdewdropdesc[4], data::newdewdropdesc[5]);
                log("Dew Drop made useful.");
            }
            if (options.zoom_out != 0) {
                // 0x3FF0 is the stock camera distance and 0x3FE4 the lowest
                // stable one, so each step backs the camera off by one unit.
                int zoom = std::clamp(options.zoom_out, 1, 4) + 1;
                add_u16(hex_addr("03698A"), 16368 - zoom);
                add_u16(hex_addr("036A26"), 16368 - zoom);
                log("Zoom out factor set to " + std::to_string(zoom - 1) + ".");
            }
            if (options.ivory_wings) {
                // White Wings become the Ivory Wings, granted by Lavaar.
                for (int i = 0; i < 4; i++) {
                    add_hex(data::ivorywings[i * 3], data::ivorywings[i * 3 + 2]);
                }
                // Melrode wingsmith actor type, then Lavaar's NPC type and
                // second dialogue.
                add_hex("49F90F", "01");
                add_hex("5EBB13", "02");
                add_hex("5EBB1A", "1CA0");
                if (options.wingsmiths == ListMode::Off) {
                    // Nothing else is rewriting what Lavaar hands over.
                    add_hex("5EBB16", "000E");
                }
                log("White Wings replaced with Ivory Wings (granted by Lavaar).");
            }
            if (options.text_improvements) {
                // Merrow also rewrites two Lost Keys texts here; those wait
                // until Lost Keys itself is ported.
                if (auto encoded = translate_string(data::newsavetext)) {
                    std::vector<uint8_t> bytes = { 0xA0, 0xC0 };
                    bytes.insert(bytes.end(), encoded->begin(), encoded->end());
                    add(hex_addr("06B268"), bytes);
                    log("Text improvements added.");
                }
                if (options.lost_keys != 0) {
                    // Abbott and the Melrode Shannon explain the Lost Keys
                    // premise instead of the vanilla opening.
                    const std::pair<const char*, const std::string*> lk_texts[2] = {
                        { "055428", &data::newAbbottIntroLK },
                        { "D305E0", &data::newMelrodeShannonLK },
                    };
                    for (const auto& entry : lk_texts) {
                        if (auto lk = translate_string(*entry.second)) {
                            std::vector<uint8_t> lk_bytes = { 0xA0, 0xC0 };
                            lk_bytes.insert(lk_bytes.end(), lk->begin(), lk->end());
                            add(hex_addr(entry.first), lk_bytes);
                        }
                    }
                    log("Lost Keys intro text added.");
                }
            }

            if (options.start_hp != 50 || options.start_mp != 15 || options.start_agility != 5 || options.start_defense != 4) {
                std::string stats = hex4(options.start_hp) + hex4(options.start_hp) + hex4(options.start_mp) + hex4(options.start_mp) + hex4(options.start_agility) + hex4(options.start_defense);
                add_hex("054908", stats);
                log("Starting stats: HP " + std::to_string(options.start_hp) + ", MP " + std::to_string(options.start_mp) + ", AGI " + std::to_string(options.start_agility) + ", DEF " + std::to_string(options.start_defense));
            }
        }

        // QuestPatchBuild.cs's cosmetic section. Merrow keeps these last so
        // they can't affect anything else, and rolls them off the system RNG;
        // here they come off the seeded RNG so a seed reproduces its colours.
        void patch_cosmetics() {
            if (options.text_palette != 0) {
                // Merrow's fixed palettes, in its dropdown's order.
                static const char* const fixed_palettes[7] = {
                    nullptr,                // 0: off
                    nullptr,                // 1: random, handled below
                    "F83E9C1BBA0DD009",     // 2: red
                    "F83E9C1B629D19AB",     // 3: blue
                    "F83E318DBDEFF735",     // 4: white
                    "F83E9C1B6AD5318D",     // 5: black (the game's default)
                    nullptr,
                };
                static const char* const palette_names[6] = {
                    "off", "random", "red", "blue", "white", "black"
                };
                int choice = std::clamp(options.text_palette, 0, 5);
                if (choice == 1) {
                    // A random hue rotation of either the light or the dark
                    // base palette, as Merrow's "random" setting does.
                    ColourRoll roll = roll_colour();
                    bool light = rng.next(2) != 0;
                    const std::vector<std::string>& base =
                        light ? data::baseRedTextPalette : data::baseDarkTextPalette;
                    std::string palette = "F83E";
                    for (int i = 0; i < 3; i++) {
                        palette += rgba_to_rgba5551(apply_roll(rgba5551_to_rgba(base[i]), roll));
                    }
                    add_hex("D3E240", palette);
                    log("Text palette: random (" + roll.describe() + (light ? ", light)." : ", dark)."));
                }
                else if (fixed_palettes[choice] != nullptr) {
                    add_hex("D3E240", fixed_palettes[choice]);
                    log(std::string("Text palette: ") + palette_names[choice] + ".");
                }
            }

            if (options.staff_palette) {
                // The staff texture is 768 RGBA5551 colours, all moved by the
                // same roll.
                ColourRoll roll = roll_colour();
                std::string palette;
                palette.reserve(768 * 4);
                for (int i = 0; i < 768; i++) {
                    palette += rgba_to_rgba5551(apply_roll(rgba5551_to_rgba(data::stafftexture.substr(i * 4, 4)), roll));
                }
                add_hex("86EB70", palette);
                log("Staff palette: random (" + roll.describe() + ").");
            }

            if (options.cloak_palette) {
                // One flat RGBA8888 colour, written over each of the six
                // cloak palette entries.
                char rgb[8];
                snprintf(rgb, sizeof(rgb), "%06X", rng.next(0x1000000));
                for (const std::string& location : data::cloaklocations) {
                    add_hex(location, std::string(rgb) + "FF");
                }
                log(std::string("Cloak colour: #") + rgb + ".");
            }

            if (options.brian_palette) {
                // Brian's clothes are two texture pages of 0x980 bytes each,
                // rotated by a hue of their own. briantexture2 holds more
                // colours than the write covers, so only the first 1216 of
                // each are used, as Merrow does.
                constexpr int brian_colours = 1216;
                static const struct { const char* address; const std::string& texture; } pages[2] = {
                    { "86D5F0", data::briantexture1 },
                    { "86DFF0", data::briantexture2 },
                };
                std::string hues;
                for (const auto& page : pages) {
                    ColourRoll roll = roll_colour();
                    std::string colours;
                    colours.reserve(brian_colours * 4);
                    for (int i = 0; i < brian_colours; i++) {
                        colours += rgba_to_rgba5551(apply_roll(rgba5551_to_rgba(page.texture.substr(i * 4, 4)), roll));
                    }
                    add_hex(page.address, colours);
                    if (!hues.empty()) {
                        hues += "/";
                    }
                    hues += roll.describe();
                }
                log("Brian's clothes: random (" + hues + ").");
            }

            if (options.spell_palette) {
                // Each entry is an address and the vanilla halfword; only the
                // low byte, the palette index, is rerolled (1-17).
                for (size_t i = 0; i * 2 + 1 < data::allcolors.size(); i++) {
                    char index[4];
                    snprintf(index, sizeof(index), "%02X", rng.next(17) + 1);
                    add_hex(data::allcolors[i * 2], data::allcolors[i * 2 + 1].substr(0, 2) + index);
                }
                log("Spell palettes randomized.");
            }

        }


        // QuestPatchBuild.cs's Lost Keys section: the door changes that keep a
        // Lost Keys run completable, the boss items when boss order is not
        // also shuffling them, and Shannon's hints about where the gems went.
        void patch_lost_keys() {
            if (options.lost_keys == 0) {
                return;
            }

            log("");
            log(std::string("LOST KEYS: ") + (options.lost_keys == 1 ? "progressive." : "open world."));
            log("  Chest, drop and gift shuffling forced on, wingsmith shuffling off, Ivory Wings on.");

            if (!options.unlock_doors) {
                // Only reachable in Progressive, since Open World is meant to
                // be played with the progression locks already open. Colleen's
                // side is opened up and the castle gate is locked behind the
                // Fire Ruby instead.
                for (int entry : { 7, 8, 9, 10, 18 }) {
                    add_hex(data::unlockedDoorData[entry * 2], data::unlockedDoorData[entry * 2 + 1]);
                }
                // The fire symbol that now appears on the Brannoch Castle gate.
                add_hex("0E72F0", data::firegate);
            }

            if (!options.boss_order) {
                // With boss order shuffling on, patch_monsters writes these as
                // part of its own loop; this covers the case where it is off.
                // Merrow indexes a six-entry address table here and would run
                // off the end at Beigis, so the addresses come from dropdata,
                // which is where Merrow reads them for the boss-order path.
                log("");
                log("BOSS ITEMS (Lost Keys):");
                for (int i = 0; i < boss_count; i++) {
                    add_u8(data::dropdata[(i + drop_count) * 2], lk_boss_items[i]);
                    log("  " + data::monsternames[(i + drop_count) * 2] + " carries " + item_name(lk_boss_items[i]));
                }
            }

            if (options.shannon_hints) {
                // Each of the five Shannons describes where one gem ended up.
                // Two coin flips per gem pick the adjective and the sentence.
                static const char* const region_names[5] = { "earth", "wind", "water", "fire", "book" };
                log("");
                log("SHANNON HINTS:");
                for (int i = 0; i < 5; i++) {
                    const std::vector<std::string>& words =
                        i == 0 ? data::earthhints :
                        i == 1 ? data::windhints :
                        i == 2 ? data::waterhints :
                        i == 3 ? data::firehints :
                        (options.fire_book ? data::bookfhints : data::bookhints);

                    int location = hints[i] < 0 ? 0 : hints[i];
                    size_t word_index = static_cast<size_t>(location) * 2 + static_cast<size_t>(hint_coins[i]);
                    if (word_index >= words.size()) {
                        // Merrow's book hint tables stop one entry short of the
                        // range its own roll can produce, so the last location
                        // has no wording. Fall back rather than read past the
                        // end; the hint is vaguer but never wrong.
                        word_index = words.size() - (hint_coins[i] == 0 ? 2 : 1);
                    }
                    const std::string& word = words[word_index];
                    const std::string& gem = data::gemnames[static_cast<size_t>(gem_ids[i] - 20)];

                    std::string line;
                    if (hint_coins[i + 5] == 0) {
                        line = "The " + gem + " currently#rests somewhere " + word +
                               ".#You must find it before any#darker purposes befall it.%";
                    }
                    else {
                        line = "The " + gem + " resides#in a " + word +
                               " place.#We must retrieve it for the#good of all Celtland.%";
                    }

                    auto encoded = translate_string(line);
                    if (!encoded) {
                        continue;
                    }
                    // Unlike the gifter text these are written without the
                    // A0C0 header, as Merrow does.
                    add(hex_addr(data::shannonhints[i]), *encoded);
                    // Drop the Shannons' follow-up dialogue so the hint does
                    // not depend on which order the items were found in.
                    add_hex(data::shannonrules[i], "01");
                    log(std::string("  ") + region_names[i] + ": " + gem + " is " + word);
                }
            }
        }


        // Util/AreaEncounterData.cs. Two independent shuffles:
        //
        //  - Tables: the areas swap which of the six global enemy tables they
        //    draw from, so Holy Plains can end up fielding Brannoch's roster.
        //    Pack enemy ids are then wrapped into the new table's size.
        //  - Compositions: each encounter region rerolls which packs it can
        //    spawn, and each pack rerolls which enemies it holds.
        //
        // Baragoon Moor, Brannoch Castle and Mammon's World share pack
        // definitions across their submaps and so write to the same addresses.
        // After both passes their enemy ids are wrapped to the smallest table
        // any member of the group ended up with, or a submap could reference an
        // enemy its own table does not have.
        void shuffle_enemies() {
            if (!options.enemy_tables && !options.enemy_randomizer) {
                return;
            }
            enemy_areas = mapdata::areas;
            enemy_packs = mapdata::packs;
            enemy_regions = mapdata::regions;

            auto table_size = [&](const mapdata::Area& area) {
                size_t index = area.map.table_index;
                return index < mapdata::monster_tables.size()
                    ? static_cast<int>(mapdata::monster_tables[index].enemies.size())
                    : 0;
            };
            auto cap_ids = [&](const mapdata::Area& area, int limit) {
                if (limit <= 0) {
                    return;
                }
                for (int p = 0; p < area.pack_count; p++) {
                    for (mapdata::PackMember& member : enemy_packs[area.pack_start + p].members) {
                        member.enemy_id %= limit;
                    }
                }
            };

            if (options.enemy_randomizer) {
                // Tier-aware: each merged area draws one of the files whose
                // monsters fall inside its spread (DOCS/enemyrandologic.xlsx).
                // Runs before the random table shuffle and replaces it.
                progression_tables.assign(merrow::progression::areas.size(), 0);
                int previous = -1;
                for (size_t a = 0; a < merrow::progression::areas.size(); a++) {
                    std::vector<int> candidates = progression::candidate_tables(static_cast<int>(a), options);
                    // Prefer not to repeat the previous area's file when there
                    // is a choice, so consecutive areas look different.
                    if (candidates.size() > 1) {
                        candidates.erase(std::remove(candidates.begin(), candidates.end(), previous), candidates.end());
                    }
                    int table = candidates[static_cast<size_t>(rng.next(static_cast<int>(candidates.size())))];
                    progression_tables[a] = table;
                    previous = table;
                    const merrow::progression::AreaInfo& info = merrow::progression::areas[a];
                    for (int raw = info.first_raw; raw <= info.last_raw; raw++) {
                        enemy_areas[static_cast<size_t>(raw)].map.table_index = static_cast<uint16_t>(table);
                        cap_ids(enemy_areas[static_cast<size_t>(raw)], table_size(enemy_areas[static_cast<size_t>(raw)]));
                    }
                }
            }
            else if (options.enemy_tables) {
                std::vector<int> table_indices;
                table_indices.reserve(enemy_areas.size());
                for (const mapdata::Area& area : enemy_areas) {
                    table_indices.push_back(area.map.table_index);
                }
                rng.shuffle(table_indices);
                for (size_t k = 0; k < enemy_areas.size(); k++) {
                    enemy_areas[k].map.table_index = static_cast<uint16_t>(table_indices[k]);
                    cap_ids(enemy_areas[k], table_size(enemy_areas[k]));
                }
            }

            if (options.enemy_randomizer) {
                for (const mapdata::Area& area : enemy_areas) {
                    // Every region gets a full seven presets, wrapping if the
                    // area has fewer packs than that.
                    for (int r = 0; r < area.region_count; r++) {
                        mapdata::Region& region = enemy_regions[area.region_start + r];
                        std::vector<int> pool = rng.count_and_shuffle(area.pack_count);
                        region.presets.assign(7, 0);
                        if (!pool.empty()) {
                            for (int k = 0; k < 7; k++) {
                                region.presets[static_cast<size_t>(k)] =
                                    pool[static_cast<size_t>(k) % pool.size()];
                            }
                        }
                        region.preset_count = 7;
                    }

                    int available = table_size(area);
                    for (int p = 0; p < area.pack_count; p++) {
                        std::vector<int> ids = rng.count_and_shuffle(available);
                        if (ids.empty()) {
                            continue;
                        }
                        std::vector<mapdata::PackMember>& members = enemy_packs[area.pack_start + p].members;
                        for (size_t m = 0; m < members.size(); m++) {
                            members[m].enemy_id = ids[m % ids.size()];
                        }
                    }
                }
            }

            auto clamp_group = [&](int start, int last, const char* what) {
                int smallest = 20;
                for (int k = start; k <= last; k++) {
                    smallest = std::min(smallest, table_size(enemy_areas[static_cast<size_t>(k)]));
                }
                for (int k = start; k <= last; k++) {
                    cap_ids(enemy_areas[static_cast<size_t>(k)], smallest);
                }
                enemy_group_notes += std::string("  ") + what + " capped to " +
                    std::to_string(smallest) + " enemies (shared packs).\n";
            };
            clamp_group(13, 14, "Baragoon Moor");
            clamp_group(15, 20, "Brannoch Castle");
            clamp_group(21, 26, "Mammon's World");
        }

        // The three write shapes from AreaEncounterData.cs: each pack as
        // packCount groups of three big-endian words, the area's 0x18-byte map
        // header with its (possibly new) table index folded back in, and each
        // region's preset list, which starts 8 bytes past the region address.
        void patch_enemies() {
            if (!options.enemy_tables && !options.enemy_randomizer) {
                return;
            }

            auto push_u32 = [](std::vector<uint8_t>& out, uint32_t value) {
                out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
                out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
                out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
                out.push_back(static_cast<uint8_t>(value & 0xFF));
            };
            auto push_u16 = [](std::vector<uint8_t>& out, uint32_t value) {
                out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
                out.push_back(static_cast<uint8_t>(value & 0xFF));
            };

            log("");
            log("ENEMIES:");
            if (options.enemy_randomizer) {
                std::string text;
                progression::make_plan(progression_tables, options, text);
                spoiler += text;
            }
            else if (options.enemy_tables) {
                log("  Area enemy tables shuffled.");
            }
            if (options.enemy_randomizer) {
                log("  Encounter compositions shuffled.");
            }

            for (const mapdata::Area& area : enemy_areas) {
                for (int p = 0; p < area.pack_count; p++) {
                    const mapdata::MonsterPack& pack = enemy_packs[area.pack_start + p];
                    std::vector<uint8_t> bytes;
                    bytes.reserve(pack.members.size() * 12);
                    for (const mapdata::PackMember& member : pack.members) {
                        push_u32(bytes, static_cast<uint32_t>(member.enemy_id));
                        push_u32(bytes, static_cast<uint32_t>(member.min_count));
                        push_u32(bytes, static_cast<uint32_t>(member.extra_count));
                    }
                    if (!bytes.empty()) {
                        add(pack.rom_address, std::move(bytes));
                    }
                }

                std::vector<uint8_t> header;
                header.reserve(24);
                push_u32(header, area.map.unk0);
                push_u32(header, area.map.ptr_door_data);
                push_u32(header, area.map.door_count);
                push_u32(header, area.map.unk8);
                push_u32(header, (static_cast<uint32_t>(area.map.unk10) << 16) | area.map.table_index);
                push_u32(header, area.map.unk14);
                add(area.map.rom_address, std::move(header));

                // Only written when the compositions were actually rerolled.
                // Two of Merrow's 72 regions carry preset lists that disagree
                // with the ROM (they look swapped with each other), so writing
                // unchanged region data back would quietly alter them.
                for (int r = 0; options.enemy_randomizer && r < area.region_count; r++) {
                    const mapdata::Region& region = enemy_regions[area.region_start + r];
                    auto preset = [&](size_t i) {
                        return i < region.presets.size() ? static_cast<uint32_t>(region.presets[i]) : 0u;
                    };
                    std::vector<uint8_t> bytes;
                    bytes.reserve(16);
                    push_u16(bytes, static_cast<uint32_t>(region.preset_count));
                    for (size_t i = 0; i < 7; i++) {
                        push_u16(bytes, preset(i));
                    }
                    add(region.rom_address + 8, std::move(bytes));
                }

                size_t table = area.map.table_index;
                log("  " + area.name + ": table " + std::to_string(table) +
                    (table < mapdata::monster_tables.size() && !mapdata::monster_tables[table].enemies.empty()
                        ? " (" + mapdata::monster_tables[table].enemies[0] + ", ...)"
                        : ""));
            }
            if (!enemy_group_notes.empty()) {
                log(enemy_group_notes.substr(0, enemy_group_notes.size() - 1));
            }
        }

        void build() {
            place_bubble();
            place_boss_spells();
            shuffle_spells();
            shuffle_spell_names();
            shuffle_items();
            shuffle_monsters();
            shuffle_enemies();
            shuffle_spirits();
            shuffle_chests();

            patch_spells();
            patch_items();
            patch_monsters();
            patch_misc();
            patch_lost_keys();
            patch_enemies();
            patch_spirits();
            patch_chests();
            patch_cosmetics();
        }
    };

    Options active;
    bool active_loaded = false;
    zelda64::randomizer::NativeState native;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "randomizer.json";
    }

    std::filesystem::path spoiler_path() {
        return zelda64::get_app_folder_path() / "randomizer_spoiler.txt";
    }

    std::filesystem::path presets_path() {
        return zelda64::get_app_folder_path() / "randomizer_presets.json";
    }

    nlohmann::json load_presets_file() {
        std::ifstream in(presets_path());
        if (!in.good()) {
            return nlohmann::json::object();
        }
        nlohmann::json j;
        try {
            in >> j;
        }
        catch (nlohmann::json::parse_error&) {
            return nlohmann::json::object();
        }
        return j.is_object() ? j : nlohmann::json::object();
    }

    void write_presets_file(const nlohmann::json& j) {
        std::ofstream out(presets_path());
        out << j.dump(4);
    }

    const char* list_mode_name(ListMode mode) {
        switch (mode) {
            case ListMode::Shuffle: return "shuffle";
            case ListMode::Random: return "random";
            default: return "off";
        }
    }

    ListMode list_mode_from_name(const std::string& name) {
        if (name == "shuffle") return ListMode::Shuffle;
        if (name == "random") return ListMode::Random;
        return ListMode::Off;
    }
}

static nlohmann::json options_to_json(const Options& o) {
    nlohmann::json j;
    j["mode"] = o.mode == Mode::Randomizer ? "randomizer" : "vanilla";
    j["seed"] = o.seed;
    j["spell_shuffle"] = o.spell_shuffle;
    j["hinted_spell_names"] = o.hinted_spell_names;
    j["linear_spell_names"] = o.linear_spell_names;
    j["early_healing"] = o.early_healing;
    j["extra_healing"] = o.extra_healing;
    j["distribute_spells"] = o.distribute_spells;
    j["spell_overrides"] = o.spell_overrides;
    j["spell_rebalance"] = o.spell_rebalance;
    j["bubble"] = o.bubble;
    j["level_1_spells"] = o.level_1_spells;
    j["max_accuracy"] = o.max_accuracy;
    j["max_accuracy_all"] = o.max_accuracy_all;
    j["soul_search"] = o.soul_search;
    j["invalidity"] = o.invalidity;
    j["boss_spells"] = o.boss_spells;
    j["spirit_shuffle"] = o.spirit_shuffle;
    j["chest_shuffle"] = o.chest_shuffle;
    j["chests"] = list_mode_name(o.chests);
    j["drops"] = list_mode_name(o.drops);
    j["gifts"] = list_mode_name(o.gifts);
    j["wingsmiths"] = list_mode_name(o.wingsmiths);
    j["shuffle_shannon"] = o.shuffle_shannon;
    j["wingsmith_wings_only"] = o.wingsmith_wings_only;
    j["boss_rewards_shuffler"] = o.boss_rewards_shuffler;
    j["monster_stats"] = o.monster_stats;
    j["variance"] = o.variance;
    j["monster_scale"] = o.monster_scale;
    j["scale_percent"] = o.scale_percent;
    j["exp_by_bst"] = o.exp_by_bst;
    j["exp_boost"] = o.exp_boost;
    j["boss_order"] = o.boss_order;
    j["boss_element"] = o.boss_element;
    j["start_hp"] = o.start_hp;
    j["start_mp"] = o.start_mp;
    j["start_agility"] = o.start_agility;
    j["start_defense"] = o.start_defense;
    j["fast_monastery"] = o.fast_monastery;
    j["fast_blue_cave"] = o.fast_blue_cave;
    j["fast_shamwood"] = o.fast_shamwood;
    j["fast_mammon"] = o.fast_mammon;
    j["unlock_doors"] = o.unlock_doors;
    j["locked_endgame"] = o.locked_endgame;
    j["crystal_return"] = o.crystal_return;
    j["brannoch_return"] = o.brannoch_return;
    j["mammon_door"] = o.mammon_door;
    j["restless_npcs"] = o.restless_npcs;
    j["max_message_speed"] = o.max_message_speed;
    j["hud_lock"] = o.hud_lock;
    j["celtland_drift"] = o.celtland_drift;
    j["level_2_spells"] = o.level_2_spells;
    j["reveal_spirits"] = o.reveal_spirits;
    j["better_dew_drop"] = o.better_dew_drop;
    j["zoom_out"] = o.zoom_out;
    j["ivory_wings"] = o.ivory_wings;
    j["text_improvements"] = o.text_improvements;
    j["lost_keys"] = o.lost_keys;
    j["fire_book"] = o.fire_book;
    j["shannon_hints"] = o.shannon_hints;
    j["enemy_tables"] = o.enemy_tables;
    j["enemy_randomizer"] = o.enemy_randomizer;
    j["encounter_rate"] = o.encounter_rate;
    j["mp_regain"] = o.mp_regain;
    j["staff_hit_mp"] = o.staff_hit_mp;
    j["element_uncap"] = o.element_uncap;
    j["drop_limit_disabled"] = o.drop_limit_disabled;
    j["wing_unlock_indoors"] = o.wing_unlock_indoors;
    j["wing_unlock_skye"] = o.wing_unlock_skye;
    j["wings_never_expire"] = o.wings_never_expire;
    j["text_palette"] = o.text_palette;
    j["staff_palette"] = o.staff_palette;
    j["cloak_palette"] = o.cloak_palette;
    j["brian_palette"] = o.brian_palette;
    j["spell_palette"] = o.spell_palette;
    return j;
}

void zelda64::randomizer::save_options(const Options& o) {
    std::ofstream out(options_path());
    out << options_to_json(o).dump(4);
}

static Options options_from_json(const nlohmann::json& j) {
    Options o;
    auto get = [&j](const char* key, auto& out) {
        auto it = j.find(key);
        if (it != j.end()) {
            try {
                out = it->get<std::remove_reference_t<decltype(out)>>();
            }
            catch (nlohmann::json::type_error&) {}
        }
    };
    std::string mode = "vanilla";
    get("mode", mode);
    o.mode = mode == "randomizer" ? Mode::Randomizer : Mode::Vanilla;
    get("seed", o.seed);
    get("spell_shuffle", o.spell_shuffle);
    get("hinted_spell_names", o.hinted_spell_names);
    get("linear_spell_names", o.linear_spell_names);
    get("early_healing", o.early_healing);
    get("extra_healing", o.extra_healing);
    get("distribute_spells", o.distribute_spells);
    get("spell_overrides", o.spell_overrides);
    get("spell_rebalance", o.spell_rebalance);
    get("bubble", o.bubble);
    get("level_1_spells", o.level_1_spells);
    get("max_accuracy", o.max_accuracy);
    get("max_accuracy_all", o.max_accuracy_all);
    get("soul_search", o.soul_search);
    get("invalidity", o.invalidity);
    get("boss_spells", o.boss_spells);
    get("spirit_shuffle", o.spirit_shuffle);
    get("chest_shuffle", o.chest_shuffle);
    std::string list;
    list = "shuffle"; get("chests", list); o.chests = list_mode_from_name(list);
    list = "shuffle"; get("drops", list); o.drops = list_mode_from_name(list);
    list = "off"; get("gifts", list); o.gifts = list_mode_from_name(list);
    list = "off"; get("wingsmiths", list); o.wingsmiths = list_mode_from_name(list);
    get("shuffle_shannon", o.shuffle_shannon);
    get("wingsmith_wings_only", o.wingsmith_wings_only);
    get("boss_rewards_shuffler", o.boss_rewards_shuffler);
    get("monster_stats", o.monster_stats);
    get("variance", o.variance);
    get("monster_scale", o.monster_scale);
    get("scale_percent", o.scale_percent);
    get("exp_by_bst", o.exp_by_bst);
    get("exp_boost", o.exp_boost);
    get("boss_order", o.boss_order);
    get("boss_element", o.boss_element);
    get("start_hp", o.start_hp);
    get("start_mp", o.start_mp);
    get("start_agility", o.start_agility);
    get("start_defense", o.start_defense);
    get("fast_monastery", o.fast_monastery);
    get("fast_blue_cave", o.fast_blue_cave);
    get("fast_shamwood", o.fast_shamwood);
    get("fast_mammon", o.fast_mammon);
    get("unlock_doors", o.unlock_doors);
    get("locked_endgame", o.locked_endgame);
    get("crystal_return", o.crystal_return);
    get("brannoch_return", o.brannoch_return);
    get("mammon_door", o.mammon_door);
    get("restless_npcs", o.restless_npcs);
    get("max_message_speed", o.max_message_speed);
    get("hud_lock", o.hud_lock);
    get("celtland_drift", o.celtland_drift);
    get("level_2_spells", o.level_2_spells);
    get("reveal_spirits", o.reveal_spirits);
    get("better_dew_drop", o.better_dew_drop);
    get("zoom_out", o.zoom_out);
    get("ivory_wings", o.ivory_wings);
    get("text_improvements", o.text_improvements);
    get("lost_keys", o.lost_keys);
    get("fire_book", o.fire_book);
    get("shannon_hints", o.shannon_hints);
    get("enemy_tables", o.enemy_tables);
    // "enemy_progression" is the name the option had for a day.
    get("enemy_progression", o.enemy_randomizer);
    get("enemy_randomizer", o.enemy_randomizer);
    o.lost_keys = std::clamp(o.lost_keys, 0, 2);
    o.zoom_out = std::clamp(o.zoom_out, 0, 4);
    get("encounter_rate", o.encounter_rate);
    get("mp_regain", o.mp_regain);
    get("staff_hit_mp", o.staff_hit_mp);
    get("element_uncap", o.element_uncap);
    get("drop_limit_disabled", o.drop_limit_disabled);
    get("wing_unlock_indoors", o.wing_unlock_indoors);
    get("wing_unlock_skye", o.wing_unlock_skye);
    get("wings_never_expire", o.wings_never_expire);
    get("text_palette", o.text_palette);
    get("staff_palette", o.staff_palette);
    get("cloak_palette", o.cloak_palette);
    get("brian_palette", o.brian_palette);
    get("spell_palette", o.spell_palette);

    o.encounter_rate = std::clamp(o.encounter_rate, 0, 4);
    o.text_palette = std::clamp(o.text_palette, 0, 5);
    o.staff_hit_mp = std::clamp(o.staff_hit_mp, 0, 9);
    // 7 is off, 8-12 are the speed tiers; treat anything else as vanilla.
    if (o.mp_regain != 7 && (o.mp_regain < 8 || o.mp_regain > 12)) {
        o.mp_regain = 10;
    }
    o.variance = std::clamp(o.variance, 0, 5);
    o.scale_percent = std::clamp(o.scale_percent, 50, 150);
    o.exp_boost = std::clamp(o.exp_boost, 0, 12);
    o.start_hp = std::clamp(o.start_hp, 1, 999);
    o.start_mp = std::clamp(o.start_mp, 0, 999);
    o.start_agility = std::clamp(o.start_agility, 0, 999);
    o.start_defense = std::clamp(o.start_defense, 0, 999);
    return o;
}

zelda64::randomizer::Options zelda64::randomizer::load_options() {
    std::ifstream in(options_path());
    if (!in.good()) {
        return Options{};
    }
    nlohmann::json j;
    try {
        in >> j;
    }
    catch (nlohmann::json::parse_error&) {
        return Options{};
    }
    return options_from_json(j);
}

std::vector<std::string> zelda64::randomizer::preset_names() {
    std::vector<std::string> names;
    nlohmann::json presets = load_presets_file();
    for (auto it = presets.begin(); it != presets.end(); ++it) {
        names.push_back(it.key());
    }
    std::sort(names.begin(), names.end());
    return names;
}

bool zelda64::randomizer::save_preset(const std::string& name, const Options& options) {
    if (name.empty()) {
        return false;
    }
    nlohmann::json presets = load_presets_file();
    nlohmann::json entry = options_to_json(options);
    // Settings only: the seed belongs to the run, not the preset.
    entry.erase("seed");
    presets[name] = std::move(entry);
    write_presets_file(presets);
    return true;
}

bool zelda64::randomizer::load_preset(const std::string& name, Options& out) {
    nlohmann::json presets = load_presets_file();
    auto it = presets.find(name);
    if (it == presets.end() || !it->is_object()) {
        return false;
    }
    std::string seed = out.seed;
    out = options_from_json(*it);
    out.seed = seed;
    return true;
}

bool zelda64::randomizer::delete_preset(const std::string& name) {
    nlohmann::json presets = load_presets_file();
    if (presets.erase(name) == 0) {
        return false;
    }
    write_presets_file(presets);
    return true;
}

const zelda64::randomizer::Options& zelda64::randomizer::active_options() {
    // Easier Quest plays with its own fixed settings in place of the user's,
    // so that the boot writes and every native hook see the same thing.
    if (zelda64::easierquest::active()) {
        return zelda64::easierquest::preset();
    }
    if (!active_loaded) {
        active = load_options();
        active_loaded = true;
    }
    return active;
}

zelda64::randomizer::Result zelda64::randomizer::generate(const Options& options) {
    Result result;
    result.seed_value = seed_value_from_text(options.seed);

    // Merrow's UI forces these whenever Lost Keys is on and disables the
    // toggles, because Lost Keys scatters the gems and wings through the
    // chest and gift lists and hands the wings out itself. Without the
    // same coercion a Lost Keys seed could place a gem into a list that is
    // never written, leaving it unobtainable.
    Options effective = options;
    if (effective.lost_keys != 0) {
        if (effective.chests == ListMode::Off) {
            effective.chests = ListMode::Shuffle;
        }
        if (effective.gifts == ListMode::Off) {
            effective.gifts = ListMode::Shuffle;
        }
        if (effective.drops == ListMode::Off) {
            effective.drops = ListMode::Shuffle;
        }
        effective.wingsmiths = ListMode::Off;
        effective.ivory_wings = true;
    }

    Builder builder(effective, result.seed_value);
    builder.log("Quest 64 Recompiled randomizer (port of Merrow)");
    builder.log("Seed: " + options.seed + " (" + std::to_string(result.seed_value) + ")");
    builder.log("");
    builder.build();

    result.writes = std::move(builder.writes);
    result.spoiler = std::move(builder.spoiler);
    result.beigis_moved = builder.beigis_moved;
    result.spirit_slots = std::move(builder.spirit_slots);
    result.chest_placements = std::move(builder.chest_placements);
    result.progression_tables = std::move(builder.progression_tables);
    return result;
}

const zelda64::randomizer::NativeState& zelda64::randomizer::native_state() {
    return native;
}

void zelda64::randomizer::apply_at_boot(uint8_t* rdram) {
    reset_native_scratch();
    const Options& options = active_options();
    if (options.mode != Mode::Randomizer) {
        return;
    }

    Result result = generate(options);
    native.beigis_moved = result.beigis_moved;
    native.spirit_slots = result.spirit_slots;
    native.chest_placements = result.chest_placements;
    if (!result.progression_tables.empty()) {
        std::string unused;
        progression::set_active(progression::make_plan(result.progression_tables, options, unused));
    }

    std::span<const uint8_t> rom = recomp::get_rom();
    std::vector<uint8_t> patched(rom.begin(), rom.end());

    // The boot segment (ROM 0x1000 onwards, 1MB) has already been copied to
    // RAM at the entrypoint, so writes that land in it go to RAM as well.
    constexpr uint32_t boot_rom_start = 0x1000;
    constexpr uint32_t boot_size = 0x100000;
    constexpr int32_t boot_ram_start = 0x80000400;

    for (const Write& write : result.writes) {
        for (size_t i = 0; i < write.data.size(); i++) {
            uint32_t offset = write.rom_offset + static_cast<uint32_t>(i);
            if (offset >= patched.size()) {
                break;
            }
            patched[offset] = write.data[i];
            if (offset >= boot_rom_start && offset < boot_rom_start + boot_size) {
                MEM_B(0, boot_ram_start + static_cast<int32_t>(offset - boot_rom_start)) = static_cast<int8_t>(write.data[i]);
            }
        }
    }

    recomp::set_rom_contents(std::move(patched));

    std::ofstream spoiler(spoiler_path());
    spoiler << result.spoiler;
}

std::string zelda64::randomizer::describe(const Options& options) {
    if (options.mode != Mode::Randomizer) {
        return "Randomizer off";
    }
    return "Randomizer on, seed " + (options.seed.empty() ? std::string("(empty)") : options.seed);
}
