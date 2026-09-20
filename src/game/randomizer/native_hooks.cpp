#include <algorithm>
#include <cmath>
#include <fstream>

#include "hardmode.h"
#include "randomizer.h"
#include "enemy_progression.h"
#include "recomp.h"
#include "zelda_config.h"

// Stage 2 of the Merrow port: the options that patch game *code* rather than
// data. The recomp never executes ROM code (see CLAUDE.md's "Conventions that
// bit us"), so the instruction immediates Merrow's QuestPatchBuild.cs writes
// into the ROM end up baked into RecompiledFuncs/ as C literals and a ROM or
// RAM byte patch has no effect at all. Data-only options still go through the
// ROM writes in randomizer.cpp; only the ones below need hooks.
//
// Every address here was found the same way: translate Merrow's ROM offset to
// a VRAM address (the boot segment loads ROM 0x1000 at 0x80000400, so
// RAM = ROM offset + 0x7FFFF400), then read the matching instruction out of
// RecompiledFuncs/. Merrow's patch offsets usually land on the low byte of a
// 16-bit immediate field, i.e. three bytes into the instruction.
//
// Each hook is registered in us.rev0.toml at the instruction *after* the one
// that sets the register, so it overwrites the vanilla value in the window
// between "set" and "used" and nothing else in the function has to change.
// Where Merrow instead rewrote an instruction to make a branch unconditional
// (changing its rs/rt register to $zero), the hook forces the value the
// existing comparison already tests, which has the same effect.
//
// None of this is verified in-game yet.

using zelda64::randomizer::active_options;
using zelda64::randomizer::Mode;
using zelda64::randomizer::native_state;
using zelda64::randomizer::Options;
namespace progression = zelda64::randomizer::progression;

namespace {
    // Hard Mode replaces the randomizer outright (see quest64_on_init), so
    // these hooks stand down with it on even if the randomizer is enabled.
    bool randomizing() {
        return active_options().mode == Mode::Randomizer && !zelda64::hardmode::active();
    }
}

namespace {
    // Stage 2 diagnostic. Each hook records, the first time it runs, whether
    // its option was on. That separates "the hook never fires" (the function
    // is not the live gate, or is never reached) from "the hook fires but the
    // option never arrived", which look identical from in-game. Written to
    // randomizer_hooks.txt next to the spoiler log.
    void note_hook(const char* name, bool applied) {
        std::ofstream out(zelda64::get_app_folder_path() / "randomizer_hooks.txt", std::ios::app);
        out << name << ": fired, option " << (applied ? "ON (value overridden)" : "off (left vanilla)") << "\n";
    }
}

#define QUEST64_NOTE_HOOK(applied)                       \
    do {                                                 \
        static bool noted_ = false;                      \
        if (!noted_) {                                   \
            noted_ = true;                               \
            note_hook(__func__, (applied));              \
        }                                                \
    } while (0)

extern "C" {

// --- Encounter rate ---------------------------------------------------------
// func_8001C5F4, the field encounter countdown. Merrow patches three
// immediates in it: the step added per frame (ROM 0x01D253), the roll added to
// the countdown (0x01D28F) and the countdown maximum (0x01D28A).

// Vanilla `lui $at, 0x4049` at 0x8001C650 loads the step as the high half of a
// double; Merrow only ever patches its low byte, so 0x40 stays put. Hooked
// before the `mtc1 $at, $f19` that consumes it.
void quest64_randomizer_encounter_step(recomp_context* ctx) {
    const Options& options = active_options();
    QUEST64_NOTE_HOOK(!(!randomizing() || options.encounter_rate == 2));
    if (!randomizing() || options.encounter_rate == 2) {
        return;
    }
    // Halved, Reduced, Default, Increased, Doubled.
    static constexpr uint32_t step_low_byte[5] = { 0x59, 0x54, 0x49, 0x44, 0x39 };
    int index = std::clamp(options.encounter_rate, 0, 4);
    ctx->r1 = S32((0x4000u | step_low_byte[index]) << 16);
}

// Vanilla `addiu $a0, $zero, 0x7D0` (max) and `addiu $t1, $t0, 0x32` (roll)
// both feed the get_rand call at 0x8001C690. Hooked just before it, where r8
// still holds the countdown the roll was added to.
void quest64_randomizer_encounter_roll_max(recomp_context* ctx) {
    const Options& options = active_options();
    QUEST64_NOTE_HOOK(!(!randomizing() || options.encounter_rate == 2));
    if (!randomizing() || options.encounter_rate == 2) {
        return;
    }
    static constexpr uint32_t roll_low_byte[5] = { 0x64, 0x50, 0x32, 0x28, 0x19 };
    static constexpr uint32_t max_value[5] = { 0x0FA0, 0x0960, 0x07D0, 0x0640, 0x03E8 };
    int index = std::clamp(options.encounter_rate, 0, 4);
    ctx->r9 = ADD32(ctx->r8, roll_low_byte[index]);
    ctx->r4 = ADD32(0, max_value[index]);
}

// --- MP regain -------------------------------------------------------------

// Walking MP regen, func_80002F60. `addiu $t9, $t7, 0x1` at 0x80003858 is the
// MP gained per tick; Merrow's "off" setting patches that 1 to 0 (ROM
// 0x00445B). The speed tiers patch a data byte instead, so they are handled by
// the ROM writes and not here. Hooked before the `sh` that stores it.
void quest64_randomizer_walk_mp_regain(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || active_options().mp_regain != 7));
    if (!randomizing() || active_options().mp_regain != 7) {
        return;
    }
    ctx->r25 = ctx->r15;
}

// MP regained per staff hit, func_80004448. `addiu $t3, $t2, 0x1` at
// 0x800044D8 (ROM 0x0050DB) is the amount; Merrow allows 1-9. Hooked before
// the `sh` that stores it, with r10 still holding the pre-hit MP.
void quest64_randomizer_staff_hit_mp(recomp_context* ctx) {
    const Options& options = active_options();
    QUEST64_NOTE_HOOK(!(!randomizing() || options.staff_hit_mp == 1));
    if (!randomizing() || options.staff_hit_mp == 1) {
        return;
    }
    ctx->r11 = ADD32(ctx->r10, std::clamp(options.staff_hit_mp, 0, 9));
}

// --- Element level cap -----------------------------------------------------
// Merrow's "max element uncap" raises the cap from 50 to 99 across six sites.

// func_800074A0, `slti $at, $a0, 0x62` at 0x80007908 gates the EXP-table
// lookup; past the cap the game zeroes EXP progress instead. Merrow raises the
// threshold to 0x126 (ROM 0x00850A), which a level byte can never reach, so
// the lookup path is always taken. Hooked before the `bne` that tests it.
void quest64_randomizer_element_exp_gate(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().element_uncap));
    if (!randomizing() || !active_options().element_uncap) {
        return;
    }
    ctx->r1 = SIGNED(ctx->r4) < 0x126 ? 1 : 0;
}

// Same function, `slti $at, $a0, 0x62` at 0x80007944 is the level-up gate
// itself; Merrow raises it to 0x64 (ROM 0x008546) so growth reaches 99.
void quest64_randomizer_element_growth_cap(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().element_uncap));
    if (!randomizing() || !active_options().element_uncap) {
        return;
    }
    ctx->r1 = SIGNED(ctx->r4) < 0x64 ? 1 : 0;
}

// Same function, `addiu $a0, $zero, 0x32` at 0x80007960 loads the "all
// elements at this level" value the following bnel chain compares against;
// Merrow raises it to 0x63 (ROM 0x008563). Hooked before the first compare.
void quest64_randomizer_element_all_maxed(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().element_uncap));
    if (!randomizing() || !active_options().element_uncap) {
        return;
    }
    ctx->r4 = ADD32(0, 0x63);
}

// func_80002F60, `addiu $a0, $zero, 0x32` at 0x800032D4 is the same "all
// elements maxed" value for the overworld spirit grab (ROM 0x003ED7).
void quest64_randomizer_element_spirit_grab(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().element_uncap));
    if (!randomizing() || !active_options().element_uncap) {
        return;
    }
    ctx->r4 = ADD32(0, 0x63);
}

// func_8001F3DC holds four identical per-element gates,
// `slti $at, $a0, 0x32` at 0x8001F6FC/0x8001F73C/0x8001F77C/0x8001F7C4
// (Merrow's elementCapLocations 0-3), each followed by the `beq` this is
// hooked before. One hook serves all four.
void quest64_randomizer_element_cap(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().element_uncap));
    if (!randomizing() || !active_options().element_uncap) {
        return;
    }
    ctx->r1 = SIGNED(ctx->r4) < 0x63 ? 1 : 0;
}

// --- Enemy drop limit ------------------------------------------------------
// func_80002F60, `beq $v0, $zero, L_800036C0` at 0x800036B0 skips the write
// that marks the drop slot used up. Merrow rewrites the instruction's rt to
// $v0 so it always branches (ROM 0x0042B1); forcing r2 to 0 makes the existing
// `== 0` test pass, which is the same thing.
void quest64_randomizer_drop_limit(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().drop_limit_disabled));
    if (!randomizing() || !active_options().drop_limit_disabled) {
        return;
    }
    ctx->r2 = 0;
}

// --- Wing unlock -----------------------------------------------------------
// func_800222B4 returns whether wings may be used here.

// `andi $t7, $t6, 0xA` at 0x800222C8 tests the indoor flags; Merrow zeroes the
// mask (ROM 0x022ECB) so the "disabled" branch is never taken. Hooked before
// the `bne`.
void quest64_randomizer_wings_indoors(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().wing_unlock_indoors));
    if (!randomizing() || !active_options().wing_unlock_indoors) {
        return;
    }
    ctx->r15 = 0;
}

// `beq $t0, $zero, L_800222F4` at 0x800222E4 skips the disable for the Isle of
// Skye flag. Merrow rewrites its rs to $zero so it always branches (ROM
// 0x022EE4); forcing r8 to 0 makes the existing `== 0` test pass.
void quest64_randomizer_wings_skye(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !active_options().wing_unlock_skye));
    if (!randomizing() || !active_options().wing_unlock_skye) {
        return;
    }
    ctx->r8 = 0;
}

// --- Boss order ------------------------------------------------------------
// func_8001C8C4, `addiu $at, $zero, 0x7` at 0x8001C8D4 is the value the
// following `bne` compares against to take Beigis's special map path (which
// substitutes zeroed coordinates for the real map check). When boss order
// moves Beigis out of his own arena that path misfires, so Merrow changes the
// compared value to 10 (ROM 0x01D4D7), which nothing matches, disabling it.
void quest64_randomizer_beigis_map_check(recomp_context* ctx) {
    QUEST64_NOTE_HOOK(!(!randomizing() || !native_state().beigis_moved));
    if (!randomizing() || !native_state().beigis_moved) {
        return;
    }
    ctx->r1 = ADD32(0, 0x0A);
}

}

// --- Enemy progression: per-area stat scaling -------------------------------
// DOCS/enemyrandologic.xlsx rules 5-6. func_80008EF4 loads an area's monster
// file (the area's table_index picks a row of the file table at 0x80054160:
// rom start, rom end, then RAM pointers to the end of the loaded monster
// table, its start, and a third block) with one synchronous dma_write to
// 0x8020E6F0. Hooked right after that call, the table is rescaled in place
// for the area being loaded, so battle set-up, Soul Searcher, and the exp
// and stones awards all read the same numbers. Every area load re-reads the
// file from the ROM, so nothing compounds. gCurrentMap already names the
// destination at that point.
//
// Entry layout (0x38 bytes): +2 index, +4/+6 HP (twice), +0xC AGI, +0xE DEF,
// +0x10 EXP (word), +0x14 Stones (word), +0x2A ATK.

namespace {
    constexpr int32_t gCurrentMap = 0x80084EEC;
    constexpr int32_t gNextMap = 0x80084EE4;
    constexpr int32_t file_table = 0x80054160;
    constexpr int32_t file_table_row = 0x8008D0BC;   // set by func_80008EF4
    constexpr int entry_size = 0x38;

    // Sheet: Settings B23-B24 (HP 999; ATK/DEF/AGI 255). EXP and Stones are
    // words in the game, capped generously.
    constexpr int hp_cap = 999;
    constexpr int stat_cap = 255;
    constexpr int reward_cap = 65535;

    // The table most recently loaded, for the damage hook's monster test.
    int32_t loaded_table_start = 0;
    int32_t loaded_table_end = 0;

    bool progressing() {
        return randomizing() && progression::active();
    }

    int32_t scaled(int32_t value, double f, int cap) {
        if (f == 1.0) return value;
        double out = std::nearbyint(static_cast<double>(value) * f);
        return static_cast<int32_t>(std::min<double>(std::max<double>(out, 1.0), cap));
    }

    bool is_monster_entry(gpr entry) {
        int32_t e = static_cast<int32_t>(entry);
        return loaded_table_start != 0 && e >= loaded_table_start && e < loaded_table_end;
    }
}

extern "C" {

// func_80008EF4 0x80008F5C: the instruction after `jal dma_write`.
void quest64_randomizer_enemy_scale_table(uint8_t* rdram, recomp_context* ctx) {
    if (!progressing()) return;
    int32_t row = MEM_W(0, file_table_row);
    if (row < file_table || row >= file_table + 6 * 20) return;
    int32_t table = MEM_W(0xC, row);
    int32_t end = MEM_W(0x8, row);
    int entries = (end - table) / entry_size;
    if (entries <= 0 || entries > 16) return;
    loaded_table_start = table;
    loaded_table_end = end;

    int map = MEM_W(0, gCurrentMap);
    if (progression::factor(map, 0, progression::Stat::HP) == 1.0 &&
        progression::factor(MEM_W(0, gNextMap), 0, progression::Stat::HP) != 1.0) {
        map = MEM_W(0, gNextMap);
    }

    std::ofstream out(zelda64::get_app_folder_path() / "randomizer_hooks.txt", std::ios::app);
    out << "enemy_table: map " << map << " table row " << (row - file_table) / 20 << " entries " << entries << "\n";

    for (int i = 0; i < entries; i++) {
        int32_t e = table + i * entry_size;
        auto f = [&](progression::Stat s) { return progression::factor(map, i, s); };
        int32_t hp = scaled(MEM_HU(0x4, e), f(progression::Stat::HP), hp_cap);
        MEM_H(0x4, e) = static_cast<int16_t>(hp);
        MEM_H(0x6, e) = static_cast<int16_t>(scaled(MEM_HU(0x6, e), f(progression::Stat::HP), hp_cap));
        MEM_H(0xC, e) = static_cast<int16_t>(scaled(MEM_HU(0xC, e), f(progression::Stat::AGI), stat_cap));
        MEM_H(0xE, e) = static_cast<int16_t>(scaled(MEM_HU(0xE, e), f(progression::Stat::DEF), stat_cap));
        MEM_H(0x2A, e) = static_cast<int16_t>(scaled(MEM_HU(0x2A, e), f(progression::Stat::ATK), stat_cap));
        MEM_W(0x10, e) = scaled(MEM_W(0x10, e), f(progression::Stat::EXP), reward_cap);
        MEM_W(0x14, e) = scaled(MEM_W(0x14, e), f(progression::Stat::EXP), reward_cap);
        out << "  entry " << i << ": HP " << hp << " ATK " << MEM_HU(0x2A, e) << " DEF " << MEM_HU(0xE, e)
            << " AGI " << MEM_HU(0xC, e) << " EXP " << MEM_W(0x10, e) << "\n";
    }
}

// func_80006BEC(base, ?, attacker record) is the damage Brian takes: base *
// attacker ATK / (ATK + Brian's DEF), then a random spread. The base is the
// spell's own power, which is what actually carries an enemy's damage across
// the game, so it is scaled here on entry (before 0x80006BF8, with s0 = base
// and a2 = the attacker's record, whose +0x64 is its table entry only for
// monsters). Brian's own attacks go through the monster-side formula
// (func_8000ACC0) and never reach this function.
void quest64_randomizer_enemy_scale_damage(uint8_t* rdram, recomp_context* ctx) {
    if (!progressing()) return;
    gpr record = ctx->r6;
    int32_t entry = record ? MEM_W(0x64, record) : 0;
    if (!is_monster_entry(entry)) return;
    int index = MEM_HU(2, entry);
    int map = MEM_W(0, gCurrentMap);
    double f = progression::factor(map, index, progression::Stat::DMG);
    if (f == 1.0) f = progression::factor(MEM_W(0, gNextMap), index, progression::Stat::DMG);
    static int noted = 0;
    if (noted < 20) {
        noted++;
        std::ofstream out(zelda64::get_app_folder_path() / "randomizer_hooks.txt", std::ios::app);
        out << "player_damage: base " << static_cast<int32_t>(ctx->r16) << " entry " << index << " x" << f << "\n";
    }
    ctx->r16 = static_cast<gpr>(scaled(static_cast<int32_t>(ctx->r16), f, reward_cap));
}

}
