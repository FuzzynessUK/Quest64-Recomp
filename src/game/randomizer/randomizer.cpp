#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <optional>
#include <random>

#include "randomizer.h"
#include "merrow_data.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"

// The shuffle and patch logic below follows Merrow's Shuffle.cs and
// QuestPatchBuild.cs section by section; comments name the section they come
// from so the two can be compared. Merrow is copyright (c) 2021 Jonah Davidson
// (Hangedman), MIT licence.

namespace data = merrow::data;
using zelda64::randomizer::ListMode;
using zelda64::randomizer::Options;
using zelda64::randomizer::Write;

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
        std::vector<int> crashlock = data::crashlock;
        std::vector<std::string> item_capital_case = data::itemcapitalcase;
        std::vector<std::string> new_spell_item_desc = data::newSpellItemDesc;

        // Shuffle results.
        std::vector<int> shuffles = std::vector<int>(player_spells, -1);
        std::vector<int> new_item_spells = std::vector<int>(6, 0);
        std::vector<int> item_spell_fix = std::vector<int>(6, 0);
        std::vector<std::string> hint_names = std::vector<std::string>(player_spells);
        std::vector<int> chests = std::vector<int>(chest_count);
        std::vector<int> drops = std::vector<int>(drop_count);
        std::vector<int> gifts;
        std::vector<int> wings = std::vector<int>(wing_count);
        std::vector<int> monster_stats = data::monsterstatvanilla;
        std::vector<int> boss_order = std::vector<int>(boss_count);
        int guilty_element = 4;
        bool beigis_moved = false;

        Builder(const Options& opts, uint32_t seed) : options(opts), rng(seed) {}

        double difficulty_scale() const {
            return options.scale_percent / 100.0;
        }

        double extremity() const {
            return options.variance * 0.1;
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

        void shuffle_spells() {
            for (int i = 0; i < player_spells; i++) {
                shuffles[i] = -1;
            }
            std::vector<int> reorg = rng.count_and_shuffle(player_spells);

            if (!options.spell_shuffle) {
                for (int i = 0; i < player_spells; i++) {
                    shuffles[i] = i;
                }
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

            // Spell items must keep rules that match what they now cast.
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

        void shuffle_spell_names() {
            for (int i = 0; i < player_spells; i++) {
                int pick = rng.next(4);
                if (options.linear_spell_names) {
                    pick = 0;
                }
                const auto& mine = data::shuffleNames2[i];
                const auto& theirs = data::shuffleNames2[shuffles[i]];
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
        void roll_list(std::vector<int>& list, ListMode mode) {
            if (mode == ListMode::Random) {
                for (int& slot : list) {
                    slot = rng.next(item_count);
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

            for (int l = 0; l < drop_count; l++) {
                drops[l] = data::dropdata[l * 2 + 1];
            }
            roll_list(drops, options.drops);

            gifts.assign(options.shuffle_shannon ? 8 : 10, 0);
            for (size_t l = 0; l < gifts.size(); l++) {
                gifts[l] = data::itemgranters[l * 2 + 1];
            }
            if (options.gifts != ListMode::Off) {
                if (options.gifts == ListMode::Random) {
                    for (int& slot : gifts) {
                        slot = rng.next(item_count);
                    }
                    if (!options.shuffle_shannon) {
                        // Keep one book and one key in circulation.
                        gifts[8] = 24;
                        gifts[9] = 25;
                    }
                }
                rng.shuffle(gifts);
                if (!options.shuffle_shannon) {
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
                roll_list(wings, options.wingsmiths);
            }
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
                guilty_element = rng.next(0, 4);
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
                        monster_stats[(i * 6) + 4] = value == 0 ? 1 : value;
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

        void patch_spells() {
            // Spell Damage Rebalance
            int rebalanced = 0;
            if (options.spell_rebalance) {
                for (int i = 0; i < 18; i++) {
                    std::string dmg = hex4(data::damageRebalance[i * 3 + 2]);
                    std::string& spell = spells[data::damageRebalance[i * 3] * 4 + 3];
                    spell = spell.substr(0, 24) + dmg + spell.substr(28);
                }
                rebalanced = 2;
                log("Spell damage rebalanced.");
            }
            if (rebalanced != 0 && !options.spell_shuffle) {
                for (int i = 0; i < 18; i++) {
                    uint32_t addr = std::stoul(spells[data::damageRebalance[i * 3] * 4 + 2]) + 12;
                    add_u16(addr, data::damageRebalance[i * 3 + rebalanced]);
                }
            }

            // Skelebat group in Blue Cave that can crash under lag.
            add_hex("667260", "000000060000000100000001");

            if (!options.spell_shuffle) {
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
                    add_hex(data::shuffleNames2[i][5], data::shuffleNames2[i][6]);
                }
                for (int i = 0; i < player_spells; i++) {
                    // Spell names are plain ASCII, padded to 16 bytes.
                    std::vector<uint8_t> name(hint_names[i].begin(), hint_names[i].end());
                    name.resize(16, 0);
                    add(hex_addr(data::shuffleNames2[i][4]), name);
                }
            }

            // Spell item softlock protection
            for (int i = 0; i < 6; i++) {
                uint32_t addr = std::stoul(data::items[25 + (i * 3)], nullptr, 16);
                int rule = item_spell_fix[i] == 0 ? 0x0002 : (item_spell_fix[i] == 1 ? 0x0001 : 0x0003);
                add_u16(addr, rule);
            }

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
                constexpr int boss_items[boss_count] = { 20, 21, 22, 255, 23, 255, 255 };
                constexpr uint32_t boss_addresses[boss_count] = { 14186532, 14186588, 14186644, 14186700, 14186756, 14186812, 14186868 };
                static const char* const boss_names[boss_count] = { "Solvaring", "Zelse", "Nepty", "Shilf", "Fargo", "Guilty", "Beigis" };
                log("");
                log("BOSS ORDER (arena: boss):");
                for (int i = 0; i < boss_count; i++) {
                    add_hex(data::bosslocdata[boss_order[i] * 4], data::bosslocdata[(i * 4) + 1]);
                    add_hex(data::bosslocdata[(boss_order[i] * 4) + 2], data::bosslocdata[(i * 4) + 3]);
                    add_hex(data::rewardhpdata[boss_order[i] * 2], data::rewardhpdata[(i * 2) + 1]);
                    add_u8(boss_addresses[boss_order[i]], boss_items[i]);
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
                    double hue = rng.next_double() * 360.0;
                    bool light = rng.next(2) != 0;
                    const std::vector<std::string>& base =
                        light ? data::baseRedTextPalette : data::baseDarkTextPalette;
                    std::string palette = "F83E";
                    for (int i = 0; i < 3; i++) {
                        palette += rgba_to_rgba5551(hue_shift(rgba5551_to_rgba(base[i]), hue));
                    }
                    add_hex("D3E240", palette);
                    log("Text palette: random (" + std::to_string(static_cast<int>(std::lround(hue))) +
                        (light ? " light)." : " dark)."));
                }
                else if (fixed_palettes[choice] != nullptr) {
                    add_hex("D3E240", fixed_palettes[choice]);
                    log(std::string("Text palette: ") + palette_names[choice] + ".");
                }
            }

            if (options.staff_palette) {
                // The staff texture is 768 RGBA5551 colours, all rotated by
                // the same hue.
                double hue = rng.next_double() * 360.0;
                std::string palette;
                palette.reserve(768 * 4);
                for (int i = 0; i < 768; i++) {
                    palette += rgba_to_rgba5551(hue_shift(rgba5551_to_rgba(data::stafftexture.substr(i * 4, 4)), hue));
                }
                add_hex("86EB70", palette);
                log("Staff palette: random (" + std::to_string(static_cast<int>(std::lround(hue))) + ").");
            }
        }

        void build() {
            shuffle_spells();
            shuffle_spell_names();
            shuffle_items();
            shuffle_monsters();

            patch_spells();
            patch_items();
            patch_monsters();
            patch_misc();
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

void zelda64::randomizer::save_options(const Options& o) {
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
    j["chests"] = list_mode_name(o.chests);
    j["drops"] = list_mode_name(o.drops);
    j["gifts"] = list_mode_name(o.gifts);
    j["wingsmiths"] = list_mode_name(o.wingsmiths);
    j["shuffle_shannon"] = o.shuffle_shannon;
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
    j["encounter_rate"] = o.encounter_rate;
    j["mp_regain"] = o.mp_regain;
    j["staff_hit_mp"] = o.staff_hit_mp;
    j["element_uncap"] = o.element_uncap;
    j["drop_limit_disabled"] = o.drop_limit_disabled;
    j["wing_unlock_indoors"] = o.wing_unlock_indoors;
    j["wing_unlock_skye"] = o.wing_unlock_skye;
    j["text_palette"] = o.text_palette;
    j["staff_palette"] = o.staff_palette;

    std::ofstream out(options_path());
    out << j.dump(4);
}

zelda64::randomizer::Options zelda64::randomizer::load_options() {
    Options o;
    std::ifstream in(options_path());
    if (!in.good()) {
        return o;
    }
    nlohmann::json j;
    try {
        in >> j;
    }
    catch (nlohmann::json::parse_error&) {
        return o;
    }

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
    std::string list;
    list = "shuffle"; get("chests", list); o.chests = list_mode_from_name(list);
    list = "shuffle"; get("drops", list); o.drops = list_mode_from_name(list);
    list = "off"; get("gifts", list); o.gifts = list_mode_from_name(list);
    list = "off"; get("wingsmiths", list); o.wingsmiths = list_mode_from_name(list);
    get("shuffle_shannon", o.shuffle_shannon);
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
    get("encounter_rate", o.encounter_rate);
    get("mp_regain", o.mp_regain);
    get("staff_hit_mp", o.staff_hit_mp);
    get("element_uncap", o.element_uncap);
    get("drop_limit_disabled", o.drop_limit_disabled);
    get("wing_unlock_indoors", o.wing_unlock_indoors);
    get("wing_unlock_skye", o.wing_unlock_skye);
    get("text_palette", o.text_palette);
    get("staff_palette", o.staff_palette);

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

const zelda64::randomizer::Options& zelda64::randomizer::active_options() {
    if (!active_loaded) {
        active = load_options();
        active_loaded = true;
    }
    return active;
}

zelda64::randomizer::Result zelda64::randomizer::generate(const Options& options) {
    Result result;
    result.seed_value = seed_value_from_text(options.seed);

    Builder builder(options, result.seed_value);
    builder.log("Quest 64 Recompiled randomizer (port of Merrow)");
    builder.log("Seed: " + options.seed + " (" + std::to_string(result.seed_value) + ")");
    builder.log("");
    builder.build();

    result.writes = std::move(builder.writes);
    result.spoiler = std::move(builder.spoiler);
    result.beigis_moved = builder.beigis_moved;
    return result;
}

const zelda64::randomizer::NativeState& zelda64::randomizer::native_state() {
    return native;
}

void zelda64::randomizer::apply_at_boot(uint8_t* rdram) {
    const Options& options = active_options();
    if (options.mode != Mode::Randomizer) {
        return;
    }

    Result result = generate(options);
    native.beigis_moved = result.beigis_moved;

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
        return "Vanilla";
    }
    return "Randomizer, seed " + (options.seed.empty() ? std::string("(empty)") : options.seed);
}
