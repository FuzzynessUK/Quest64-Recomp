#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <span>
#include <type_traits>
#include <vector>

#include "enhancements.h"
#include "archipelago.h"
#include "hardmode.h"
#include "randomizer.h"
#include "zelda_render.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
#include "zelda_debug.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"
#include "librecomp/addresses.hpp"

namespace data = merrow::data;
using zelda64::enhancements::Options;

namespace {
    Options active;
    bool active_loaded = false;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "enhancements.json";
    }

    // Brian's live stats, the same block the cheats menu edits.
    constexpr int32_t gPlayerMainData = 0x8007BA80;
    constexpr int32_t player_hp = gPlayerMainData + 0x04;
    constexpr int32_t player_max_hp = gPlayerMainData + 0x06;

    // Magic Barrier, as found in RAM: a byte counting how much barrier is
    // left. The value is one more than the number of enemy attacks it will
    // absorb, so the stock 4 is three attacks and 6 is five.
    constexpr int32_t magic_barrier_timer = 0x8007BB42;
    constexpr int magic_barrier_bonus = 2;
    constexpr int magic_barrier_min = 3;
    constexpr int magic_barrier_max = 6;

    // Every monster's stat row, as used by the randomizer: six halfwords per
    // monster with HP first, and HP is stored twice in a row.
    constexpr int monster_count = 75;

    // Healing Lv2 potency, the one field that differs between the US and
    // Japanese spell tables.
    constexpr int healing_potency = 0x0C;
    constexpr int jp_healing_lv2 = 16;

    // Spell entries are found by name so a change to the table cannot
    // silently point a patch at a different spell.
    uint32_t spell_entry_address(const std::string& name) {
        for (size_t i = 0; i + 3 < data::spells.size(); i += 4) {
            if (data::spells[i] == name) {
                return static_cast<uint32_t>(std::stoul(data::spells[i + 1], nullptr, 16));
            }
        }
        return 0;
    }

    struct Write {
        uint32_t rom_offset;
        uint16_t value;
        // Monster HP is a halfword; the spell effect parameter is one byte.
        bool single_byte = false;
    };

    std::vector<Write> build_writes(const Options& options) {
        std::vector<Write> writes;
        if (options.one_hit_ko) {
            // Bosses included: the point is that nothing survives a hit.
            for (int monster = 0; monster < monster_count; monster++) {
                uint32_t address = static_cast<uint32_t>(
                    std::stoul(data::monsterstatlocations[monster][0], nullptr, 16));
                writes.push_back({ address, 1, false });
                writes.push_back({ address + 2, 1, false });
            }
        }
        if (options.jp_healing) {
            uint32_t entry = spell_entry_address("Healing Lv2");
            if (entry != 0) {
                writes.push_back({ entry + healing_potency, jp_healing_lv2, false });
            }
        }
        return writes;
    }
}

zelda64::enhancements::Options zelda64::enhancements::load_options() {
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
    get("one_hit_ko", o.one_hit_ko);
    get("n64_mode", o.n64_mode);
    get("saved_resolution", o.saved_resolution);
    get("saved_aspect", o.saved_aspect);
    get("saved_antialiasing", o.saved_antialiasing);
    get("saved_hud_ratio", o.saved_hud_ratio);
    get("speedrun_timer", o.speedrun_timer);
    get("timer_position", o.timer_position);
    o.timer_position = std::clamp(o.timer_position, 0, 5);
    get("jp_healing", o.jp_healing);
    get("exit_from_anywhere", o.exit_from_anywhere);
    get("longer_magic_barrier", o.longer_magic_barrier);
    get("boss_max_mp", o.boss_max_mp);
    get("double_exp", o.double_exp);
    get("fast_mp_recovery", o.fast_mp_recovery);
    get("faster_walk", o.faster_walk);
    get("stack_items", o.stack_items);
    get("remove_borders", o.remove_borders);
    get("hud_hp_custom", o.hud_hp_custom);
    get("hud_hp_x", o.hud_hp_x);
    get("hud_hp_y", o.hud_hp_y);
    get("hud_sp_custom", o.hud_sp_custom);
    get("hud_sp_x", o.hud_sp_x);
    get("hud_sp_y", o.hud_sp_y);
    get("minimap", o.minimap);
    get("minimap_custom", o.minimap_custom);
    get("minimap_x", o.minimap_x);
    get("minimap_y", o.minimap_y);
    get("minimap_size", o.minimap_size);
    get("minimap_zoom", o.minimap_zoom);
    get("minimap_background", o.minimap_background);
    get("minimap_hide_spirits", o.minimap_hide_spirits);
    get("minimap_hide_chests", o.minimap_hide_chests);
    get("minimap_hide_givers", o.minimap_hide_givers);
    get("minimap_checks_guide", o.minimap_checks_guide);
    get("hide_compass", o.hide_compass);
    get("stat_up_effect", o.stat_up_effect);
    get("spell_notice", o.spell_notice);
    get("song_notice", o.song_notice);
    get("song_notice_one", o.song_notice_one);
    get("item_notice", o.item_notice);
    get("ap_notice", o.ap_notice);
    get("notifications", o.notifications);
    get("notify_never_expire", o.notify_never_expire);
    get("notify_max", o.notify_max);
    o.notify_max = std::clamp(o.notify_max, 1, 10);
    get("notify_position", o.notify_position);
    o.notify_position = std::clamp(o.notify_position, 0, 8);
    get("hard_mode", o.hard_mode);
    get("easier_quest", o.easier_quest);
    return o;
}

void zelda64::enhancements::save_options(const Options& o) {
    nlohmann::json j;
    j["one_hit_ko"] = o.one_hit_ko;
    j["n64_mode"] = o.n64_mode;
    j["saved_resolution"] = o.saved_resolution;
    j["saved_aspect"] = o.saved_aspect;
    j["saved_antialiasing"] = o.saved_antialiasing;
    j["saved_hud_ratio"] = o.saved_hud_ratio;
    j["speedrun_timer"] = o.speedrun_timer;
    j["timer_position"] = o.timer_position;
    j["jp_healing"] = o.jp_healing;
    j["exit_from_anywhere"] = o.exit_from_anywhere;
    j["longer_magic_barrier"] = o.longer_magic_barrier;
    j["boss_max_mp"] = o.boss_max_mp;
    j["double_exp"] = o.double_exp;
    j["fast_mp_recovery"] = o.fast_mp_recovery;
    j["faster_walk"] = o.faster_walk;
    j["stack_items"] = o.stack_items;
    j["remove_borders"] = o.remove_borders;
    j["hud_hp_custom"] = o.hud_hp_custom;
    j["hud_hp_x"] = o.hud_hp_x;
    j["hud_hp_y"] = o.hud_hp_y;
    j["hud_sp_custom"] = o.hud_sp_custom;
    j["hud_sp_x"] = o.hud_sp_x;
    j["hud_sp_y"] = o.hud_sp_y;
    j["minimap"] = o.minimap;
    j["minimap_custom"] = o.minimap_custom;
    j["minimap_x"] = o.minimap_x;
    j["minimap_y"] = o.minimap_y;
    j["minimap_size"] = o.minimap_size;
    j["minimap_zoom"] = o.minimap_zoom;
    j["minimap_background"] = o.minimap_background;
    j["minimap_hide_spirits"] = o.minimap_hide_spirits;
    j["minimap_hide_chests"] = o.minimap_hide_chests;
    j["minimap_hide_givers"] = o.minimap_hide_givers;
    j["minimap_checks_guide"] = o.minimap_checks_guide;
    j["hide_compass"] = o.hide_compass;
    j["stat_up_effect"] = o.stat_up_effect;
    j["spell_notice"] = o.spell_notice;
    j["song_notice"] = o.song_notice;
    j["song_notice_one"] = o.song_notice_one;
    j["item_notice"] = o.item_notice;
    j["ap_notice"] = o.ap_notice;
    j["notifications"] = o.notifications;
    j["notify_never_expire"] = o.notify_never_expire;
    j["notify_max"] = o.notify_max;
    j["notify_position"] = o.notify_position;
    j["hard_mode"] = o.hard_mode;
    j["easier_quest"] = o.easier_quest;
    std::ofstream out(options_path());
    out << j.dump(4);
}

const zelda64::enhancements::Options& zelda64::enhancements::active_options() {
    // Connected to Archipelago as the game booted: the seed decides the
    // gameplay ones (see archipelago::apply_seed_settings), the player's own
    // file the rest.
    if (zelda64::archipelago::seed_settings_in_effect()) {
        static Options seed;
        static bool seed_built = false;
        if (!seed_built) {
            seed = load_options();
            zelda64::archipelago::apply_seed_settings(seed);
            seed_built = true;
        }
        return seed;
    }
    if (!active_loaded) {
        active = load_options();
        // Easier Quest's two JP options. Checked against the saved
        // hard_mode flag rather than hardmode::active(), which is not known
        // yet the first time this runs; Hard Mode ignores both anyway.
        if (active.easier_quest && !active.hard_mode) {
            active.jp_healing = true;
            active.longer_magic_barrier = true;
        }
        active_loaded = true;
    }
    return active;
}

void zelda64::enhancements::apply_at_boot(uint8_t* rdram) {
    const Options& options = active_options();
    zelda64::renderer::set_hud_layout(options.hud_hp_custom, options.hud_hp_x, options.hud_hp_y,
        options.hud_sp_custom, options.hud_sp_x, options.hud_sp_y);
    zelda64::renderer::set_borders_removed(options.remove_borders);
    set_hide_compass(options.hide_compass);
    // Fast MP Recovery: the walking regen byte is in the boot segment, which
    // is in RAM already and never read from the ROM again, so it is written
    // there. 0x28 is the fastest of Merrow's tiers (vanilla 0x41).
    if (options.fast_mp_recovery && !zelda64::hardmode::active()) {
        MEM_B(0, static_cast<int32_t>(0x80070F39)) = 0x28;
    }
    std::vector<Write> writes = build_writes(options);
    if (writes.empty()) {
        return;
    }

    // Reads back whatever the randomizer left, so the two stack rather than
    // one overwriting the other.
    std::span<const uint8_t> rom = recomp::get_rom();
    std::vector<uint8_t> patched(rom.begin(), rom.end());

    for (const Write& write : writes) {
        if (write.single_byte) {
            if (write.rom_offset < patched.size()) {
                patched[write.rom_offset] = static_cast<uint8_t>(write.value & 0xFF);
            }
            continue;
        }
        if (write.rom_offset + 1 >= patched.size()) {
            continue;
        }
        patched[write.rom_offset] = static_cast<uint8_t>((write.value >> 8) & 0xFF);
        patched[write.rom_offset + 1] = static_cast<uint8_t>(write.value & 0xFF);
    }

    recomp::set_rom_contents(std::move(patched));
}

int zelda64::enhancements::element_cap(uint8_t* rdram) {
    if (zelda64::hardmode::active()) {
        return zelda64::hardmode::element_cap(rdram);
    }
    const zelda64::randomizer::Options& r = zelda64::randomizer::active_options();
    return r.mode == zelda64::randomizer::Mode::Randomizer && r.element_uncap ? 99 : 50;
}

bool zelda64::enhancements::elements_all_maxed(uint8_t* rdram) {
    // gPlayerMainData +0x24..+0x27: Fire, Earth, Water, Wind.
    constexpr int32_t elements = 0x8007BA80 + 0x24;
    int cap = element_cap(rdram);
    for (int i = 0; i < 4; i++) {
        if (static_cast<int>(MEM_BU(i, elements)) < cap) {
            return false;
        }
    }
    return true;
}

// func_80002F60 at 0x800032D8: a0 is the level the overworld spirit grab
// compares all four elements against, and when every one is at it the grab
// is skipped outright - the spirit cannot be picked up at all, which leaves
// it standing there and, as a check, never sent. Run after the randomizer's
// and Hard Mode's hooks at the same address: a value no element byte can
// equal lets the grab through, and the screen it opens is closed again at
// once by on_frame (nothing is left to raise). Not under Hard Mode, whose
// cap rises with each boss and turns a spirit away on purpose so it can be
// come back for.
extern "C" void quest64_enh_spirit_cap(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    if (zelda64::hardmode::active()) {
        return;
    }
    ctx->r4 = 0x100;
}

// ---- Stack items ---------------------------------------------------------
//
// Hard Mode's item menu, redone for the vanilla game. The bag, gInventory
// (0x8008CF78, 150 bytes, 0xFF empty), is kept in Hard Mode's own layout:
//
//   bytes 0-74    one entry per item: a stackable item once, however many
//                 there are; anything else once per copy
//   75 + id       for a stackable id: 0x80 | the copies beyond the first,
//                 or 0xFF when there is only the one
//   the rest      0xFF
//
// A vanilla item id is at most 0x19, so a stacked bag can never be mistaken
// for a plain one. Nothing in the game's own code changes: whatever adds an
// item (a chest, a shop, a drop, Archipelago, the cheats) puts it in the
// first free slot as usual, and using one takes its entry out as usual - the
// frame after, the bag is put back into this shape, which merges a new copy
// into its stack and brings a used one's entry back with one fewer. The
// list is kept in item-id order (Hard Mode sorts too), so an entry stays
// where it was when one of a stack is used. Not while the item menu is open.
//
// The count is shown the way Hard Mode shows it: at the end of the item's
// description, "(03)". The description is copied into memory of our own
// with room for the number and the text table's pointer (0x803A9954, entry
// 32 + id; entries 0-31 are the names) aimed at the copy. Names, the item
// data and every other text are left exactly as they are.
//
// Turning the option off puts a stacked bag back into one slot per item.
// Hard Mode does all of this itself, so none of it runs there.
namespace {
    constexpr int32_t bag = 0x8008CF78;
    constexpr int bag_slots = 150;
    constexpr int list_slots = 75;
    constexpr int stack_base = 75;
    constexpr int stack_ids = 32;
    constexpr int bag_empty = 0xFF;
    constexpr int max_stack = 99;
    // Hard Mode's stackable set among the vanilla items - the consumables,
    // the flute, the bell, the Replica, the shoes and the two amulets - and
    // the six wings (0x0E-0x13), which Hard Mode leaves apart.
    constexpr int stackable_last = 0x13;
    constexpr int32_t item_text_table = 0x803A9954;
    constexpr int description_entry = 32;
    constexpr int32_t item_menu_mask = 0x8007B2E4;
    constexpr uint32_t item_menu_bit = 0x1;
    constexpr int32_t game_mode = 0x8007B2E0;
    constexpr int32_t next_map = 0x80084EE4;

    bool stackable(int id) { return id >= 0 && id <= stackable_last; }

    bool in_game(uint8_t* rdram) {
        int mode = MEM_HU(0, game_mode);
        return mode != 2 && mode != 4 && static_cast<int32_t>(MEM_W(0, next_map)) != -1;
    }

    // Reads the bag in either layout into a count per id. False if it holds
    // something neither layout would (then it is left alone).
    bool read_bag(uint8_t* rdram, int (&counts)[256], std::vector<int>* order) {
        for (int& c : counts) c = 0;
        for (int slot = 0; slot < bag_slots; slot++) {
            int b = MEM_BU(slot, bag);
            if (b == bag_empty) {
                continue;
            }
            if (b & 0x80) {
                if (slot < stack_base || slot >= stack_base + stack_ids) {
                    return false;
                }
                counts[slot - stack_base] += b & 0x7F;
                continue;
            }
            counts[b]++;
            if (order) order->push_back(b);
        }
        return true;
    }

    // The bag as stack_bag last left it. Using an item (0x800220FC,
    // func_80021434) takes its entry out by moving every later byte of all
    // 150 down one - the counts at 75 + id with them - so after a use the
    // bag cannot be read by position: a count sits one slot too low and is
    // taken for the id below (Spirit Light's lands in the list, and then
    // nothing is stacked again). But nothing but this code ever writes a
    // count, and the game only shifts them or leaves them, so they are still
    // the ones last_bag had, in the same order. Matched up that way, each
    // count is known to be its own id's wherever it has ended up.
    uint8_t last_bag[bag_slots];
    bool have_last = false;

    // The counts per id from the bag as it is now, with the count bytes
    // matched to last_bag by order. False if they do not match.
    bool counts_since_last(uint8_t* rdram, int (&counts)[256]) {
        int last_ids[stack_ids];
        uint8_t last_values[stack_ids];
        int last_n = 0;
        for (int id = 0; id < stack_ids; id++) {
            uint8_t b = last_bag[stack_base + id];
            if (b != bag_empty) {
                last_ids[last_n] = id;
                last_values[last_n++] = b;
            }
        }
        for (int& c : counts) c = 0;
        int k = 0;
        for (int slot = 0; slot < bag_slots; slot++) {
            uint8_t b = static_cast<uint8_t>(MEM_BU(slot, bag));
            if (b == bag_empty) {
                continue;
            }
            if (b & 0x80) {
                if (k >= last_n || b != last_values[k]) {
                    return false;
                }
                counts[last_ids[k++]] += b & 0x7F;
                continue;
            }
            counts[b]++;
        }
        return k == last_n;
    }

    // menu_open: the list on screen is only rewritten when that leaves it
    // as it was before or as the game has just made it - an item used, its
    // entry put back while more are left - so nothing moves under the
    // cursor. A new item waits for the menu to close.
    // A bag saved while the above went wrong: its counts slid down by the
    // items used since, and nothing read it again. Take the smallest slide
    // that puts every count back on an id that stacks.
    bool read_slid_bag(uint8_t* rdram, int (&counts)[256]) {
        for (int slide = 1; slide < stack_base; slide++) {
            bool fits = true;
            for (int& c : counts) c = 0;
            for (int slot = 0; slot < bag_slots && fits; slot++) {
                int b = MEM_BU(slot, bag);
                if (b == bag_empty) {
                    continue;
                }
                if (b & 0x80) {
                    int id = slot + slide - stack_base;
                    fits = stackable(id);
                    if (fits) counts[id] += b & 0x7F;
                    continue;
                }
                counts[b]++;
            }
            if (fits) {
                return true;
            }
        }
        return false;
    }

    void stack_bag(uint8_t* rdram, bool menu_open) {
        int counts[256];
        if (!(have_last && counts_since_last(rdram, counts))) {
            if (menu_open) {
                return;
            }
            if (!read_bag(rdram, counts, nullptr) && !read_slid_bag(rdram, counts)) {
                return;
            }
        }
        for (int id = 0; id <= stackable_last; id++) {
            counts[id] = std::min(counts[id], max_stack);
        }
        uint8_t shaped[bag_slots];
        std::memset(shaped, bag_empty, sizeof shaped);
        int n = 0;
        for (int id = 0; id < 0x80; id++) {
            if (counts[id] == 0) {
                continue;
            }
            int entries = stackable(id) ? 1 : counts[id];
            if (n + entries > list_slots) {
                have_last = false;
                return;   // more than the list holds: leave the bag as it is
            }
            for (int k = 0; k < entries; k++) {
                shaped[n++] = static_cast<uint8_t>(id);
            }
            if (stackable(id)) {
                int extra = counts[id] - 1;
                if (extra > 0) {
                    shaped[stack_base + id] = static_cast<uint8_t>(0x80 | extra);
                }
            }
        }
        if (menu_open) {
            bool as_before = std::memcmp(shaped, last_bag, list_slots) == 0;
            bool as_now = true;
            for (int slot = 0; slot < list_slots && as_now; slot++) {
                as_now = MEM_BU(slot, bag) == shaped[slot];
            }
            if (!as_before && !as_now) {
                return;
            }
        }
        for (int slot = 0; slot < bag_slots; slot++) {
            if (MEM_BU(slot, bag) != shaped[slot]) {
                for (int s = 0; s < bag_slots; s++) {
                    MEM_B(s, bag) = static_cast<int8_t>(shaped[s]);
                }
                break;
            }
        }
        std::memcpy(last_bag, shaped, sizeof last_bag);
        have_last = true;
    }

    // The option is off: a bag that was stacked goes back to a slot a copy.
    void unstack_bag(uint8_t* rdram) {
        bool stacked = false;
        for (int id = 0; id < stack_ids && !stacked; id++) {
            stacked = (MEM_BU(stack_base + id, bag) & 0x80) != 0 && MEM_BU(stack_base + id, bag) != bag_empty;
        }
        if (!stacked) {
            return;
        }
        int counts[256];
        if (!read_bag(rdram, counts, nullptr)) {
            return;
        }
        uint8_t flat[bag_slots];
        std::memset(flat, bag_empty, sizeof flat);
        int n = 0;
        for (int id = 0; id < 0x80 && n < bag_slots; id++) {
            for (int k = 0; k < counts[id] && n < bag_slots; k++) {
                flat[n++] = static_cast<uint8_t>(id);
            }
        }
        for (int s = 0; s < bag_slots; s++) {
            MEM_B(s, bag) = static_cast<int8_t>(flat[s]);
        }
    }

    // The descriptions with a count on the end, one per stackable item.
    int32_t counted_text[stackable_last + 1] = {};
    int32_t digits_at[stackable_last + 1] = {};

    void show_counts(uint8_t* rdram) {
        int counts[256];
        if (!read_bag(rdram, counts, nullptr)) {
            return;
        }
        for (int id = 0; id <= stackable_last; id++) {
            int32_t entry = item_text_table + 4 * (description_entry + id);
            int32_t now = static_cast<int32_t>(MEM_W(0, entry));
            if (counted_text[id] == 0 || now != counted_text[id]) {
                // First time, or the table has been loaded afresh: copy the
                // description as it is now and point the table at the copy.
                if ((static_cast<uint32_t>(now) >> 24) != 0x80) {
                    continue;
                }
                constexpr int longest = 240;
                int len = 0;
                while (len < longest && MEM_BU(len, now) != 0xFF) {
                    len++;
                }
                if (counted_text[id] == 0) {
                    void* mem = recomp::alloc(rdram, longest + 16);
                    if (mem == nullptr) {
                        return;
                    }
                    counted_text[id] = static_cast<int32_t>(
                        static_cast<uint32_t>(reinterpret_cast<uint8_t*>(mem) - rdram) + 0x80000000u);
                }
                // The count is " x15": a space, x in the letter set (0x82),
                // then two digits in the digit set (0x80). Four characters, so
                // it goes at the end of a line with room for it - the last
                // one if it can, as Hard Mode puts it - or on a line of its
                // own when the description has fewer than three and none has
                // room. Hard Mode's "(15)" is seven characters and runs past
                // the box on the vanilla descriptions.
                const uint8_t tag[] = { 0x7F, 0x82, 0x17, 0x80, 0x00, 0x00 };
                constexpr int tag_width = 4;
                constexpr int box_width = 20;   // characters a description line holds
                std::vector<uint8_t> text;
                for (int i = 0; i < len; i++) {
                    text.push_back(static_cast<uint8_t>(MEM_BU(i, now)));
                }
                // Lines are split by 0xE0; bytes of 0x80 and up are control
                // codes and take no room.
                std::vector<size_t> line_end;
                std::vector<int> line_width;
                int width = 0;
                for (size_t i = 0; i < text.size(); i++) {
                    if (text[i] == 0xE0) {
                        line_end.push_back(i);
                        line_width.push_back(width);
                        width = 0;
                    }
                    else if (text[i] < 0x80) {
                        width++;
                    }
                }
                line_end.push_back(text.size());
                line_width.push_back(width);
                int pick = -1;
                for (int l = static_cast<int>(line_end.size()) - 1; l >= 0 && pick < 0; l--) {
                    if (line_width[l] + tag_width <= box_width) {
                        pick = l;
                    }
                }
                // The character set in use where the tag goes (0x80 digits and
                // signs, 0x81 capitals, 0x82 small letters), put back after
                // it so the rest of the text reads as it did.
                auto set_at = [&text](size_t pos) {
                    uint8_t set = 0x81;
                    for (size_t i = 0; i < pos; i++) {
                        if (text[i] == 0x80 || text[i] == 0x81 || text[i] == 0x82) set = text[i];
                    }
                    return set;
                };
                size_t at;
                if (pick >= 0) {
                    at = line_end[pick];
                    uint8_t restore = set_at(at);
                    text.insert(text.begin() + static_cast<long>(at), restore);
                    text.insert(text.begin() + static_cast<long>(at), tag, tag + sizeof tag);
                }
                else if (line_end.size() < 3) {
                    // A line of its own, without the leading space.
                    at = text.size();
                    text.push_back(0xE0);
                    text.insert(text.end(), tag + 1, tag + sizeof tag);
                    at += 1;
                }
                else {
                    // Every line is full: the shortest one takes it.
                    int shortest = 0;
                    for (int l = 1; l < static_cast<int>(line_width.size()); l++) {
                        if (line_width[l] < line_width[shortest]) shortest = l;
                    }
                    at = line_end[shortest];
                    uint8_t restore = set_at(at);
                    text.insert(text.begin() + static_cast<long>(at), restore);
                    text.insert(text.begin() + static_cast<long>(at), tag, tag + sizeof tag);
                }
                text.push_back(0xFF);
                int32_t out = counted_text[id];
                for (size_t i = 0; i < text.size(); i++) {
                    MEM_B(static_cast<int32_t>(i), out) = static_cast<int8_t>(text[i]);
                }
                // The two digits are the tag's last two bytes; `at` is where
                // the tag starts (the line-of-its-own tag has no space).
                size_t tag_len = (pick < 0 && line_end.size() < 3) ? sizeof tag - 1 : sizeof tag;
                digits_at[id] = out + static_cast<int32_t>(at + tag_len - 2);
                MEM_W(0, entry) = out;
            }
            int count = std::min(counts[id], max_stack);
            MEM_B(0, digits_at[id]) = static_cast<int8_t>(count / 10);
            MEM_B(1, digits_at[id]) = static_cast<int8_t>(count % 10);
        }
    }
}

bool zelda64::enhancements::item_menu_open(uint8_t* rdram) {
    return (static_cast<uint32_t>(MEM_W(0, item_menu_mask)) & item_menu_bit) != 0 ||
           MEM_HU(0, game_mode) == 2;
}

void zelda64::enhancements::bag_counts(uint8_t* rdram, int (&counts)[256]) {
    if (!read_bag(rdram, counts, nullptr)) {
        // Something neither layout would hold: count plain ids and nothing else.
        for (int& c : counts) c = 0;
        for (int slot = 0; slot < bag_slots; slot++) {
            int b = MEM_BU(slot, bag);
            if (b != bag_empty && b < 0x80) {
                counts[b]++;
            }
        }
    }
}

void zelda64::enhancements::on_frame(uint8_t* rdram) {
    const Options& options = active_options();

    if (!zelda64::hardmode::active() && in_game(rdram)) {
        if (options.stack_items) {
            stack_bag(rdram, item_menu_open(rdram));
            show_counts(rdram);
        }
        else {
            unstack_bag(rdram);
        }
    }
    else if (static_cast<int32_t>(MEM_W(0, next_map)) == -1) {
        // No save loaded (the title, or a reset): the next bag is a save's
        // own, read afresh. Not in modes 2 and 4, where a battle's item
        // uses still have to be caught up with afterwards.
        have_last = false;
    }

    // The element-choice screen (bit 3 of the menu mask 0x8007B2E4) opened
    // with every element already at the cap - a natural level-up, a spirit
    // taken at the cap, or an Archipelago Level Up - has nothing to raise
    // and no way out. It is closed the frame it appears.
    {
        constexpr int32_t menu_mask = 0x8007B2E4;
        constexpr uint32_t menu_spirit = 0x8;
        uint32_t mask = static_cast<uint32_t>(MEM_W(0, menu_mask));
        if ((mask & menu_spirit) && elements_all_maxed(rdram)) {
            MEM_W(0, menu_mask) = static_cast<int32_t>(mask & ~menu_spirit);
        }
    }

    if (options.longer_magic_barrier) {
        // The counter only rises when the spell is cast and falls as the
        // barrier is used, so acting on a rise applies this once per cast
        // rather than every frame.
        //
        // Each cast is extended by two and then held inside 3-6, so the
        // stock top roll of 4 (three absorbed attacks) becomes 6 (five) and
        // nothing can come out shorter than 3 or longer than 6.
        static int previous = 0;
        int current = MEM_BU(0, magic_barrier_timer);
        if (current > previous && current > 0) {
            int extended = std::clamp(current + magic_barrier_bonus,
                magic_barrier_min, magic_barrier_max);
            MEM_B(0, magic_barrier_timer) = static_cast<int8_t>(extended);
            current = extended;
        }
        previous = current;
    }

    if (!options.one_hit_ko) {
        return;
    }
    // Monsters are handled in the ROM, but Brian's HP lives in the save, so a
    // game already in progress needs it held down here. Only ever lowered, so
    // a death in progress is never undone.
    if (MEM_HU(0, player_max_hp) != 1) {
        MEM_H(0, player_max_hp) = 1;
    }
    if (MEM_HU(0, player_hp) > 1) {
        MEM_H(0, player_hp) = 1;
    }
}

void zelda64::enhancements::cast_exit() {
    if (!active_options().exit_from_anywhere) {
        return;
    }
    // What the Exit spell does: drop the player back at the start of the area
    // they are in. Goes through the same queued warp the cheats menu uses, so
    // the game runs its own fade and spawn, and it waits for a safe moment.
    // Passing from_cheats = false keeps it working with cheats turned off.
    int map = zelda64::current_map();
    if (map < 0 || map >= zelda64::map_count()) {
        return;
    }
    // drop_if_busy: pressed in a battle, a menu or a transition this does
    // nothing at all. Queuing it instead would fire the warp the moment the
    // battle ended, which is worse than ignoring the press.
    zelda64::do_map_warp(map, 0, 0, false, true);
}

// Faster walking (Options::faster_walk). The movement handlers leave the
// frame's velocity in the player struct at +0x18 (x) and +0x20 (z), and
// func_80005748 then moves Brian by it, resolving collisions against
// position + velocity. The velocity is scaled on entry to that call and
// restored on its return, so the game's own wall test sees the longer step
// while the handlers read back next frame exactly what they wrote. (Leaving
// it scaled fed the scaled value into the walk handler's speed lerp, which
// ran away to the cap, and into the skid's 0.68-a-frame decay, which then
// barely decayed at all: that was the long slide after letting go of the
// stick.) Every state moves through this call - the field walk
// (func_8000534C), battle movement (func_80004E58) and the eight-frame skid
// after the stick is released (func_80003F98, D_80070F50) - so the slide
// keeps its vanilla frame count and covers 1.5x the distance, the same
// shape as Hard Mode's 2.75 target speed gives. Hard Mode keeps its own
// pace: the scale is not stacked on top of it.
namespace {
    constexpr float walk_speed_scale = 1.5f;
    // Walls are about 3.5 units thick and the collision test is on position +
    // velocity rather than swept, so a single step must stay under that or
    // Brian ends up on the far side. Vanilla walks 2 units a frame, so the
    // 1.5x step sits exactly on the cap.
    constexpr float max_step_units = 3.0f;

    // Left by the entry hook for the exit hook: the struct that was scaled
    // and the factor actually applied (smaller than walk_speed_scale when
    // the step cap bit). Both hooks run on the game thread.
    int32_t scaled_player = 0;
    float applied_scale = 1.0f;

    float read_f32(uint8_t* rdram, int32_t addr) {
        int32_t bits = MEM_W(0, addr);
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    void write_f32(uint8_t* rdram, int32_t addr, float value) {
        int32_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        MEM_W(0, addr) = bits;
    }
}

extern "C" void quest64_enh_walk_scale(uint8_t* rdram, recomp_context* ctx) {
    applied_scale = 1.0f;
    if (!zelda64::enhancements::active_options().faster_walk || zelda64::hardmode::active()) {
        return;
    }
    int32_t player = static_cast<int32_t>(ctx->r5);
    float vx = read_f32(rdram, player + 0x18);
    float vz = read_f32(rdram, player + 0x20);
    float scale = walk_speed_scale;
    float step = std::sqrt(vx * vx + vz * vz) * scale;
    if (step > max_step_units) {
        scale *= max_step_units / step;
    }
    write_f32(rdram, player + 0x18, vx * scale);
    write_f32(rdram, player + 0x20, vz * scale);
    scaled_player = player;
    applied_scale = scale;
}

extern "C" void quest64_enh_walk_unscale(uint8_t* rdram, recomp_context*) {
    if (applied_scale == 1.0f) {
        return;
    }
    // func_80005748 may have zeroed or projected the velocity on a wall;
    // dividing keeps whatever it decided in the game's own units.
    write_f32(rdram, scaled_player + 0x18, read_f32(rdram, scaled_player + 0x18) / applied_scale);
    write_f32(rdram, scaled_player + 0x20, read_f32(rdram, scaled_player + 0x20) / applied_scale);
    applied_scale = 1.0f;
}

// ---- Boss max MP ----------------------------------------------------------
//
// func_8000BB68 is the boss reward. For every boss but Mammon (a0 = 7) it adds
// the halfword at 0x8004C2C0 + 2 * boss to max HP (capped at 500) and refills
// HP; both paths leave through 0x8000BCF4, where this hook runs. With the
// option on, max MP rises too and MP is refilled, the way the Japanese
// release does it: Eltale Monsters' copy of this routine (ROM 0xE128, RAM
// 0x8000D528) reads a second table straight after the HP one and treats it
// exactly like HP - add, cap at 500, refill. That table is copied here, since
// the US ROM has only zeroes after its HP table. a1 still holds the boss's
// number at the hook. Every call is logged to boss_mp.txt.
namespace {
    // Eltale Monsters, 0x8004DD60: Solvaring, Zelse, Nepty, Shilf, Fargo,
    // Guilty, Beigis.
    constexpr int jp_boss_mp_bonus[7] = { 5, 5, 5, 10, 10, 15, 15 };
}

extern "C" void quest64_enh_boss_max_mp(uint8_t* rdram, recomp_context* ctx) {
    constexpr int32_t player_main = 0x8007BA80;   // +6 max HP, +8 MP, +0xA max MP
    constexpr int max_mp_cap = 500;
    int boss = static_cast<int>(ctx->r5);
    bool on = zelda64::enhancements::active_options().boss_max_mp && !zelda64::hardmode::active();
    int before = MEM_HU(0xA, player_main);
    if (on && boss >= 0 && boss < 7) {
        int max_mp = std::min(before + jp_boss_mp_bonus[boss], max_mp_cap);
        MEM_H(0xA, player_main) = static_cast<int16_t>(max_mp);
        MEM_H(0x8, player_main) = static_cast<int16_t>(max_mp);
    }
    std::ofstream out(zelda64::get_app_folder_path() / "boss_mp.txt", std::ios::app);
    out << "boss " << boss << ": option " << (on ? "on" : "off") << ", max HP " << MEM_HU(0x6, player_main)
        << ", max MP " << before << " -> " << MEM_HU(0xA, player_main) << "\n";
}

// ---- Hide Compass --------------------------------------------------------
//
// func_8001E25C draws the field HUD: the HP/MP block, the spirits, and - in
// the field, not in a battle - the compass, func_8001EA84. That routine puts
// it at (260, 24) (the HUD's shared position slot, 0x8008C648/C64C, which
// every HUD element sets for itself before drawing), turns the disc
// (display list 0x803A8EA0) by the camera's angle and draws the arrow over it
// through func_800210FC. Hooked at its entry, returning skips all of it.
namespace {
    std::atomic<bool> compass_hidden{ false };
}

void zelda64::enhancements::set_hide_compass(bool hide) {
    compass_hidden.store(hide);
}

extern "C" int quest64_enh_hide_compass(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    (void)ctx;
    return compass_hidden.load() ? 1 : 0;
}
