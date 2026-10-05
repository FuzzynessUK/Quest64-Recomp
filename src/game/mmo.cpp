#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <random>
#include <string>

#include "mmo.h"
#include "enhancements.h"
#include "hardmode.h"
#include "zelda_config.h"
#include "recomp.h"

extern "C" void func_8001817C(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_80019CA4(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_80013F20(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_800268D4(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_800208B8(uint8_t* rdram, recomp_context* ctx);

// Real Time Combat (Enhancements, Fun): real time battles instead of turn based.
//
// How a vanilla battle takes turns. gBattleState (0x8008C592) bit 0x1 is
// "in battle", 0x2 "an enemy's turn", 0x4 "the turn is changing", 0x10
// escaped, 0x100 a boss, 0x200 the opening camera sweep, 0x400 every enemy
// down. func_8001D358 ends a turn - every action Brian or an enemy finishes
// calls it: it runs the actor's status countdown (func_8001817C), removes
// the turn's ring, on Brian's turn picks the next living enemy from the order
// list (0x8007D0B0, index 0x8008C598) and sets a 20-frame pause (0x8008C594,
// which also holds Brian's stick), toggles 0x2 and sets 0x4. The battle
// manager, func_8001CFE8, then waits out the pause, ends the battle on 0x400
// or an escape, gives the enemy whose turn it is its "act" bit (+0x8 bit 1
// of its 0x128-byte slot, set by func_8000932C), and draws the new ring
// round whoever acts (func_80019A98) - the circle Brian can walk in, which
// func_80005748 holds him to (centre 0x8008C5A4 / 0x8008C430). The camera
// follows whoever's turn it is.
//
// Every enemy's action handler runs every frame (func_80008C20, table
// 0x8004C290 by the slot's action id); the idle one, action 0, does nothing
// until its "act" bit is set, then approaches, strikes or casts and, when
// that is over, ends the turn.
//
// So, in Real Time Combat, in battle:
//   - ending a turn runs the status countdown and nothing else: it is always
//     Brian's turn, his stick is never held, the camera stays on him;
//   - each idle enemy gets its "act" bit on a timer of its own, so they all
//     come at him at once, each going back to idle and waiting again;
//   - the ring's centre follows Brian, so he is never held to it, and the
//     ring itself is taken away; the edge of the arena still lets him run;
//   - every enemy down, or Brian off the edge of the arena, raises 0x4 so the
//     battle manager ends the battle the way it would have at a turn change.
namespace {
    constexpr int32_t gBattleState = 0x8008C592;
    constexpr int32_t turn_pause = 0x8008C594;
    constexpr int32_t ring_handle = 0x8008C55E;
    constexpr int32_t ring_centre_x = 0x8008C5A4;
    constexpr int32_t ring_centre_z = 0x8008C430;
    constexpr int32_t brian = 0x8007BACC;
    constexpr int32_t brian_flags = 0x8007BAC0;      // bit 0x20: off the edge of the arena
    // Enemies still standing: the game takes one off as each dies (0x800098C4,
    // 0x80016C58) without moving the others down, so it is not how many slots
    // are in use - a survivor can sit in a slot past it. Walk all six slots.
    constexpr int32_t enemy_count = 0x8007C990;
    constexpr int32_t enemies = 0x8007C998;
    constexpr int enemy_size = 0x128;
    constexpr int max_enemies = 6;

    // Game frames (the game runs at 30). Each enemy keeps a clock of its own:
    // after each of its actions it rests before the next - a rest of its own,
    // drawn once a battle between three and four and a half seconds, and up to
    // half a second more each time (never under three). An action itself takes about three
    // seconds, so with one rest for everybody two enemies settled into "you
    // act while I rest" and it looked like turns; rests of their own drift
    // apart. Their first actions are spread over the opening three seconds.
    constexpr int cooldown = 90;
    constexpr int cooldown_max = 135;
    constexpr int cooldown_jitter = 15;
    std::array<int, 6> rest{};
    constexpr int first_wait_min = 5;
    constexpr int jitter = 10;

    std::array<int, max_enemies> wait{};
    // No pacing: every monster acts the moment its own timer runs out, however
    // many are at it already and whatever Brian is doing. Unfair on purpose.
    constexpr int32_t brian_state = 0x8007BAB8;    // u16, for the log
    int note_frames = 0;
    bool was_in_battle = false;
    bool ring_gone = false;
    std::mt19937 rng{ std::random_device{}() };

    int roll(int lo, int hi) {
        return std::uniform_int_distribution<int>(lo, hi)(rng);
    }

    void log_line(const std::string& line) {
        std::ofstream out(zelda64::get_app_folder_path() / "mmo.txt", std::ios::app);
        out << line << "\n";
    }

    bool on() {
        // Hard Mode too: it changes nothing the turns run on (end turn, the
        // battle manager, the enemy update and actions, the close-up, the
        // movement routines), and its hook inside the status countdown still
        // runs, since the countdown is called as it is.
        return zelda64::enhancements::active_options().real_time_combat;
    }

    float mem_f(uint8_t* rdram, int32_t addr) {
        uint32_t bits = static_cast<uint32_t>(MEM_W(0, addr));
        float f;
        std::memcpy(&f, &bits, sizeof f);
        return f;
    }
    void set_f(uint8_t* rdram, int32_t addr, float f) {
        uint32_t bits;
        std::memcpy(&bits, &f, sizeof bits);
        MEM_W(0, addr) = static_cast<int32_t>(bits);
    }

    bool enemy_alive(uint8_t* rdram, int slot) {
        const int32_t e = enemies + slot * enemy_size;
        return MEM_H(0x74, e) != -1 && MEM_HU(0xA, e) != 0;
    }
}

bool zelda64::mmo::active() {
    return on();
}

void zelda64::mmo::on_frame(uint8_t* rdram, recomp_context* ctx) {
    if (!on()) {
        return;
    }
    uint16_t state = MEM_HU(0, gBattleState);
    const bool in_battle = (state & 0x1) != 0;
    if (!in_battle) {
        was_in_battle = false;
        return;
    }
    if (!was_in_battle) {
        was_in_battle = true;
        ring_gone = false;
        int present = MEM_W(0, enemy_count);
        present = present < 1 ? 1 : (present > max_enemies ? max_enemies : present);
        for (int slot = 0; slot < max_enemies; slot++) {
            wait[slot] = first_wait_min + (cooldown * (slot % present)) / present + roll(0, jitter);
            rest[slot] = roll(cooldown, cooldown_max);
        }
        log_line("battle: real time");
    }
    // The opening sweep, or the manager acting on a turn change or the end.
    if ((state & 0x200) != 0 || (state & 0x4) != 0) {
        return;
    }

    // It is always Brian's turn. A battle the enemies open (they get the
    // first move) starts with 0x2 set, and with no turn ever ending it would
    // stay set: Brian's stick is held while it is (func_80003B60) and the
    // camera follows that enemy.
    if ((state & 0x2) != 0) {
        MEM_H(0, gBattleState) = static_cast<int16_t>(state & ~0x2);
        state = static_cast<uint16_t>(state & ~0x2);
        log_line("the enemies had the first move: it is Brian's turn");
    }

    // The ring is not drawn (nobody is held to it: see the movement hooks
    // below).
    if (!ring_gone) {
        ring_gone = true;
        recomp_context c = *ctx;
        c.r29 = ADD32(ctx->r29, -0x40);
        c.r4 = MEM_HU(0, ring_handle);
        func_80019CA4(rdram, &c);
    }

    // The end of the battle, as a turn change would have found it.
    const int count = max_enemies;
    int alive = 0;
    for (int slot = 0; slot < count; slot++) {
        alive += enemy_alive(rdram, slot) ? 1 : 0;
    }
    if (alive == 0) {
        MEM_H(0, gBattleState) = static_cast<int16_t>(state | 0x400 | 0x4);
        MEM_H(0, turn_pause) = 0x14;
        log_line("every enemy down: the battle ends");
        return;
    }
    if ((state & 0x10) != 0 || ((state & 0x100) == 0 && (MEM_HU(0, brian_flags) & 0x20) != 0)) {
        MEM_H(0, gBattleState) = static_cast<int16_t>(state | 0x4);
        log_line("off the edge: escaping");
        return;
    }

    // Every two seconds, what is going on, for when it is not what it should
    // be: Brian's state and its timer, the battle bits, the things that hold
    // his stick (the turn pause, two flags at 0x8008C638/C, the menu mask),
    // and each enemy's action and flags.
    if (--note_frames <= 0) {
        note_frames = 60;
        char line[256];
        int n = std::snprintf(line, sizeof line, "brian state %d timer %d | battle %04X pause %d locks %d %d menu %08X | enemies",
                              MEM_HU(0, brian_state), MEM_HU(4, brian_state), state, MEM_HU(0, turn_pause),
                              MEM_W(0, 0x8008C638), MEM_W(0, 0x8008C63C), static_cast<uint32_t>(MEM_W(0, 0x8007B2E4)));
        for (int slot = 0; slot < count && n > 0 && n < static_cast<int>(sizeof line) - 24; slot++) {
            const int32_t e = enemies + slot * enemy_size;
            if (MEM_H(0x74, e) == -1) {
                continue;
            }
            n += std::snprintf(line + n, sizeof line - n, " [%d hp %d act %d fl %X]", slot, MEM_HU(0xA, e), MEM_HU(0, e), MEM_HU(0x8, e));
        }
        log_line(line);
    }

    // Each idle enemy counts down on its own timer and goes when it is ready.
    for (int slot = 0; slot < count; slot++) {
        if (!enemy_alive(rdram, slot)) {
            continue;
        }
        const int32_t e = enemies + slot * enemy_size;
        const bool idle = MEM_HU(0, e) == 0 && (MEM_HU(0x8, e) & 0x1) == 0;
        if (!idle) {
            continue;
        }
        if (wait[slot] > 0) {
            wait[slot]--;
        }
        if (wait[slot] == 0) {
            MEM_H(0x8, e) = static_cast<int16_t>(MEM_HU(0x8, e) | 0x1);
            wait[slot] = rest[slot] + roll(0, cooldown_jitter);
        }
    }
}

// func_8001D358 at its first instruction, a0 the actor whose action has
// finished. In battle, only the status countdown: the turn stays Brian's.
extern "C" int quest64_mmo_end_turn(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0) {
        return 0;
    }
    func_8001817C(rdram, ctx);
    return 1;
}

// func_800140EC(?, actor) at its first instruction: in battle it saves the
// camera (to 0x80086B00, setting camera flag 0x40) and swings it round to
// the actor for a close-up; an enemy's attack or spell calls it
// (func_8000A284), and so do two of the world update's routines. The camera
// puts itself back only when that flag is set, so refusing the close-up
// leaves nothing to undo. In Real Time Combat the camera never leaves Brian: only
// his own close-ups go ahead.
extern "C" int quest64_mmo_camera_focus(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0) {
        return 0;
    }
    return static_cast<int32_t>(ctx->r5) != brian ? 1 : 0;
}

// The turn ring holds whoever is moving inside it: Brian through
// func_80005748, an enemy through func_8000A508 (which sets bit 0x2 of the
// enemy's flags when it reaches the edge, and the approach, func_80009588,
// gives up there). In a vanilla battle it is centred on whoever's turn it
// is. With everyone acting at once there is no such actor, so each movement
// routine is handed a ring centred on the one moving, at its entry: a step
// is always far shorter than the ring is wide, so nobody is held back, and
// an enemy walks all the way to Brian. The arena's edge, tested next in
// both, still holds.
namespace {
    void centre_ring_on(uint8_t* rdram, int32_t actor) {
        set_f(rdram, ring_centre_x, mem_f(rdram, actor + 0x0));
        set_f(rdram, ring_centre_z, mem_f(rdram, actor + 0x8));
    }
}

// func_80005748 at its entry, a1 = Brian.
extern "C" void quest64_mmo_brian_ring(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0) {
        return;
    }
    centre_ring_on(rdram, static_cast<int32_t>(ctx->r5));
}

// func_8000A508(moving, x, z, actor) at its entry, a3 = the enemy.
extern "C" void quest64_mmo_enemy_ring(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0) {
        return;
    }
    centre_ring_on(rdram, static_cast<int32_t>(ctx->r7));
}

// Hit stun. A hit on Brian (func_80006BEC) or an enemy (func_8000ACC0) takes
// the HP off, shows the number, then stuns: Brian goes to state 5 for a time
// from the attacker's data (func_80004040 waits it out, then dies at 0 HP or
// goes back to state 0), an enemy to action 2 (func_80009818: knocked back,
// then dies at 0 HP or goes back to idle), each with its hurt animation
// (func_8001D8B0) and hit flags (+0x60 bits 0/1, cleared when the stun ends).
// Whatever the one hit was doing - an attack, a spell - is lost. In Real Time
// Combat a hit that leaves the target standing skips all of that: the hooks
// sit where the stun begins and finish the routine themselves, doing what
// remains after it (the shake on Brian, the hit sound), restoring s0 and sp
// as the epilogue would. A hit that kills goes on as before, since the stun's
// end is where the death happens.
namespace {
    constexpr int32_t brian_hp = 0x8007BA84;

    void hit_sound(uint8_t* rdram, recomp_context* ctx, uint32_t flags) {
        if ((flags & 0x8000) == 0) {
            return;
        }
        ctx->r4 = 0;
        ctx->r5 = 0x18;
        ctx->r6 = 0xFF;
        func_800268D4(rdram, ctx);
    }

    // The routine's epilogue: v0 = damage (s0), s0 and ra back, frame popped.
    void leave(uint8_t* rdram, recomp_context* ctx, int frame, gpr damage) {
        const gpr sp = ctx->r29;
        ctx->r2 = damage;
        ctx->r31 = MEM_W(0x24, sp);
        ctx->r16 = MEM_W(0x20, sp);
        ctx->r29 = ADD32(sp, frame);
    }
}

// func_80006BEC at 0x80006EBC: s0 = the damage, the flags halfword at 0x4E(sp).
extern "C" int quest64_mmo_brian_hit(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0 || MEM_HU(0, brian_hp) == 0) {
        return 0;
    }
    const gpr damage = ctx->r16;
    const uint32_t flags = MEM_HU(0x4E, ctx->r29);
    if (damage != 0) {
        ctx->r4 = 2;
        func_80013F20(rdram, ctx);
    }
    hit_sound(rdram, ctx, flags);
    leave(rdram, ctx, 0x48, damage);
    return 1;
}

// func_8000ACC0 at 0x8000AFFC: v1 = the enemy's slot, s0 = the damage, the
// flags halfword at 0x5E(sp).
extern "C" int quest64_mmo_enemy_hit(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0 || MEM_HU(0xA, ctx->r3) == 0) {
        return 0;
    }
    const gpr damage = ctx->r16;
    hit_sound(rdram, ctx, MEM_HU(0x5E, ctx->r29));
    leave(rdram, ctx, 0x50, damage);
    return 1;
}

// Spells in flight. A cast takes the first free one of ten 0x3C-byte slots
// at 0x80086F18 (func_80014A98: +4 nonzero while the spell runs, +0x24 the
// caster), and func_80015B50 says whether any slot is in use. Only Brian's
// code asks: whether he can act (func_80007030, which the spell menu, the
// element buttons and items sit behind), the attack button
// (func_80002F60), and the end of his cast and item poses (func_800045F0,
// func_80004AB8), which wait for it. Turn based, nobody else casts in his
// turn; in Real Time Combat a monster's spell - a Wind spell lingering in
// Blue Cave - locked him out until it was over. In battle, only his own
// spells count.
extern "C" int quest64_mmo_spells_active(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0) {
        return 0;
    }
    constexpr int32_t spell_slots = 0x80086F18;
    constexpr int spell_slot_size = 0x3C;
    constexpr int spell_slot_count = 10;
    int mine = 0;
    for (int i = 0; i < spell_slot_count; i++) {
        const int32_t slot = spell_slots + i * spell_slot_size;
        if (MEM_HU(0x4, slot) != 0 && MEM_W(0x24, slot) == brian) {
            mine++;
        }
    }
    ctx->r2 = mine;
    return 1;
}


// Dodge. When a monster's attack or spell misses Brian (func_80009C08's
// strike, func_80015888's spell hit), func_80006F6C shows "Miss"
// (func_800208B8), plays the dodge sound (0x17) and the evade animation (7),
// and freezes him: state 3 for a time from the attacker's data, with the busy
// bits (+0x60 bits 0/1) that func_80007030 refuses every action on until it
// ends - the spell menu "not coming up at the exact moment of a hit", more
// often the higher his Agility. In a Real Time Combat battle the dodge is the
// "Miss" and the sound only, like the hits that land: nothing he is doing is
// cut off. At 0 HP it does nothing in either case.
extern "C" int quest64_mmo_dodge(uint8_t* rdram, recomp_context* ctx) {
    if (!on() || (MEM_HU(0, gBattleState) & 0x1) == 0 || MEM_HU(0, brian_state) == 6 ||
        MEM_HU(0, brian_hp) == 0) {
        return 0;
    }
    ctx->r4 = brian;
    func_800208B8(rdram, ctx);
    ctx->r4 = 0;
    ctx->r5 = 0x17;
    ctx->r6 = 0xFF;
    func_800268D4(rdram, ctx);
    return 1;
}
