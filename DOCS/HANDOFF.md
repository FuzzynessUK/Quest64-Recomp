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
- Stage 2 (not started): Merrow options that patch *code* need native hooks:
  encounter rate, MP regain, staff-hit MP, wing unlock, element uncap,
  drop limit, EXP display, Beigis map-check skip when boss order moves him.
- Stage 3: Lost Keys mode, enemy composition shuffle, cosmetics.

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

**Still to do**: the dim overlay drawn over the field when the pause menu
opens is not a rectangle. The current build logs every 4-vertex quad
(`quad x=.. y=.. rgb a=`) to `widescreen_rects.txt`; the next step is to
open the pause menu in the field, read that log, identify the overlay quad
(likely black, alpha < 255, x spanning 0..320 or -160..160) and widen its
vertices in the walker (`G_VTX` = 0x04) by the aspect factor. After that,
remove the diagnostic logging (`log_rects`) and add a graphics-menu toggle
(`zelda64::renderer::set_widescreen_2d_enabled` already exists).

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

## Known gaps / ideas
- Warp could accept an explicit X/Y/Z (`D_80085370 = -1` + pos in
  `D_8007BA40..48`, as `func_8000BB68` does).
- Stat sliders write straight into the struct; battle may keep its own copy.
