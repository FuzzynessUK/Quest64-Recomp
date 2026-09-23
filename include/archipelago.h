#ifndef __ARCHIPELAGO_H__
#define __ARCHIPELAGO_H__

#include <cstdint>
#include <string>
#include <vector>

// The Archipelago connector: the port talks to an Archipelago server itself,
// over the same WebSocket protocol any other client uses, so there is no
// separate Quest 64 client to run. The world that decides what is where is
// tools/archipelago_world.pl's quest64.apworld.
//
// Everything here is safe to call from the game thread. The socket lives on
// a worker thread of its own and the two sides pass work through queues, so
// nothing the game does can block on the network.
namespace zelda64::archipelago {
    struct Options {
        bool enabled = false;
        // host:port. Archipelago's default port is 38281.
        std::string server = "localhost:38281";
        std::string slot;
        std::string password;
    };

    Options load_options();
    void save_options(const Options& options);
    // The copy the worker is using.
    const Options& active_options();
    // Starts, stops or reconnects the worker to match `options`, and saves
    // them. Safe to call from the UI thread on every edit.
    void apply_options(const Options& options);

    enum class Status {
        Off,
        Connecting,
        Connected,
        Failed,
    };

    Status status();
    // One line for the menu: what it is doing, or why it stopped.
    std::string status_line();
    // The name of the slot the server accepted us as, once connected.
    std::string connected_slot();

    // --- location and item ids ---------------------------------------------
    // The same scheme tools/archipelago_world.pl assigns, which is what makes
    // a location the game reports the one the server is holding. 0x5164 is
    // "Q64"; the next nibble separates the groups so one can grow without
    // disturbing another. Never renumber these: an id has to mean the same
    // thing forever or old seeds stop matching.
    constexpr int64_t id_base = 0x51640000;
    constexpr int64_t group_item = 0x0000;
    constexpr int64_t group_chest = 0x1000;
    constexpr int64_t group_giver = 0x2000;
    constexpr int64_t group_enemy = 0x3000;
    constexpr int64_t group_boss = 0x4000;
    constexpr int64_t group_spirit = 0x5000;
    // What a spirit used to hand over: the element-choice screen.
    constexpr int64_t item_level_up = id_base + group_item + 0xF00;
    // One per boss, + his number 1-8. Archipelago's own item: the game has
    // no such thing and never sees one. Until it arrives that boss is not
    // spawned at all.
    constexpr int64_t item_boss_soul = id_base + group_item + 0xE00;

    // --- the game side -----------------------------------------------------
    // Boot, after the randomizer: with the connector on, the chests' own
    // contents are taken out of the ROM. What a chest holds is the server's
    // business now, and it hands it over itself.
    void apply_at_boot(uint8_t* rdram);

    // Called once a frame from the cheats frame hook, on the game thread.
    // Notices the checks the game has reached and applies what the server
    // has sent. Does nothing at all unless the connector is on.
    void on_frame(uint8_t* rdram);

    // Level Ups the server has sent that the game has not opened the
    // element-choice screen for yet.
    int pending_level_ups();

    // Phase 3 calls these; they are no-ops until the connector is on.

    // Tell the server a location has been checked. Sending one twice is
    // harmless - the server ignores a repeat - so the caller does not have to
    // be careful about it.
    void send_check(int64_t location);

    // A save remembers how many of the server's items it has already been
    // given. Archipelago re-sends the whole list on every connect, so
    // without this a reload would hand everything over a second time - the
    // Level Ups especially, which are spent the moment they arrive.
    //
    // The key is the one the run timer works out for the save being written
    // or read (a hash of the bytes themselves); both are called from its
    // Controller Pak hooks, which are the only place that knows a save is
    // going past.
    void save_progress(const std::string& save_key);
    void load_progress(const std::string& save_key);

    // Tell the server the goal is done.
    void goal_reached();

    // Called once as the program exits.
    void shutdown();
}

#endif
