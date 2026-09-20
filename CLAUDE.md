# Quest 64: Recompiled — project notes for Claude

Static recompilation of Quest 64 (N64, US) built on N64Recomp / N64ModernRuntime
with RT64, forked from Rainchus/Quest64-Recomp. Fork: FuzzynessUK/Quest64-Recomp,
working branch `local/submodule-url-fix`. Much of the source tree still carries
Majora's Mask names (`zelda64::`, `zelda_*.h`, "mm_") from the template it was
derived from; that is expected, don't rename them.

Read `DOCS/HANDOFF.md` first: it records what has been built, what is still
open, and every reverse-engineered address the features depend on. It is the
detailed record; this file is the working guide.

## What exists

Four user-facing additions on top of the base port, all reached from the config
menu:

- **Cheats tab** — warp, stat sliders, movement speed, Get Item, and a master
  on/off that gates everything on the tab.
- **Randomizer tab** — a port of Merrow (MIT). Data-only options are ROM writes
  applied at boot; options that patch *code* are native hooks instead. Includes
  Lost Keys (both rulesets), enemy table and encounter shuffling, Shannon
  hints, and the cosmetic palettes.
- **Enhancements tab** — Fun (One Hit KO, N64 Mode) and Quality of Life (JP
  Healing Amounts, JP Magic barrier, Exit from anywhere, Speedrun Timer).
- **Reset** — a button on General and a bindable "Reset Game" control. It
  relaunches the application, because ultramodern can start a game but has no
  way to tear a running one down.

## Build and deploy

- `cmake --build build-cmake --target Quest64Recompiled` (Ninja + clang-cl;
  on this machine cmake is under Visual Studio's CMake folder and is not on
  PATH, and the build must run inside `vcvars64.bat` or clang-cl cannot find
  the Windows SDK).
- After changing a hook in `us.rev0.toml`, run `N64Recomp.exe us.rev0.toml`
  first to regenerate `RecompiledFuncs/` (git-ignored), then build.
- **The game runs from `D:\Games\Quest64Final\`**, not from `build-cmake/`.
  After every build copy `build-cmake/Quest64Recompiled.exe` there, plus any
  changed files under `assets/`. Check with
  `diff -rq assets D:/Games/Quest64Final/assets`.
- **RML and RCSS are loaded from `<exe dir>/assets` at runtime**, so a
  menu-text or layout change needs only the asset copied — no rebuild.
- If the exe will not copy ("Device or resource busy"), the game is running.
- Settings and logs: `%LOCALAPPDATA%\Quest64Recompiled\`.
- Game-side C++ that touches RDRAM uses the `MEM_W/MEM_H/MEM_B` macros from
  `recomp.h` and needs a parameter named `rdram` in scope.

## Layout

- `src/game/debug.cpp` — cheats, the queued map warp, and the per-frame hook
  `quest64_cheats_frame` on `func_80026658`, which also drives the enhancements
  and the speedrun timer.
- `src/game/enhancements.cpp` / `include/enhancements.h` — Enhancements tab.
  ROM-level changes in `apply_at_boot` (called after the randomizer so the two
  stack), RAM-level ones in `on_frame`. Settings in `enhancements.json`.
- `src/game/speedrun.cpp` — run timer. Starts when the title overlay stops
  running, stops on a hook at the Mammon defeat message. The overlay is
  `assets/speedrun_timer.rml`, its own RmlUi context with input and mouse
  capture off so it draws without swallowing anything.
- `src/game/randomizer/` — Merrow port. **Both** `merrow_data.cpp` and
  `merrow_mapdata.cpp` are GENERATED (`tools/convert_merrow_datastore.pl` and
  `tools/convert_merrow_mapdata.pl`); never edit them by hand. Applied to the
  in-memory ROM at boot via `quest64_on_init` in `src/main/main.cpp`.
- `src/game/randomizer/native_hooks.cpp` — the randomizer options that patch
  code rather than data. Each hook writes a register in the window between the
  instruction that sets it and the one that uses it.
- `src/game/map_table.cpp` — map/submap/entrance tables extracted from the ROM.
- `src/game/widescreen.cpp` — display-list post-processor hooked in
  `nnScExecuteGraphics` before `osSpTaskLoad`; extends 2D fades/backdrops with
  RT64's extended GBI. F3DEX 1.23 opcodes.
- `src/ui/ui_config.cpp` + `assets/config_menu/*.rml` — config tabs. Cheats,
  Randomizer and Enhancements use the debug-tab styling (`config-debug*`).
  `assets/recomp.rcss` is compiled from `assets/scss/`; edit both.
- `patches/` — game-side C compiled to MIPS (RECOMP_PATCH). Separate toolchain,
  see `patches/Makefile`; it includes libultra headers from
  `../../reference/sf64decomp`.

## Conventions that bit us

- **Look in `Quest64Syms/*.toml` before reverse engineering anything.** Three
  separate features were solved by a named symbol that was there all along
  (`gBattleState`, `gInventory`, `gMsg_Shannon_Mammon_has_been_destroyed`).
- **`gGameMode` is not a battle indicator.** It is written with 2 in one place
  and 4 in another, so "1 means field" does not hold. A battle is running when
  bit 0 of `gBattleState` (0x8008C592) is set — `func_8001C5F4` tests exactly
  that before counting down to the next encounter.
- `apply_map_warp` leaves a request **queued** when the game is not ready, so
  it fires later rather than being refused. Anything that should be "now or
  never" must pass `drop_if_busy`.
- The recomp never executes ROM code: ROM patches that change instructions do
  nothing; those need native hooks. Conversely, values the game keeps in RAM
  cannot be patched in the ROM at all and need `on_frame`.
- Hook functions named in `us.rev0.toml` are implicitly declared in the
  generated C; define them `extern "C"` and rerun N64Recomp.
- RmlUi: a document needs `rml.rcss` and `recomp.rcss` linked or it has no base
  styles, and only `latolatin` and `promptfont` are registered faces. A made-up
  `font-family` renders nothing.
- RmlUi range inputs fire `change` while a document loads (a `min="1"` slider
  clamps its initial 0), so setters bound to game memory must ignore changes
  until the game has reported real values.
- RT64: a real extended scissor changes the frame's detected aspect and
  stretches everything; use `gEXSetScissorAlign` with a `-320` right offset
  (Zelda64Recomp's pattern) around extended draws instead.
- Don't extend the game's inset frame clear (`8..311`); it paints the clear
  colour into the border the 3D viewport never covers.
- Line endings: most sources are CRLF, a few (`zelda_debug.h`, `debug.cpp`,
  `cheats.rml`, `general.rml`, `CLAUDE.md`) are LF. Check with `file` first.
- Editing with perl: `\Q...\E` does **not** interpret `\r\n`, so build the
  search and replacement in variables first. `${CMAKE_SOURCE_DIR}` and friends
  interpolate as perl variables — use the Edit tool for `CMakeLists.txt`.

## Comparing the US and Japanese ROMs

Eltale Monsters (JP) is the source of several enhancements, and the two builds
*can* be compared despite having different layouts:

1. The JP dump is byte-swapped (.v64). Swap each 16-bit pair to get big-endian
   before doing anything.
2. Exact byte matching fails, because data moved by 0x25B8 and every
   `lui`/`addiu` holding an address differs. Mask I-type immediates — keep the
   opcode and registers, drop the low 16 bits — and functions line up.
3. Aligning on a function's *first* 16 instructions misses any function whose
   opening changed; slide the window through the function to find an anchor.

This lines up roughly two thirds of the game and makes "what did JP change
here?" answerable. `DOCS/HANDOFF.md` records the findings, including the JP
spell table offset and why the stat-gain sparkle effect cannot be ported by
injecting a call.

## Reference material

- `Quest64Syms/dump.toml` and `data_dump.toml` — function/data symbols.
- Merrow randomizer (MIT): github.com/hangedmandesign/merrow, checked out at
  `../reference/merrow`.
- Zelda64Recomp `patches/ui_patches.c` — reference for RT64 extended-GBI use.
