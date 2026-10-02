#include <atomic>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "traps.h"
#include "archipelago.h"
#include "enhancements.h"
#include "notify.h"
#include "zelda_config.h"
#include "librecomp/game.hpp"
#include "recomp.h"

extern "C" void func_8001FCF8(uint8_t* rdram, recomp_context* ctx);

namespace {
    constexpr int32_t gPlayerMainData = 0x8007BA80;   // +4 HP, +6 max HP, +8 MP, +0xA max MP
    constexpr int32_t player_state = 0x8007BAB8;      // u16: 0-2 standing, walking, battle movement
    constexpr int32_t game_mode_addr = 0x8007B2E0;    // gGameMode, u16: 1 field, 3 battle
    constexpr int32_t gNextMap = 0x80084EE4;          // -1 until a save is loaded
    constexpr int32_t menu_mask = 0x8007B2E4;
    constexpr uint32_t moving_or_menu = 0xC0 | 0x8;   // door/transition bits, spirit screen
    constexpr auto ice_time = std::chrono::seconds(5);

    // The ice block the game puts on Brian when Ice Knife freezes him, shown
    // without the freeze. An Ice Knife hit runs func_8001715C: status bit 0x8
    // in the status block Brian's object points at (+0x68), then
    // func_8001FCF8(actor, 8, effect - 1, -1), which spawns the block's effect object
    // and keeps it in the block's visual slot (+0x44). The per-frame updater
    // (func_8001FEEC) ends the effect as soon as bit 0x8 is gone. So the trap
    // calls func_8001FCF8 alone and takes the effect straight back out of the
    // slot: the game never sees a frozen Brian (no lost turns, no status
    // icon), the updater leaves the effect alone, and the trap ends it after
    // five seconds the way the updater would (clear bit 0 of +8, state 8).
    // Brian cannot move meanwhile, so it never needs moving with him. A door
    // (or map load) takes it off and on_frame puts it back afterwards.
    constexpr int32_t player_object = 0x8007BACC;    // +0x68: status block
    constexpr int32_t gBattleState = 0x8008C592;     // bit 0: a battle is running
    constexpr int32_t gCurrentMap = 0x80084EEC;
    constexpr uint16_t freeze_bit = 0x8;
    // The visual is the spell's effect value (+0x35 of its entry) minus one:
    // Restriction's 1 gives 0, the binding rings; Ice Knife's 2 gives 1, the ice.
    constexpr int ice_visual = 1;
    int32_t ice_effect = 0;
    int ice_battle = 0;
    int32_t ice_map = 0;

    // Waiting traps, one count each. Death is not counted here: it goes to
    // the DeathLink machinery, which has its own wait.
    std::atomic<int> waiting_ice{ 0 };
    std::atomic<int> waiting_mp{ 0 };
    std::atomic<int> waiting_hp{ 0 };

    bool is_frozen = false;
    // Read by the controller callback (another thread): while an Ice Trap
    // holds Brian, the game is not shown A, B, Z or the stick.
    std::atomic<bool> buttons_locked{ false };
    std::chrono::steady_clock::time_point thaw_at;
    // Doors and map loads: the freeze lets go when one starts and starts
    // over, five full seconds, once Brian has stood in the new room for
    // resume_after. hold_allowed is false outside plain play (worked out
    // each frame), so the movement hook never pins a scripted walk.
    constexpr int32_t game_state = 0x8007B2E4;        // gGameState, the same word as menu_mask
    constexpr uint32_t transition_bits = 0xE0;         // 0x80 leaving, 0x40 loading, 0x20 just loaded
    constexpr int32_t map_word = 0x80084EE4;          // map and submap loaded (as the minimap reads them)
    constexpr int32_t submap_word = 0x80084EE8;
    constexpr auto resume_after = std::chrono::milliseconds(800);
    bool ice_suspended = false;
    bool hold_allowed = false;
    int32_t ice_room_map = -1;
    int32_t ice_room_submap = -1;
    std::chrono::steady_clock::time_point settled_since{};
    // When a door or map load was last seen: no trap lands until
    // resume_after has passed since, so none arrives mid-door.
    std::chrono::steady_clock::time_point last_transition{};
    int32_t last_map = -1;
    int32_t last_submap = -1;

    void log_line(const std::string& line) {
        std::ofstream out(zelda64::get_app_folder_path() / "traps.txt", std::ios::app);
        out << line << "\n";
    }

    // Where a trap can land: a save loaded, in the field or a battle, no menu
    // or door, and Brian in a plain state (never mid-hit or mid-cast).
    bool ready(uint8_t* rdram) {
        uint16_t mode = MEM_HU(0, game_mode_addr);
        return MEM_W(0, gNextMap) != -1 && (mode == 1 || mode == 3) &&
               (static_cast<uint32_t>(MEM_W(0, menu_mask)) & moving_or_menu) == 0 &&
               MEM_HU(0, player_state) <= 2 && MEM_HU(4, gPlayerMainData) != 0 &&
               !zelda64::enhancements::item_menu_open(rdram);
    }

    // Takes one from a count, if there is one.
    bool take(std::atomic<int>& count) {
        int n = count.load();
        while (n > 0) {
            if (count.compare_exchange_weak(n, n - 1)) {
                return true;
            }
        }
        return false;
    }

    void show_ice(uint8_t* rdram, recomp_context* ctx) {
        int32_t block = MEM_W(0x68, player_object);
        if (ctx == nullptr || block == 0 || MEM_W(0x44, block) != 0) {
            // No context to call from, or a block already up (a real freeze).
            log_line("Ice Trap: no ice block (slot busy or no context)");
            return;
        }
        recomp_context copy = *ctx;
        copy.r4 = S32(player_object);
        copy.r5 = freeze_bit;
        copy.r6 = ice_visual;
        copy.r7 = S32(-1);
        func_8001FCF8(rdram, &copy);
        ice_effect = MEM_W(0x44, block);
        MEM_W(0x44, block) = 0;
        ice_battle = MEM_HU(0, gBattleState) & 1;
        ice_map = MEM_W(0, gCurrentMap);
        char line[80];
        std::snprintf(line, sizeof line, "Ice Trap: ice block effect at 0x%08X", static_cast<uint32_t>(ice_effect));
        log_line(line);
    }

    void end_ice(uint8_t* rdram) {
        if (ice_effect == 0) {
            return;
        }
        // A battle starting or ending, or a new map, clears the effects; the
        // object may be someone else's by now, so it is left alone.
        if ((MEM_HU(0, gBattleState) & 1) == ice_battle && MEM_W(0, gCurrentMap) == ice_map) {
            uint16_t flags = MEM_HU(8, ice_effect);
            if ((flags & 1) != 0) {
                MEM_H(8, ice_effect) = static_cast<int16_t>(flags & ~1);
                MEM_H(0, ice_effect) = 8;
            }
        }
        ice_effect = 0;
    }

    // Half of max off the current value, floored.
    void drain(uint8_t* rdram, int32_t value_off, int32_t max_off, int floor, const char* what) {
        int value = MEM_HU(value_off, gPlayerMainData);
        int max = MEM_HU(max_off, gPlayerMainData);
        int after = value - max / 2;
        if (after < floor) {
            after = floor;
        }
        MEM_H(value_off, gPlayerMainData) = static_cast<int16_t>(after);
        char line[96];
        std::snprintf(line, sizeof line, "%s Trap: %s %d -> %d (max %d)", what, what, value, after, max);
        log_line(line);
        zelda64::notify::post(std::string(what) + " Trap!");
    }
}

const char* zelda64::traps::name(Trap trap) {
    switch (trap) {
    case Trap::Death: return "Death";
    case Trap::Ice: return "Ice";
    case Trap::Mp: return "MP";
    case Trap::Hp: return "HP";
    }
    return "?";
}

void zelda64::traps::queue(Trap trap) {
    log_line(std::string(name(trap)) + " Trap queued");
    switch (trap) {
    case Trap::Death: zelda64::archipelago::queue_trap_death(); break;
    case Trap::Ice: waiting_ice.fetch_add(1); break;
    case Trap::Mp: waiting_mp.fetch_add(1); break;
    case Trap::Hp: waiting_hp.fetch_add(1); break;
    }
}

bool zelda64::traps::frozen() {
    return is_frozen && !ice_suspended && hold_allowed;
}

void zelda64::traps::on_frame(uint8_t* rdram, recomp_context* ctx) {
    auto now = std::chrono::steady_clock::now();
    struct LockAtExit {
        ~LockAtExit() { buttons_locked.store(is_frozen && !ice_suspended); }
    } lock_at_exit;
    // Back on the title or file select: nothing held over into the next save.
    if (MEM_W(0, gNextMap) == -1) {
        is_frozen = false;
        ice_suspended = false;
        hold_allowed = false;
        ice_effect = 0;
        return;
    }

    // Only ever pin Brian in plain play: standing, walking, battle movement
    // or the skid after letting go of the stick, with no door or map load
    // under way. A door's walk-out and walk-in move Brian by script through
    // the same routine, and stopping those left him stuck in the doorway,
    // out of bounds.
    uint16_t state = MEM_HU(0, player_state);
    bool transition = (static_cast<uint32_t>(MEM_W(0, game_state)) & transition_bits) != 0;
    // Walking out through an exit (state 15, func_80002F60) counts as a
    // transition too, outside a battle. The walk-in on arrival is caught by
    // the room changing. (Not state 3: that is any stand-and-act animation,
    // and counting it let A or Z break the ice.)
    if ((MEM_HU(0, gBattleState) & 1) == 0 && state == 15) {
        transition = true;
    }
    bool plain = state <= 2 || state == 4;
    hold_allowed = plain && !transition;
    int32_t map_now = MEM_W(0, map_word);
    int32_t submap_now = MEM_W(0, submap_word);
    if (transition || map_now != last_map || submap_now != last_submap) {
        last_transition = now;
        last_map = map_now;
        last_submap = submap_now;
    }

    if (is_frozen) {
        bool moved = MEM_W(0, map_word) != ice_room_map || MEM_W(0, submap_word) != ice_room_submap;
        if (!ice_suspended && (transition || moved)) {
            // A door: let go, and start over once Brian is in the new room.
            ice_suspended = true;
            settled_since = {};
            end_ice(rdram);
            log_line("Ice Trap: screen transition, suspended");
        }
        else if (ice_suspended) {
            if (transition || !plain || !ready(rdram)) {
                settled_since = {};
            }
            else if (settled_since == std::chrono::steady_clock::time_point{}) {
                settled_since = now;
            }
            else if (now - settled_since >= resume_after) {
                ice_suspended = false;
                thaw_at = now + ice_time;
                ice_room_map = MEM_W(0, map_word);
                ice_room_submap = MEM_W(0, submap_word);
                show_ice(rdram, ctx);
                log_line("Ice Trap: settled in the new room, frozen again");
            }
        }
        else if (now >= thaw_at) {
            is_frozen = false;
            end_ice(rdram);
            log_line("Ice Trap: thawed");
        }
        else if (ice_effect != 0 && (MEM_HU(0, gBattleState) & 1) != ice_battle) {
            // A battle began or ended under the ice: the game cleared its
            // effects, so the block goes back on.
            ice_effect = 0;
            show_ice(rdram, ctx);
        }
    }

    if (!ready(rdram) || transition || now - last_transition < resume_after) {
        return;
    }
    // One trap a frame, so two that arrive together both show.
    if (take(waiting_ice)) {
        if (is_frozen && !ice_suspended) {
            // A second Ice Trap while frozen adds its five seconds on.
            thaw_at += ice_time;
        }
        else if (!is_frozen) {
            is_frozen = true;
            ice_suspended = false;
            thaw_at = now + ice_time;
            ice_room_map = MEM_W(0, map_word);
            ice_room_submap = MEM_W(0, submap_word);
            show_ice(rdram, ctx);
        }
        log_line("Ice Trap: Brian frozen");
        zelda64::notify::post("Ice Trap!");
    }
    else if (take(waiting_mp)) {
        drain(rdram, 0x8, 0xA, 0, "MP");
    }
    else if (take(waiting_hp)) {
        drain(rdram, 0x4, 0x6, 1, "HP");
    }
}

bool zelda64::traps::input_locked() {
    return buttons_locked.load();
}
