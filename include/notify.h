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
    // What a line is about. A kind lets the overlay treat repeats of the
    // same thing as one line - a new song name taking the place of the one
    // showing rather than stacking under it - where the setting for that
    // kind asks for it. Anything left unlabelled always stacks.
    enum class Kind {
        Any,
        Song,
        // Archipelago traffic. Posted whatever the setting says and dropped
        // by the overlay instead, so the Notifications setting takes effect
        // as it is changed rather than on the next launch.
        ApReceived,   // an item this slot was sent
        ApSent,       // an item found here that belongs to someone else
        ApRoom,       // an item passing between two other players
    };

    struct Message {
        std::string text;
        Kind kind = Kind::Any;
    };

    // Game thread (or any thread): queue a line for the overlay.
    void post(std::string text, Kind kind = Kind::Any);

    // UI thread: the lines posted since the last call, oldest first.
    std::vector<Message> take();
}

#endif
