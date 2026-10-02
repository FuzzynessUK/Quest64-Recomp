#ifndef __ARCHIPELAGO_CLIENT_H__
#define __ARCHIPELAGO_CLIENT_H__

// The connection to an Archipelago server, through APCpp
// (https://github.com/N00byKing/APCpp). Nothing else in the game talks to
// APCpp or the network: src/game/archipelago.cpp drives this from its worker
// thread and keeps all of the game's own rules.
//
// Threads: APCpp runs its own network thread and calls back on it. Those
// callbacks only put events into a locked queue here, and the worker takes
// them out with poll_event(). APCpp's own message queue has no lock, so it is
// only ever read from inside those callbacks, on APCpp's thread, and copied
// into a locked queue of our own for poll_message().

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace q64ap {
    enum class Status { Idle, Disconnected, Connected, Authenticated, Refused };

    struct ConnectInfo {
        std::string address;    // host:port; a ws:// or wss:// prefix is dropped
        std::string slot;
        std::string password;   // may be empty; never logged
        // The slot_data keys to hand back as SlotData events.
        std::vector<std::string> slot_data_keys;
    };

    enum class EventType {
        ResetItems,         // the server is about to send the whole item list again
        ItemReceived,       // item `id` is at `index` in the server's list
        LocationChecked,    // the server says location `id` is checked
        SlotData,           // slot_data[key] = value, as JSON text
        DeathReceived,      // a DeathLink death: key = who, value = cause (may be empty)
    };

    struct Event {
        EventType type = EventType::ResetItems;
        int64_t id = 0;
        uint32_t index = 0;     // ItemReceived: counted from the last ResetItems
        bool notify = false;    // ItemReceived: new to this slot, worth a notice
        std::string key;        // SlotData
        std::string value;      // SlotData
    };

    struct Message {
        bool item_send = false;     // someone's find going to someone other than us
        std::string text;
        std::string item;           // item_send: what, named in its own game
        std::string sender;         // item_send: who found it
        std::string receiver;       // item_send: who it is for
    };

    // Where APCpp's own log lines go. Set before connect().
    void set_log(std::function<void(const std::string&)> log);

    bool connect(const ConnectInfo& info);
    void disconnect();
    Status status();
    int our_slot();

    void send_locations(const std::vector<int64_t>& location_ids);
    void send_goal_complete();
    // DeathLink: tell the room Brian died. APCpp sends nothing unless the
    // slot has DeathLink on (slot_data death_link).
    void send_death(const std::string& cause);

    bool poll_event(Event& out);
    bool poll_message(Message& out);
}

#endif
