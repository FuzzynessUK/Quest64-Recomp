#ifndef __NOTIFY_H__
#define __NOTIFY_H__

#include <string>
#include <vector>

// On-screen notifications, Ship of Harkinian style: a stack in the top-left
// corner where new messages appear under the ones already showing, each
// held for a few seconds and faded on its own, the rest closing up as older
// ones go. Any feature on the game thread posts a line; the UI thread draws
// them (recompui::update_notifications, assets/notifications.rml).
namespace zelda64::notify {
    // Game thread (or any thread): queue a line for the overlay.
    void post(std::string text);

    // UI thread: the lines posted since the last call, oldest first.
    std::vector<std::string> take();
}

#endif
