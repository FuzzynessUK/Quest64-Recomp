#include <cmath>
#include <cstdint>
#include <cstring>

#include "hardmode.h"
#include "recomp.h"

// The code side of Hard Mode: one native hook for every instruction the hack
// rewrites in the game's own code segment. Each is registered in
// us.rev0.toml at the address given in the comment and does nothing unless
// Hard Mode is active, so the vanilla game is untouched with it off.
//
// The hack's patches come in three shapes, and so do the hooks:
//
//   1. A `j` over the first two instructions of a game function, into a C
//      function in the payload. The hook runs at the function's entry, calls
//      the recompiled payload function and returns, skipping the original.
//   2. A `j` from the middle of a function into a hand-written asm shim that
//      does a little extra work on the registers and jumps straight back.
//      These are reproduced on the register context at the instruction the
//      shim jumps back to (or the one that consumes the changed register).
//   3. A single immediate changed in place (stat caps, positions, floats).
//      The register is overwritten in the window between the instruction
//      that sets it and the one that uses it, the randomizer's pattern.
//
// Addresses below 0x80400000 are the game's; 0x8041xxxx addresses are the
// payload's tables, read from RDRAM so they track the embedded patch rather
// than being copied here. The asm shims are at 0x80410000-0x80410C7C of the
// patched ROM's payload (tools/hardmode/ has the disassembly workflow).

using zelda64::hardmode::active;

extern "C" {
    // Payload functions recompiled by N64Recomp (src/game/hardmode/recompiled).
    void hm_func_804152D4(uint8_t* rdram, recomp_context* ctx); // func_800074A0: stat and level gains
    void hm_func_8041A204(uint8_t* rdram, recomp_context* ctx); // func_80008A00: message text loader
    void hm_func_80410C7C(uint8_t* rdram, recomp_context* ctx); // func_80012220: spirit placement
    void hm_func_8041505C(uint8_t* rdram, recomp_context* ctx); // func_8001EBDC: battle HUD element levels
    void hm_func_804151EC(uint8_t* rdram, recomp_context* ctx); // func_80020D4C: HUD number drawing
    void hm_func_8041C0F8(uint8_t* rdram, recomp_context* ctx); // func_8002684C: map BGM table
    void hm_func_8041C340(uint8_t* rdram, recomp_context* ctx); // func_800278B0: pause "environment" screen
    void hm_func_80414940(uint8_t* rdram, recomp_context* ctx); // func_80029448: pause elements screen
    void hm_func_8041A190(uint8_t* rdram, recomp_context* ctx); // get_rand
    void hm_func_80415710(uint8_t* rdram, recomp_context* ctx); // mainCFunction, once per main-loop pass
}

namespace {
    // --- Payload data ------------------------------------------------------
    // elementCapsTable[gTotalBossesBeatenCount]: the element level cap,
    // raised as bosses fall ({10,20,30,40,40,50,99,150,150} in 0.9.8.3A).
    constexpr int32_t element_caps_table = 0x8041A344;
    constexpr int32_t total_bosses_beaten = 0x8041A3D8;
    // X positions for the level-up element numbers, by digit count.
    constexpr int32_t fire_xpos = 0x8041A398;
    constexpr int32_t earth_xpos = 0x8041A388;
    constexpr int32_t water_xpos = 0x8041A378;
    constexpr int32_t wind_xpos = 0x8041A368;
    // Spell tables: five pointers each (fire, earth, water, wind, boss).
    constexpr int32_t spell_tables_brian = 0x80411668;
    constexpr int32_t spell_tables = 0x8041167C;
    constexpr int32_t spell_data_size = 0x80413F78;
    // Night-time multipliers (floats): monsters get more of everything.
    constexpr int32_t night_stones_mul = 0x8041A3B8;
    constexpr int32_t night_exp_mul = 0x8041A3BC;
    constexpr int32_t night_stat_mul_11a = 0x8041A3C0;
    constexpr int32_t night_stat_mul_114 = 0x8041A3C4;
    constexpr int32_t night_stat_mul_116 = 0x8041A3C8;
    constexpr int32_t night_stat_mul_118 = 0x8041A3CC;
    constexpr int32_t night_hp_mul = 0x8041A3D0;
    // The hack's own text for scripted message 1 (see func_80007A50 below),
    // inside the 64 KB text block at the start of the payload.
    constexpr int32_t message_1_replacement = 0x8040C500;

    // The vanilla spell table pointer array the game indexes with
    // `lui $r, 0x800C ... lw $r, 0x1B14($r)`. Adding this to the register
    // before the lw makes the same instruction read the payload's array.
    constexpr int32_t vanilla_spell_tables = 0x800C1B14;
    constexpr int32_t spell_table_redirect = spell_tables - vanilla_spell_tables;

    // --- Game data -----------------------------------------------------------
    constexpr int32_t gCurrentTime = 0x800859D0;
    constexpr int32_t gBattleState = 0x8008C592;
    constexpr int32_t gCurrentMap = 0x80084EE4;
    constexpr int32_t D_80084F04 = 0x80084F04;
    // Save-file flag bytes the hack adds. 0x800869F2 holds the "doubles
    // experience" special items, 0x800869F3 the second-quest boss flags,
    // 0x800869F0 bit 0x40 switches flag reads to that second set,
    // 0x800869F8 bit 1 doubles the encounter rate.
    constexpr int32_t hm_flags_0 = 0x800869F0;
    constexpr int32_t hm_exp_items = 0x800869F2;
    constexpr int32_t hm_boss_flags = 0x800869F3;
    constexpr int32_t hm_kill_counter = 0x800869F4;
    constexpr int32_t hm_flags_8 = 0x800869F8;
    constexpr int hm_exp_hp = 0x1;   // William's Amulet
    constexpr int hm_exp_mp = 0x2;   // Abbott's Charm
    constexpr int hm_exp_def = 0x4;  // Kiliac's Brace
    constexpr int hm_exp_agi = 0x8;  // Flora's Ring
    // Vanilla flag array the flag get/set functions index; the hack's
    // second set lives at hm_boss_flags.
    constexpr int32_t flag_array = 0x8007D19C;
    constexpr int32_t flag_array_redirect = hm_boss_flags - flag_array;
    // Per-battle bytes the payload maintains, read where the game had
    // constants 0x10/0x18/0x20.
    constexpr int32_t battle_value_0 = 0x8007BBA0;
    constexpr int32_t battle_value_2 = 0x8007BBA2;
    constexpr int32_t battle_value_4 = 0x8007BBA4;
    // Scripted message 1, the one the hack substitutes for Mammon's.
    constexpr int32_t msg_80054828 = 0x80054828;

    int32_t element_cap(uint8_t* rdram) {
        int32_t bosses = MEM_W(0, total_bosses_beaten);
        return MEM_W(bosses * 4, element_caps_table);
    }

    // The payload's night test on gCurrentTime.
    bool night(uint8_t* rdram) {
        int32_t time = MEM_W(0, gCurrentTime);
        return time >= 0x5400 || time < 0x1600;
    }

    // mtc1 / cvt.s.w / mul.s / cvt.w.s / mfc1 with a multiplier from the
    // payload. nearbyint follows the FPU rounding mode the game set.
    gpr scaled(uint8_t* rdram, gpr value, int32_t multiplier_address) {
        int32_t bits = MEM_W(0, multiplier_address);
        float multiplier;
        std::memcpy(&multiplier, &bits, sizeof(multiplier));
        float result = static_cast<float>(static_cast<int32_t>(value)) * multiplier;
        return static_cast<gpr>(static_cast<int32_t>(std::nearbyintf(result)));
    }

    bool boss_battle(uint8_t* rdram) {
        return (MEM_HU(0, gBattleState) & 0x100) != 0;
    }

    // ElementAttackHookC: true unless it is a monster's turn in battle.
    bool brians_turn(uint8_t* rdram) {
        return ((MEM_HU(0, gBattleState) & 0xFEFF) ^ 3) != 0;
    }

    int digits_index(gpr value) {
        int32_t v = static_cast<int32_t>(value);
        return v < 10 ? 0 : (v < 100 ? 1 : 2);
    }

    bool replace(uint8_t* rdram, recomp_context* ctx, void (*payload_func)(uint8_t*, recomp_context*)) {
        if (!active()) {
            return false;
        }
        payload_func(rdram, ctx);
        return true;
    }
}

extern "C" {

// === 1. Function replacements ==============================================
// Hooked at the function's first instruction; returning true makes the
// generated function return before its own body.

int quest64_hardmode_replace_800074A0(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_804152D4);
}
int quest64_hardmode_replace_80008A00(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_8041A204);
}
int quest64_hardmode_replace_80012220(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_80410C7C);
}
int quest64_hardmode_replace_8001EBDC(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_8041505C);
}
int quest64_hardmode_replace_80020D4C(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_804151EC);
}
int quest64_hardmode_replace_8002684C(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_8041C0F8);
}
int quest64_hardmode_replace_800278B0(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_8041C340);
}
int quest64_hardmode_replace_80029448(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_80414940);
}
int quest64_hardmode_replace_get_rand(uint8_t* rdram, recomp_context* ctx) {
    return replace(rdram, ctx, hm_func_8041A190);
}

// The hack turns the two backward branches at the end of the main loop
// (func_80024BEC 0x80024FC0 and 0x80024FF8) into jumps through a shim that
// calls mainCFunction and jumps back to the loop head. Hooked before each
// branch. The shim ran after the branch's delay slot, this runs before it,
// so the registers are put back afterwards to keep the delay slot's
// operands (and everything the loop head expects) as they were.
void quest64_hardmode_main_frame(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) {
        return;
    }
    gpr saved[32];
    std::memcpy(saved, &ctx->r0, sizeof(saved));
    hm_func_80415710(rdram, ctx);
    std::memcpy(&ctx->r0, saved, sizeof(saved));
}

// === 2. Asm shims, reproduced ================================================

// func_80002F60 0x800032D4: `addiu $a0, $zero, 0x32` (all four elements at
// 50 means no more spirits) -> a0 = elementCapsTable[bosses]. Before the
// `ori $t8` at 0x800032D8, shared with the randomizer's hook there.
void quest64_hardmode_spirit_cap(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = static_cast<gpr>(element_cap(rdram));
}

// func_80002F60 0x80003858: MP regained while walking, `addiu $t9, $t7, 1`
// -> no regen when gCurrentMap == D_80084F04. Before the `sh` at 0x8000385C.
void quest64_hardmode_walk_mp_regain(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_W(0, gCurrentMap) == MEM_W(0, D_80084F04)) {
        ctx->r25 = ctx->r15;
    }
}

// func_80003A1C 0x80003AE4: `addiu $t3, $zero, 0x10` -> byte at 0x8007BBA0.
// Before 0x80003AE8.
void quest64_hardmode_battle_value_t3(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r11 = MEM_BU(0, battle_value_0);
}

// Experience gains. Each `addiu $rX, $rY, 1` on a stat's exp counter becomes
// +2 while the matching special item flag is set. Hooked before the store
// (or branch) that follows the add.
// func_80004448 0x80004574 (HP exp, t3 = t2 + 1), before 0x80004578.
void quest64_hardmode_hp_exp_t3(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_hp) ctx->r11 = ADD32(ctx->r10, 2);
}
// func_80006BEC 0x80006E9C (HP exp, t9 = t8 + 1), before 0x80006EA0.
void quest64_hardmode_hp_exp_t9(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_hp) ctx->r25 = ADD32(ctx->r24, 2);
}
// func_80006BEC 0x80006EAC (DEF exp, t2 = t1 + 1), before 0x80006EB0.
void quest64_hardmode_def_exp_t2(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_def) ctx->r10 = ADD32(ctx->r9, 2);
}
// func_800073CC 0x80007470 (AGI exp, t9 = t8 + 1), before 0x80007474.
void quest64_hardmode_agi_exp_t9(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_agi) ctx->r25 = ADD32(ctx->r24, 2);
}
// func_80016520 0x800165B4 (MP exp, t4 = t3 + 1), before 0x800165B8.
void quest64_hardmode_mp_exp_t4(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_mp) ctx->r12 = ADD32(ctx->r11, 2);
}
// func_800165E4 0x80016770 (MP exp, t9 = t8 + 1), before 0x80016774.
void quest64_hardmode_mp_exp_t9(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_exp_items) & hm_exp_mp) ctx->r25 = ADD32(ctx->r24, 2);
}

// Spell table lookups: `lui $r, 0x800C; addu $r, $r, index*4; lw $r,
// 0x1B14($r)` in five functions now read the payload's pointer array.
// Hooked before each lw.
// func_800072A8 0x80007310 (t4).
void quest64_hardmode_spell_table_t4(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r12 = ADD32(ctx->r12, spell_table_redirect);
}
// func_8000B300 0x8000B45C (t5).
void quest64_hardmode_spell_table_t5(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r13 = ADD32(ctx->r13, spell_table_redirect);
}
// func_80014890 0x80014954 (a0).
void quest64_hardmode_spell_table_a0(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = ADD32(ctx->r4, spell_table_redirect);
}
// func_800149D0 0x800149F8 (t0).
void quest64_hardmode_spell_table_t0(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r8 = ADD32(ctx->r8, spell_table_redirect);
}
// func_80014A98 0x80014B58 (t8).
void quest64_hardmode_spell_table_t8(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r24 = ADD32(ctx->r24, spell_table_redirect);
}

// func_80014A98 0x80014B70 (elementAttackHook): after the spell entry
// pointer a2 = table + entry * 0x44 is formed, use Brian's own tables when
// it is his turn. s0 holds the spell id (element << 8 | index). Before the
// `bne` at 0x80014B78 the shim jumps back to.
void quest64_hardmode_brian_spell_entry(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (!brians_turn(rdram)) return;
    int32_t spell = static_cast<int32_t>(ctx->r16);
    int32_t element = (spell >> 8) & 0xFF;
    int32_t index = spell & 0xFF;
    int32_t table = MEM_W(element * 4, spell_tables_brian);
    ctx->r6 = ADD32(table, index * MEM_W(0, spell_data_size));
}

// func_80007A50 shows a scripted message by id through func_80008AD8.
// 0x80007B18, id 1 (the opening text): `lui $a1, 0x8005 ... addiu $a1, $a1,
// 0x4828` -> `lui 0x8041, addiu 0xC500`, i.e. 0x8040C500 (the immediate is
// sign-extended), a message in the payload's text block. Before the jal at
// 0x80007B1C, whose delay slot adds the 0x4828.
void quest64_hardmode_scripted_message_1(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r5 = static_cast<gpr>(static_cast<int32_t>(message_1_replacement - 0x4828));
}
// 0x80007B40, id 4 ("Mammon has been destroyed", 0x80054C04): replaced by
// message 1 unless all of the hack's boss flags are set. Before the jal at
// 0x80007B44 (delay slot adds 0x4C04); shared with the speedrun hook.
void quest64_hardmode_scripted_message_4(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_boss_flags) != 0xFF) {
        ctx->r5 = static_cast<gpr>(static_cast<int32_t>(msg_80054828 - 0x4C04));
    }
}

// func_80004040 0x800040CC: the death jingle, `addiu $a0, $zero, 0x1E` in
// the delay slot of `jal UpdateBGM`, becomes track 0x1D. A delay slot can't
// be hooked (N64Recomp emits it inside the call), so this sits at
// UpdateBGM's entry; that call is the only one in the game passing 0x1E.
void quest64_hardmode_death_jingle(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    if (ctx->r4 == 0x1E) ctx->r4 = 0x1D;
}

// func_80008FE0, monster set-up at the start of a battle: at night the
// stats copied out of the monster table are scaled by the payload's floats.
// 0x8000908C `sh $t4, 0xA($a0)`: before it, t4 (HP).
void quest64_hardmode_night_hp(uint8_t* rdram, recomp_context* ctx) {
    if (!active() || !night(rdram)) return;
    ctx->r12 = scaled(rdram, ctx->r12, night_hp_mul);
}
// 0x80009224 `sh $t8, 0x11A($a0)` then `sh $t6, 0x114($a0)`: before them.
void quest64_hardmode_night_stats_a(uint8_t* rdram, recomp_context* ctx) {
    if (!active() || !night(rdram)) return;
    ctx->r24 = scaled(rdram, ctx->r24, night_stat_mul_11a);
    ctx->r14 = scaled(rdram, ctx->r14, night_stat_mul_114);
}
// 0x80009234 `sh $t9, 0x116($a0)`: before it.
void quest64_hardmode_night_stats_b(uint8_t* rdram, recomp_context* ctx) {
    if (!active() || !night(rdram)) return;
    ctx->r25 = scaled(rdram, ctx->r25, night_stat_mul_116);
}
// 0x80009244 `sh $t1, 0x118($a0)`: before it.
void quest64_hardmode_night_stats_c(uint8_t* rdram, recomp_context* ctx) {
    if (!active() || !night(rdram)) return;
    ctx->r9 = scaled(rdram, ctx->r9, night_stat_mul_118);
}

// func_80009818, rewards after a battle.
// 0x800098D4 `lw $t7, 0x10($t6)` (exp): the shim counts the kill in a
// saturating halfword and doubles exp at night outside boss fights. Before
// 0x800098D8.
void quest64_hardmode_battle_exp(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    uint16_t kills = MEM_HU(0, hm_kill_counter);
    if (kills != 0xFFFF) {
        MEM_H(0, hm_kill_counter) = static_cast<int16_t>(kills + 1);
    }
    if (!boss_battle(rdram) && night(rdram)) {
        ctx->r15 = scaled(rdram, ctx->r15, night_exp_mul);
    }
}
// 0x800098F4 `lw $t1, 0x14($t0)` (stones): doubled at night outside boss
// fights. Before 0x800098F8.
void quest64_hardmode_battle_stones(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (!boss_battle(rdram) && night(rdram)) {
        ctx->r9 = scaled(rdram, ctx->r9, night_stones_mul);
    }
}

// func_8000ACC0 0x8000AD98: `addu $s0, $s0, $t9` (s0 += s0 >> 2) becomes
// `add $s0, $s0, $s0`. Before 0x8000AD9C, so s0 already holds the sum.
void quest64_hardmode_damage_double(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    int32_t original = static_cast<int32_t>(ctx->r16) - static_cast<int32_t>(ctx->r25);
    ctx->r16 = ADD32(original, original);
}

// Game flag get/set (func_8000B9D8 / func_8000BA0C) index a byte array at
// 0x8007D19C; with bit 0x40 of 0x800869F0 set the hack reads and writes its
// own array at 0x800869F3 instead (the boss rematches reuse the flag ids).
// 0x8000B9F4 `lbu $t9, -0x2E64($t9)`: before it, move the base.
void quest64_hardmode_flag_get(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_flags_0) & 0x40) ctx->r25 = ADD32(ctx->r25, flag_array_redirect);
}
// 0x8000BA1C `addiu $t8, $t8, -0x2E64`: before the `sra` at 0x8000BA20.
void quest64_hardmode_flag_set(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_flags_0) & 0x40) ctx->r24 = ADD32(ctx->r24, flag_array_redirect);
}

// func_8000BB68, a max HP gain: capped at 999 instead of 500.
// 0x8000BCD8 `slti $at, $v0, 0x1F5`: before the `bne` at 0x8000BCDC.
void quest64_hardmode_max_hp_gain_check(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = static_cast<int32_t>(ctx->r2) < 1000 ? 1 : 0;
}
// 0x8000BCE4 `addiu $t8, $zero, 0x1F4`: before the `sh` at 0x8000BCE8.
void quest64_hardmode_max_hp_gain_clamp(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r24 = 999;
}

// func_80010150 0x800102BC `sh $zero, 0x5388($at)` -> `sh $zero,
// 0x5300($at)`: the store is moved 0x88 down. Before it.
void quest64_hardmode_store_redirect(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = ADD32(ctx->r1, -0x88);
}

// func_80016CB0 0x80016CF4, func_800170DC 0x80017128 / 0x8001713C,
// func_8001817C 0x80018194: constants 0x10 / 0x18 / 0x20 / 0x10 replaced by
// bytes the payload keeps at 0x8007BBA0/A2/A4. Hooked before the next
// instruction in each case.
void quest64_hardmode_battle_value_t9_0(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r25 = MEM_BU(0, battle_value_0);
}
void quest64_hardmode_battle_value_t9_2(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r25 = MEM_BU(0, battle_value_2);
}
void quest64_hardmode_battle_value_t2_4(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r10 = MEM_BU(0, battle_value_4);
}
void quest64_hardmode_battle_value_t0(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r8 = MEM_BU(0, battle_value_0);
}

// func_8001C5F4, the field encounter countdown: with bit 1 of 0x800869F8
// set the step is 100.0 rather than 50.0 and the roll 4000/+100 rather than
// 2000/+50. Both share addresses with the randomizer's encounter hooks.
// 0x8001C650 `lui $at, 0x4049`: before the mtc1 at 0x8001C654.
void quest64_hardmode_encounter_step(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_flags_8) & 1) ctx->r1 = S32(0x4059 << 16);
}
// 0x8001C688 `addiu $a0, $zero, 0x7D0; addiu $t1, $t0, 0x32`: before the
// jal at 0x8001C690.
void quest64_hardmode_encounter_roll(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    if (MEM_BU(0, hm_flags_8) & 1) {
        ctx->r4 = 0xFA0;
        ctx->r9 = ADD32(ctx->r8, 0x64);
    }
}

// func_8001E25C 0x8001E4AC `addiu $t6, $t5, 1` -> `+ 0`. Before 0x8001E4B0.
void quest64_hardmode_hud_counter(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r14 = ctx->r13;
}

// func_8001F3DC, the level-up screen's four element numbers: the shims set
// a0 = 3 and pick the X position (a1) by how many digits the level has, then
// jump to the jal, whose delay slot supplies a2. Hooked before each jal.
void quest64_hardmode_levelup_fire(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = 3;
    ctx->r5 = MEM_W(digits_index(ctx->r7) * 4, fire_xpos);
}
void quest64_hardmode_levelup_earth(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = 3;
    ctx->r5 = MEM_W(digits_index(ctx->r7) * 4, earth_xpos);
}
void quest64_hardmode_levelup_water(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = 3;
    ctx->r5 = MEM_W(digits_index(ctx->r7) * 4, water_xpos);
}
void quest64_hardmode_levelup_wind(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r4 = 3;
    ctx->r5 = MEM_W(digits_index(ctx->r7) * 4, wind_xpos);
}

// func_8001F3DC 0x8001F6F8 / 0x8001F738 / 0x8001F778 / 0x8001F7C0: the four
// "is this element below 50" gates become "below elementCapsTable[bosses]".
// Before each `beq $at` (0x8001F700 etc.), shared with the randomizer.
void quest64_hardmode_element_cap(uint8_t* rdram, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = static_cast<int32_t>(ctx->r4) < element_cap(rdram) ? 1 : 0;
}

// func_800222B4, "may wings be used here": both tests are defeated.
// 0x800222C8 `andi $t7, $t6, 0xA` -> 0: before the bne at 0x800222CC.
void quest64_hardmode_wings_indoors(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r15 = 0;
}
// 0x800222E4 `beq $t0, $zero` made unconditional: before it.
void quest64_hardmode_wings_skye(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r8 = 0;
}

// func_800228F8 0x80022918 `andi $t7, $t6, 1` -> 0. Before the beq at
// 0x8002291C.
void quest64_hardmode_flag_test_off(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r15 = 0;
}

// func_80002F60 0x800036B0: the branch that skips marking a drop slot used
// up is made unconditional. Before it, shared with the randomizer.
void quest64_hardmode_drop_lock(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r2 = 0;
}

// === 3. Immediates ============================================================

// func_8000534C 0x80005418 `lui $at, 0x4000` (2.0): walk speed 2.75. Before
// the mtc1 at 0x8000541C.
void quest64_hardmode_walk_speed(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = S32(0x4030 << 16);
}

// func_80026F34 0x800276C0 and 0x80027720 `lui $at, 0x437F` (255.0) ->
// 0x4479 (996.0). Before the mtc1 that follows each.
void quest64_hardmode_bar_scale(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = S32(0x4479 << 16);
}

// guLookAtReflectF 0x80035D88 and 0x80035E24 `lui $at, 0x3FF0` (1.0 as a
// double's high word) -> 0x3FE4 (0.625). Before the mtc1 that follows each.
void quest64_hardmode_lookat(uint8_t*, recomp_context* ctx) {
    if (!active()) return;
    ctx->r1 = S32(0x3FE4 << 16);
}

}
