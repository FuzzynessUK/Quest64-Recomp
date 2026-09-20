#include <algorithm>
#include <fstream>

#include "hardmode.h"
#include "randomizer.h"
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
