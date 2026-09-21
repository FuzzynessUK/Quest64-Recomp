#ifndef __ITEMNOTICE_H__
#define __ITEMNOTICE_H__

#include <cstdint>

// "Show item received" (Options::item_notice): posts "Received <item>" to
// the notification stack (include/notify.h) whenever an item is added to
// Brian's bag - chests, gifts, drops, shops, the cheats tab alike.
namespace zelda64::itemnotice {
    // Game thread, once per frame from the cheats frame hook.
    void on_frame(uint8_t* rdram);
}

#endif
