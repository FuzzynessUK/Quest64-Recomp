#include <algorithm>
#include <array>
#include <atomic>
#include <cstring>
#include <span>
#include <string>
#include <vector>

#include "pageitem.h"
#include "hardmode.h"
#include "archipelago.h"
#include "notify.h"
#include "enhancements.h"
#include "zelda_debug.h"
#include "librecomp/game.hpp"
#include "librecomp/addresses.hpp"
#include "recomp.h"

namespace {
    constexpr uint32_t icon_rom = 0xD3C240 + zelda64::page_item::item_id * 0x100;
    constexpr int32_t name_table = 0x803A9954;    // u32 pointer per item, 32 slots
    constexpr int32_t desc_table = 0x803A99D4;
    // Item 25's name pointer as the game loads it: the tables are only
    // touched once they are in RAM.
    constexpr int32_t dark_gaol_key_name = 0x803A94C4;

    // Drawn from the approved 32x32 icon at the game's 16x16 and matched to
    // the item palette (ROM 0xD3BE40; index 0 is transparent). The second,
    // grey palette gives the greyed-out look by itself.
    constexpr std::array<uint8_t, 0x100> icon = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0x22, 0x22, 0x22, 0x57, 0x22, 0x22, 0x22, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0xD9, 0x13, 0xCE, 0x22, 0x22, 0x37, 0x22, 0xDC, 0x22, 0xCE, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x1F, 0xD2, 0x1F, 0x1F, 0x1F, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0xBB, 0xBB, 0xBB, 0x1F, 0xBB, 0xBB, 0xBB, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0xBB, 0xBB, 0x1F, 0xBB, 0xBB, 0xBB, 0xBB, 0x63, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x36, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0xBB, 0xBB, 0xBB, 0xBB, 0x1F, 0xBB, 0x36, 0x36, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x36, 0xD9, 0xD9, 0xD9, 0xD9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0x1F, 0x1F, 0x36, 0x36, 0x36, 0xD9, 0x8C, 0xD9, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0x13, 0x36, 0x36, 0x36, 0x36, 0x36, 0xD9, 0xD9, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    // The game's text: 0x00-0x19 a-z, 0x7F space, 0x81 capitals on, 0x82
    // capitals off, 0x1A apostrophe, 0xE0 new line, 0xFF end, and 0x80 for
    // numbers, whose digits are then 0x00-0x09 ("HP 50 RECOVERY" is
    // 81 07 0F 7F 80 05 00 7F 81 ...). Every item name and description
    // opens with A0 C0.
    std::vector<uint8_t> encode(const std::string& text) {
        std::vector<uint8_t> out = { 0xA0, 0xC0, 0x81 };
        bool digits = false;
        for (char c : text) {
            if (c >= 'a' && c <= 'z') {
                if (digits) { out.push_back(0x81); digits = false; }
                out.push_back(static_cast<uint8_t>(c - 'a'));
            }
            else if (c >= '0' && c <= '9') {
                if (!digits) { out.push_back(0x80); digits = true; }
                out.push_back(static_cast<uint8_t>(c - '0'));
            }
            else if (c == ' ') out.push_back(0x7F);
            else if (c == '\'') { out.push_back(0x82); out.push_back(0x1A); out.push_back(0x81); digits = false; }
            else if (c == '\n') { out.push_back(0xE0); out.push_back(0x81); digits = false; }
        }
        out.push_back(0xFF);
        return out;
    }

    int32_t name_addr = 0;
    int32_t desc_addr = 0;
    constexpr size_t desc_room = 64;   // the description is rewritten in place as pages come in

    constexpr int32_t gNextMap = 0x80084EE4;     // -1 until a save is loaded

    std::atomic<int> seed_target{ 0 };
    int baseline = -1;                            // pages in the bag as this save came in
    int last_count = -1;
    int shown_count = -2;
    int shown_target = -2;

    // Stacked or not: Stack Items keeps one entry and a count (enhancements.cpp).
    int pages_in_bag(uint8_t* rdram) {
        return zelda64::enhancements::bag_count(rdram, zelda64::page_item::item_id);
    }

    int target() {
        return seed_target.load();
    }

    void write_desc(uint8_t* rdram, const std::vector<uint8_t>& bytes) {
        size_t n = std::min(bytes.size(), desc_room);
        for (size_t i = 0; i < n; i++) {
            MEM_B(static_cast<int32_t>(i), desc_addr) = static_cast<int8_t>(bytes[i]);
        }
        MEM_B(static_cast<int32_t>(desc_room - 1), desc_addr) = static_cast<int8_t>(0xFF);
    }

    int32_t place(uint8_t* rdram, const std::vector<uint8_t>& bytes) {
        void* mem = recomp::alloc(rdram, std::max(bytes.size(), desc_room));
        if (mem == nullptr) {
            return 0;
        }
        int32_t addr = static_cast<int32_t>(static_cast<uint32_t>(reinterpret_cast<uint8_t*>(mem) - rdram) + 0x80000000u);
        for (size_t i = 0; i < bytes.size(); i++) {
            MEM_B(static_cast<int32_t>(i), addr) = static_cast<int8_t>(bytes[i]);
        }
        return addr;
    }
}

void zelda64::page_item::set_hunt(int required) {
    seed_target.store(required > 0 ? required : 0);
}

bool zelda64::page_item::available() {
    return !zelda64::hardmode::active();
}

void zelda64::page_item::apply_at_boot() {
    if (!available()) {
        return;
    }
    // Written into the ROM image in place: the menu reads each icon from it
    // with a DMA every time it shows one, so nothing has to be in RAM yet.
    std::span<const uint8_t> rom = recomp::get_rom();
    if (rom.size() >= icon_rom + icon.size()) {
        std::memcpy(const_cast<uint8_t*>(rom.data()) + icon_rom, icon.data(), icon.size());
    }
}

void zelda64::page_item::on_frame(uint8_t* rdram) {
    if (!available() || MEM_W(25 * 4, name_table) != dark_gaol_key_name) {
        return;
    }
    if (name_addr == 0) {
        // librecomp's heap is up by the time the game runs a frame.
        name_addr = place(rdram, encode("torn page"));
        desc_addr = place(rdram, encode("a page torn from\neletale's book"));
    }
    if (name_addr != 0 && MEM_W(item_id * 4, name_table) != name_addr) {
        MEM_W(item_id * 4, name_table) = name_addr;
        MEM_W(item_id * 4, desc_table) = desc_addr;
    }

    // The Page Hunt.
    int goal = target();
    if (MEM_W(0, gNextMap) == -1) {
        baseline = -1;   // title or file select: the next save starts afresh
        return;
    }
    int count = pages_in_bag(rdram);
    if (desc_addr != 0 && (count != shown_count || goal != shown_target)) {
        shown_count = count;
        shown_target = goal;
        write_desc(rdram, goal > 0 ? encode("a lost book page\nfound " + std::to_string(count) + " of " + std::to_string(goal))
                                   : encode("a page torn from\neletale's book"));
    }
    if (goal <= 0) {
        return;
    }
    if (baseline < 0) {
        // The bag as the save came in: a save already past the target does
        // not play the credits again.
        baseline = count;
        last_count = count;
        return;
    }
    if (count >= goal && last_count < goal) {
        zelda64::notify::post("All " + std::to_string(goal) + " pages found!");
        zelda64::request_ending();
        if (seed_target.load() > 0) {
            zelda64::archipelago::goal_reached();
        }
    }
    last_count = count;
}

// func_800212E4 entry: "can this item be used here?". The page's slot in the
// properties table reads a name string, which says usable with a handler
// that does not exist, so the page answers no like the other key items.
extern "C" int quest64_page_item_unusable(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    if (!zelda64::page_item::available() || (ctx->r4 & 0xFF) != zelda64::page_item::item_id) {
        return 0;
    }
    ctx->r2 = 0;
    return 1;
}
