#ifndef __PAGEITEM_H__
#define __PAGEITEM_H__

#include <cstdint>
#include <string>
#include <vector>

// The Torn Page, item 0x1A: a page ripped out of the Eletale's Book, for the
// Page Hunt. The game keeps room for 32 items and uses 26; the spare slots
// already exist in every table an item id indexes, so the page only needs
// filling in:
//   - icon: 16x16 CI8 on the item palette, slot 26 of the icon block at ROM
//     0xD3C240 (0x100 bytes an item; the item menu DMAs each one as it is
//     shown), written at boot.
//   - name and description: pointer slot 26 of the tables at RAM 0x803A9954
//     and 0x803A99D4 (loaded from ROM 0xD77380 by func_80000FE8), pointed at
//     strings in librecomp's heap.
//   - use: the properties record (RAM 0x803A91F0, 12 bytes an item) ends at
//     item 24, so slot 26 reads a name string as "usable, handler 0x13FF".
//     The usable check, func_800212E4, answers no for it instead.
// Hard Mode fills slots 0x1A-0x1F with items of its own, so none of this
// happens under Hard Mode.
namespace zelda64::page_item {
    constexpr int item_id = 0x1A;
    constexpr const char* name = "Torn Page";

    // Whether this launch has the page (not under Hard Mode).
    bool available();

    // Boot, after every other ROM patch: the icon.
    void apply_at_boot();

    // Game thread, every frame: the name and description pointers, and the
    // Page Hunt: pages counted in the bag, the description showing how many,
    // and the credits once the target is reached. Runs before the Archipelago
    // hand-over in the frame, so a page delivered on loading still counts as
    // the moment it was found.
    void on_frame(uint8_t* rdram);

    // The pages the seed wants (slot_data pages_required), 0 for none.
    // ends_run: the Page Hunt goal, whose last page rolls the credits;
    // otherwise the pages open Mammon's World (mammon_portal bit 4), which
    // archipelago.cpp checks, and reaching them is only announced.
    void set_hunt(int required, bool ends_run);

    // Text in the game's encoding for an item name or description (A0 C0
    // header, FF end); '\n' is a new line. Also used by repel.cpp.
    std::vector<uint8_t> encode_text(const std::string& text);
}

#endif
