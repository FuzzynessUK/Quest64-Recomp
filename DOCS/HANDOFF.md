# Quest 64 Recomp — working notes and handoff

Written 2026-09-18 at the end of a long session with Claude Code, so the work
can be picked up on another day or another machine. Everything below is on
the fork branch `local/submodule-url-fix` (github.com/FuzzynessUK/Quest64-Recomp).

## Build and deploy

- Configure/build: `cmake --build build-cmake --target Quest64Recompiled`
  (Ninja, clang-cl; cmake lives under Visual Studio's CMake folder on the
  original machine).
- The game is **run from `D:\Games\Quest64Final\`**, not from `build-cmake/`
  (which has no `assets/`). After every build copy
  `build-cmake/Quest64Recompiled.exe` and any changed files under `assets/`
  there. Forgetting this once cost an hour of testing the previous night's exe.
- Hooks in `us.rev0.toml` need `N64Recomp.exe us.rev0.toml` re-run to
  regenerate `RecompiledFuncs/` (not in git) before building.
- Settings and logs live in `%LOCALAPPDATA%\Quest64Recompiled\`
  (`randomizer.json`, `randomizer_spoiler.txt`, `widescreen_rects.txt`).

### On the laptop (set up 2026-09-18)

The laptop has no Visual Studio, so the toolchain went in from scratch: VS2022
Build Tools (Desktop C++ / Clang / CMake components), `make` via winget
(`ezwinports.make`), and `N64Recomp.exe` + `RSPRecomp.exe` built from
github.com/N64Recomp/N64Recomp and copied to the repo root. `rebuild.bat` in
the repo root wraps the whole thing with the right absolute paths.

The one non-obvious part: **the patches/ MIPS build needs a clang with the MIPS
backend**, which neither VS's bundled clang nor the LLVM installer package has
any more (both only register x86/ARM). Use the full release archive instead —
`LLVM-19.1.3-Windows-X64.tar.xz`, the same one this repo's own
`.github/workflows/validate.yml` downloads — unpacked at
`D:\Games\tools\LLVM-19.1.3-Windows-X64`, and point CMake at it:

```
-DPATCHES_C_COMPILER="D:\Games\tools\LLVM-19.1.3-Windows-X64\bin\clang.exe"
-DPATCHES_LD="D:\Games\tools\LLVM-19.1.3-Windows-X64\bin\ld.lld.exe"
```

`lib/sf64decomp` (libultra headers for patches/) was moved out of the repo to
`D:\Games\reference\sf64decomp` and unregistered as a submodule;
`patches/Makefile` now includes it from `../../reference/sf64decomp`. Merrow's
source is checked out next to it at `D:\Games\reference\merrow` for reference.

## What exists now

### Cheats tab (F5)
- **Warp**: map / submap / entrance dropdowns. Map names came from Merrow's
  encounter data matched to the ROM map table plus the exit graph; submaps
  and entrances describe themselves from the ROM's exit records
  ("to Larapool", "from submap 1"). Counts come from the map table so nothing
  out of range can be chosen. `src/game/map_table.cpp`.
- **Brian's stats**: HP/MP/max/agility/defense/four elements, live sliders.
- **Movement speed**: 100–150%, per-step displacement clamped to 3 units so
  the position-based wall test can't be skipped (walls ~3.5 thick).
- Game-side code is `src/game/debug.cpp`, hooked once per frame at
  `func_80026658` (`quest64_cheats_frame`).

### Randomizer tab (F6 / launcher button) — in-app port of Merrow (Stage 1)
- Vanilla / Randomizer mode, seed, Merrow's data-only options. Patch is
  applied to the in-memory ROM at boot (`on_init_callback`), ROM on disk is
  never touched. `src/game/randomizer/`; tables generated from Merrow's
  DataStore.cs by `tools/convert_merrow_datastore.pl`.
- Stage 2 (written 2026-09-18, **none of it play-tested yet**): the Merrow
  options that patch *code* rather than data, as native hooks. See
  `src/game/randomizer/native_hooks.cpp` and the Stage 2 block at the bottom
  of `us.rev0.toml`. All of it is in the Randomizer tab's World and Cosmetics
  sections.
- Stage 3: cosmetics done (text and staff palettes). Lost Keys mode and enemy
  composition shuffle are still open — see "Randomizer work still open".

### How the Stage 2 hooks work

Merrow writes its code options as raw ROM byte patches. Those can't work here:
N64Recomp bakes instruction immediates into `RecompiledFuncs/` as C literals,
so patching the ROM (or the mirrored RAM) does nothing at all. The ROM-write
path in `randomizer.cpp` still handles every data-only option.

Finding each hook site followed the same recipe, which is worth reusing:

1. Translate Merrow's ROM offset to a VRAM address. The boot segment loads ROM
   0x1000 at 0x80000400, so **RAM = ROM offset + 0x7FFFF400**. This holds for
   everything up to about 0x80049D50; above that, as far as 0x80100000, is the
   data segment (the map table, `gPlayerMainData` and friends all live there),
   and addresses landing in it are genuine runtime data that the existing ROM
   writes already reach.
2. Merrow's offsets almost always point at the *low byte of a 16-bit
   immediate*, i.e. three bytes into the instruction, so the instruction starts
   at `offset - 3`. Grep `RecompiledFuncs/` for that address to read the real
   mnemonic; `xxd` on `quest64.us.z64` plus a manual opcode decode confirms it.
3. Register the hook at the instruction *after* the one that sets the register,
   so it overwrites the vanilla value in the window between "set" and "used".
   Nothing else in the function has to change, which matters — several of these
   sites sit inside large movement/collision functions that would be far riskier
   to replace wholesale.
4. Where Merrow rewrote an instruction to force a branch (changing its rs/rt to
   `$zero` so a `beq`/`bne` always takes), the hook instead forces the value the
   existing comparison already tests. Same effect, no control-flow surgery.

Sites, for reference: encounter step/roll/max in `func_8001C5F4`; walking MP
regen in `func_80002F60`; staff-hit MP in `func_80004448`; element cap across
`func_800074A0`, `func_80002F60` and four identical gates in `func_8001F3DC`;
drop limit in `func_80002F60`; wings in `func_800222B4`; Beigis's map check in
`func_8001C8C4`.

### What to look for when testing Stage 2

Every one of these is unverified. In rough order of "most likely to be wrong":

- **Encounter rate** — the step value is the low byte of a `lui` that builds the
  high half of a *double*, so it is the setting most likely to misbehave in a
  way that isn't just "wrong rate". Watch for encounters that never fire or
  fire constantly.
- **Element cap 99** — four per-element gates are assumed identical because they
  are byte-identical in the disassembly and Merrow patches them identically;
  only the first was read in full.
- **Beigis map check** — only triggers when boss order actually moves him, so it
  needs a seed where `boss_order[6] != 6`. The spoiler log now says when that
  happened.
- **Wings indoors / on Skye**, **no drop limit**, **MP regen off**, **staff-hit
  MP** — simplest of the set, each a single register override.

### Widescreen 2D fix — IN PROGRESS
`src/game/widescreen.cpp`, hooked in `nnScExecuteGraphics` at `0x80000B08`
just before `osSpTaskLoad`. Walks each frame's display list (F3DEX 1.23
opcodes) and replaces qualifying rectangles with a `gSPDisplayList` branch
(same 8 bytes) into a sub-list at `0x807E0000+` (unused upper RDRAM) that
draws the rect with RT64's `gEXFillRectangle`/`gEXTextureRectangle` using
LEFT/RIGHT origins, so it spans the whole screen.

Confirmed working: transition fade (a 0..336×0..960 fill) and the pause
menu background (8 strips at x 23..297 drawn with a 64×32 wrapping tile).
The title screen's 320×6 image strips are deliberately left alone (texture
width > 128 = image, keep centred).

**State at end of 2026-09-18**: title fade, pause backdrop, item-menu dim
and door fade all extend to the screen edges. Key facts learned the hard way:
- The game's 2D lists sit behind RSP segment 1; the walker tracks gSPSegment.
- F3DEX 1.x gSPVertex packs n in bits 10-15 of w0 (not the SDK nibble).
- The fade/dim is a static -160..160 x -120..120 quad list (0x8004D4F0,
  vertices 0x8004D4B0) drawn as shade*env; the walker redirects the branch
  into it to a sub-list drawing gEXFillRectangle with prim=vertex colour.
- Every redirected draw needs gEXSetScissorAlign(LEFT, RIGHT, 0,0,-320,0)
  + re-issued game scissor, then a reset; a real extended scissor breaks
  RT64's frame aspect detection and stretches the whole game.

Blue bars at the far edges outdoors were the extended frame clear painting
the sky colour into the border; the clear (8..311) is now left alone and
only fills starting at x=0 are extended. Remaining cleanup: remove the F9
frame dump and per-rect logging (`log_rects`) once the fixes have had more
play-testing; the General-tab toggle "Widescreen 2D Fixes" stays.

## Reverse-engineering facts worth keeping

- Field transitions: exit record `D_80085368` = {map, submap, entrance},
  `gGameState` (0x8007B2E4) bit 0x80 = leaving, `D_8007B2EC` countdown;
  `gGameMode` (0x8007B2E0) 1 = field. `func_80016B18` is the minimal example.
  The block at 0x8008FCC0 is the BGM system, not maps.
- Player struct 0x8007BACC: +0 x, +8 z, +0x10 heading, +0x18/+0x20 frame
  velocity. `gPlayerMainData` 0x8007BA80: +4 HP, +6 maxHP, +8 MP, +A maxMP,
  +C agi, +E def, +0x24..0x27 Fire/Earth/Wind/Water.
- Map table 0x80054F10, 0x44 bytes/map, 36 maps; submap headers at
  entry+0x18 (24 bytes: +4 exits, +8 count, +0xC spawns, +0x10 u16 spawn
  count); exit records 0x24 bytes, dest map/submap/entrance at +0x1E/20/22.
- Text encoding: 0x00-0x19 = a-z, 0x81 upper, 0x82 lower, 0x7F space, 0xFF end.
- Analog stick lands in 0x80092871/2; `func_80003B60` is the player control
  handler (via the state table at 0x8004C230), `func_80005748` then does
  collision on position + velocity.

## Randomizer work still open

- **Combat EXP display** (Merrow's numerical EXP readout) is the one Stage 2
  option deliberately skipped. It is not an immediate tweak: Merrow rewrites 14
  separate instruction halves in the 0x02A3xx-0x02A6xx range, changing what the
  code *does*, not just a constant. Doing it here means replacing the display
  routine outright, either through `patches/` as MIPS or natively — not a hook.
- **Menu element caps** (Merrow's `elementCapLocations` 4-7, at ROM 0x02A3DF,
  0x02A3FB, 0x02A417, 0x02A433) are the "max 50" number the menu gauges show,
  in `func_80029448`. Each is a delay-slot `addiu $a3, $zero, 0x32` feeding a
  draw call, and a hook keyed to the jal fires *before* the delay slot sets the
  register, so it gets stomped. Cosmetic only, so it was left alone rather than
  guessing at the hook placement. The four gameplay gates are done.
- **Lost Keys mode** — the biggest remaining piece. Threaded through Shuffle.cs
  rather than localised: it widens the drop array from 67 to 74 to include
  bosses, changes the gift array size, adds its own boss item list, and
  interacts with the progression-door unlocks (`rndUnlockDoorsToggle` behaves
  differently for `rndLostKeysDropdown` index 1). Data-driven throughout, so it
  needs no new hooks — but it does need the surrounding shuffle logic ported
  carefully, not just a table write.
- **Enemy composition shuffle** — needs `Merrow/Util/AreaEncounterData.cs`
  (596 lines) ported: `MapData`, `RandomizeMonsterTables`,
  `RandomizeAllMonsterPresets`, the `FixBaragoonMoor`/`FixBrannochCastle`/
  `FixMammonsWorld` clamps (Brannoch and Mammon's World share pack definitions
  across submaps) and `GetMapWriteOperations`. Mechanical but voluminous; also
  data-only, so no hooks.
- Cosmetics that Merrow has and this doesn't: cloak colour, text content
  shuffle, seed digits on the title screen, the Merrow logo.

## Known gaps / ideas
- Warp could accept an explicit X/Y/Z (`D_80085370 = -1` + pos in
  `D_8007BA40..48`, as `func_8000BB68` does).
- Stat sliders write straight into the struct; battle may keep its own copy.
