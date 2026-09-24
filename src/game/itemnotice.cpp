// "Show item received" (Options::item_notice). gInventory (0x8008CF78) is a
// flat list of 150 item ids, 0xFF for an empty slot, and the game compacts
// it when something is used, so slots move; what is stable is how many of
// each id there are. Each frame the counts are compared with the last
// frame's and every increase posts "Received <item>" to the notification
// stack. The baseline is reset on the title and file select so loading a
// save never reads as receiving everything in the bag.
#include <array>
#include <string>

#include "itemnotice.h"
#include "enhancements.h"
#include "notify.h"
#include "randomizer/merrow_data.h"
#include "recomp.h"

namespace data = merrow::data;

namespace {
    constexpr int32_t gInventory = 0x8008CF78;
    constexpr int inventory_slots = 150;
    constexpr int inventory_empty = 0xFF;
    constexpr int item_ids = 256;

    constexpr int32_t gGameMode = 0x8007B2E0;
    constexpr int32_t gNextMap = 0x80084EE4;

    std::array<int, item_ids> previous{};
    bool primed = false;

    // "FRESH BREAD" -> "Fresh Bread"; ids the table does not know (Hard
    // Mode's specials) fall back to their number.
    std::string item_name(int id) {
        size_t index = static_cast<size_t>(id) * 3;
        if (index >= data::items.size()) {
            return "item " + std::to_string(id);
        }
        std::string name = data::items[index];
        bool start = true;
        for (char& c : name) {
            if (c == '`') c = '\'';
            if (start && c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
            else if (!start && c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
            start = (c == ' ');
        }
        return name;
    }
}

void zelda64::itemnotice::on_frame(uint8_t* rdram) {
    if (!zelda64::enhancements::active_options().item_notice) {
        return;
    }
    int mode = MEM_HU(0, gGameMode);
    bool in_game = mode != 2 && mode != 4 && static_cast<int32_t>(MEM_W(0, gNextMap)) != -1;
    if (!in_game) {
        primed = false;
        return;
    }

    // A stacked bag (Stack items) holds its extra copies as counts rather
    // than slots, so the bag is counted through the enhancements, which read
    // either layout.
    int counted[256];
    zelda64::enhancements::bag_counts(rdram, counted);
    std::array<int, item_ids> current{};
    for (int id = 0; id < item_ids; id++) {
        current[id] = counted[id];
    }
    if (primed) {
        for (int id = 0; id < item_ids; id++) {
            for (int gained = current[id] - previous[id]; gained > 0; gained--) {
                zelda64::notify::post("Received " + item_name(id));
            }
        }
    }
    previous = current;
    primed = true;
}
