#ifndef __REPEL_H__
#define __REPEL_H__

#include <array>
#include <cstdint>
#include <string>

// Repel, item 0x1B: a key item that switches random encounters off and on,
// as Pokemon's does. Enhancements > Quality of Life "Repel" (and the yaml's
// `repel`) puts one in the bag whenever a game is being played without one.
//
// The item fills a spare slot the way the Torn Page does (pageitem.h): icon
// 27 of the block at ROM 0xD3C240, name and description pointer 27 of the
// tables at 0x803A9954 / 0x803A99D4. Its properties record does not exist
// (the table at 0x803A91F0 ends at item 24; slot 27 reads text), so every
// place the game would read it is answered here instead:
//   - func_800212E4, "use this item": in the field Brian takes up the
//     item-use pose (func_8000669C) and the menu closes; in battle it cannot
//     be used.
//   - func_80021524 takes a used item out of the bag; Repel is put straight
//     back in the same place.
//   - func_800213D8, the use taking effect as the pose ends: Repel switches
//     on or off, and Brian casts Spirit Armor (the Silver Amulet's spell)
//     from a copy of its record whose status parameters are zeroed, so it is
//     the look and the sound and no defence.
// While it is on, the field encounter check (func_8001C5F4) is skipped, the
// same way the Disable encounters cheat does it, and a 16x16 icon sits over
// Brian's head: in battle after the game's own status icons, by the routine
// that draws them (func_8001FEEC, which only runs in battle), and in the
// field from the HUD's field branch, done the same way.
//
// Whether it is on is kept per save, filed against the save's bytes when the
// game writes one (the run timer's Controller Pak hooks), in
// repel_saves.json. A new game starts with it off.
//
// Hard Mode fills 0x1A-0x1F with items of its own, so none of this happens
// under Hard Mode.
namespace zelda64::repel {
    constexpr int item_id = 0x1B;
    constexpr const char* name = "Repel";

    // Whether this launch has the item at all (not under Hard Mode).
    bool available();
    // Whether encounters are being kept away right now.
    bool active();

    // Boot, after every other ROM patch: the bag icon.
    void apply_at_boot();
    // Game thread, every frame: the name and description, handing the item
    // over when the option wants it, and keeping the status block straight
    // after a cast.
    void on_frame(uint8_t* rdram);

    // The run timer's save hooks: file the state against a save written, and
    // take it back for a save loaded.
    void save_progress(const std::string& save_key);
    void load_progress(const std::string& save_key);

    // GENERATED (tools/repel/repel_icon.pl): CI8, index 0 transparent.
    extern const std::array<uint8_t, 0x100> bag_icon;    // item palette
    extern const std::array<uint8_t, 0x100> head_icon;   // status-icon palette
}

#endif
