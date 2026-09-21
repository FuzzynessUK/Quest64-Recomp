# Quest 64: Recompiled — project notes for Claude

Static recompilation of Quest 64 (N64, US) built on N64Recomp / N64ModernRuntime
with RT64, forked from Rainchus/Quest64-Recomp. Fork: FuzzynessUK/Quest64-Recomp,
working branch `local/submodule-url-fix` (pushed to remote `fork`; `origin` is
Rainchus and is never pushed to). Much of the source tree still carries
Majora's Mask names (`zelda64::`, `zelda_*.h`, "mm_") from the template it was
derived from; that is expected, don't rename them.

Read `DOCS/HANDOFF.md` first: it records what has been built, what is still
open, and every reverse-engineered address the features depend on. It is the
detailed record; this file is the working guide.

## What exists

Four user-facing additions on top of the base port, all reached from the config
menu (F5 Cheats, F6 Randomizer, Enhancements tab):

- **Cheats tab** — warp, stat sliders, Get Item, and a master on/off that
  gates everything on the tab.
- **Randomizer tab** — a port of Merrow (MIT). One On/Off toggle at the top;
  everything else on the tab is hidden (`data-if="rnd_mode == 1"`) while it
  is off. Note "Shuffle final Shannons" is Merrow's checkbox with Merrow's
  meaning: **On excludes** the final two Shannons from the gift shuffle. Data-only options are ROM writes
  applied at boot; options that patch *code* are native hooks instead. Includes
  Lost Keys (both rulesets), Shannon hints, the cosmetic palettes, Merrow's
  table/stat shuffles, and our own **Enemy Randomizer** (below).
- **Enhancements tab** — two groups:
  - *Quality of Life*: JP Healing Amounts, JP Magic barrier (+2 turns), JP
    Stat Up Effect, Show spell learnt (`src/game/spellnotice.cpp`, overlay
    posting to the notification stack: watches the four element bytes and compares
    against the unlock level halfword at each spell entry in the patched
    ROM), Exit from anywhere, Speedrun Timer (one select: Off +
    six screen positions), Faster walking.
  - *Fun*: One Hit KO, N64 Mode.
  No description paragraphs: every option label has a hover tooltip instead
  (below). A "Changed settings apply when the game is next launched" line
  appears only once something on the tab has been changed; the Randomizer
  tab does the same.
- **Audio tab** (2026-09-21) — `src/game/audio.cpp`, `include/audio.h`,
  settings in `audio.json`, applied last in `quest64_on_init`, every draw
  fresh per launch (`std::random_device`). The "Sound" tab (volumes) is the
  template's and is separate. Options:
  - *Background Music* Off/Towns/All. Towns is Merrow's music shuffle moved
    out of the randomizer (the 73 track bytes of the map music table at
    ROM 0x054700, read by `func_8002684C`), so it works in any mode. All
    adds the tracks the game starts by number - title 0x1B, battle 0xD,
    bosses (0x8001CA28), death jingle 0x1E, credits 0x14 - by remapping a0
    in hooks on `UpdateBGM` (shared with Hard Mode's death-jingle hook) and
    `func_800267F8`, one random track per source track per session. Tracks
    are 0-41 with 27-30 unused.
  - *Shuffle sound effects*: a permutation of the 70 real effect ids (the
    per-id volume table at 0x80053CAC has 84 entries, the last 14 zero)
    applied in a hook on `func_80025B8C`, the routine every effect ends in
    (direct, queued through `func_800268D4`, or the title overlay's own
    calls) with the id in a0.
- **JP Stat Up Effect** (2026-09-21, awaiting play-test) — the Eltale
  Monsters colour burst on a stat rise, which the US ROM has no routine for
  (HANDOFF "JP stat-gain effect"), drawn by us instead. `src/game/statfx.cpp`
  (game thread, from the cheats frame hook): watches max HP/MP, DEF, AGI in
  gPlayerMainData, treats a rise of 1..30 while in the field with a map
  loaded as an event, and every frame projects Brian (gPlayerData1
  0x8007BACC +0/4/8, plus 22 units for his head) through the game's own
  float camera matrices (view 0x80086E88, projection 0x80086E48, the
  outputs of the only guLookAtReflectF/guPerspectiveF call site,
  func_80012Cxx; libultra row-vector convention, clip = v*V*P). The UI
  thread (`recompui::update_stat_effects` in `ui_config.cpp`, called next to
  the timer update in `ui_state.cpp`) owns a draw-only context on
  `assets/stat_effects.rml` and sprays 18 `.spark` divs per rise between
  feet and head, drifting up and fading over about 0.8 s, repositioned every
  frame so they follow Brian. NDC to window: the 4:3 image is the full
  window height, centred. Brian's height (22) and the burst numbers are
  guesses to tune on sight.
- **Tab bar**: ten tabs no longer fit at header size, so the config menu's
  tabs are 26dp and the row starts 48dp down, clear of the quit/close
  buttons (`Tabs.scss`, mirrored in `recomp.rcss`).
- **Hard Mode** — Landmine36's hack (the .rup is embedded and applied at
  boot, its payload recompiled, its code patches redone as native hooks; own
  save folder; overrides the randomizer). Switched from the **Mods tab**,
  where it is a built-in entry pinned to the top of the mod list
  (`ui_mod_menu.cpp`, id `quest64-hard-mode`); the setting is still
  `hard_mode` in `enhancements.json`, reached through
  `recompui::is/set_hard_mode_enabled`. It was a Hacks group on Enhancements
  until 2026-09-21.
- **Tooltips** on the Randomizer and Enhancements tabs: a label carries
  `data-event-mouseover="show_tip" data-event-mouseout="hide_tip"` and a
  hidden `<div class="config-tip-text">`; `bind_tooltip_events` in
  `ui_config.cpp` copies the text into the tab's one floating
  `<div class="config-tip">`, which sits outside the scroll container so it
  is never clipped, below the label or above it in the lower half. The
  Randomizer texts are Merrow's own tooltips, reworded to our option names;
  starting stats and cosmetics have none on request.
- **Reset** — a button on General and a bindable "Reset Game" control.
- Tab order (`config_menu.rml` and `config_tab_to_index`): General, Controls,
  Graphics, Sound, Mods, Cheats, Randomizer, Enhancements, Audio, Debug. It
  relaunches the application, because ultramodern can start a game but has no
  way to tear a running one down.

## What we have been working on (2026-09-18 → 09-21)

Chronologically, newest last. All committed and pushed unless noted.

1. **Speedrun timer** — overlay in its own RmlUi context; starts when the
   title overlay hands over, stops on the Mammon-destroyed message.
2. **Hard Mode port** (`a21d884`) — user play-tested and confirmed fine.
   Regenerate the payload with `perl tools/hardmode/recompile_payload.pl` if
   Landmine ships a new `.rup`.
3. **Enemy Randomizer** (`c2cf68f` … `387bee5`) — the big one. Design lives
   in `DOCS/enemyrandologic.xlsx`, generated by `tools/enemyrandologic.pl`
   (which also generates `enemy_progression_data.cpp`, so sheet and code
   cannot drift). Also copied to `D:\Downloads\enemyrandologic.xlsx` for the
   user. Final logic:
   - Eight tiers in story order (T1 Holy Plains/Connor … T8 Mammon's World).
   - Placement is per *monster set* (the six monster files), any set in any
     area, previous area's set skipped when there is a choice; packs rerolled
     with Merrow's composition logic. Files are the unit because every file
     DMAs to the same RAM base (file table 0x80054160), so a roster can never
     mix files.
   - Stats are rescaled **in RAM when the file loads** (hook after the DMA in
     `func_80008EF4`, `before_vram 0x80008F5C`, keyed on gCurrentMap
     0x80084EEC with a gNextMap fallback):
     `new = AreaAvg(dest) × (own / AreaAvg(home))^0.5 × guard(dest)`,
     guard = budget / avg_power with budget never falling along the story.
     Caps HP 999, ATK/DEF/AGI 255, EXP 65535. Applies to HP (both halfwords),
     AGI, DEF, ATK, EXP and Stones in the 0x38-byte table entry.
   - **No separate damage scaling.** The base handed to `func_80006BEC`
     already follows the attacker's ATK; a damage hook double-counted
     (Were Hare for 592) and was removed. Brian's own damage is untouched.
   - Bosses are out of scope (ids 67+). Hard Mode overrides all of it.
   - One switch, `enemy_randomizer` in `randomizer.json` (loads the old
     `enemy_progression` key too). Sits at the top of the Monsters group.
   - Spoiler at `%LOCALAPPDATA%\Quest64Recompiled\randomizer_spoiler.txt`
     lists every monster's vanilla → scaled stats per area.
4. **Menu tidy-up** (`6ffd3c8` … `fbd5ec4`) — Enhancements reordered, timer
   collapsed to one control (settings on disk unchanged: `speedrun_timer` +
   `timer_position`), "Text improvements" label shrunk to 16dp because the
   word IMPROVEMENTS alone is wider than the 196dp label column.
5. **Faster walking** (`89bd879`, reworked 2026-09-21) — now the mechanism the
   old Movement speed cheat used, fixed at 150% (3.0 a step, exactly the cap): the velocity every movement
   state hands to `func_80005748` (player struct +0x18/+0x20, a1) is scaled
   on entry and divided back before the routine's single `jr $ra` at
   0x80005A08, 3-unit step cap kept. The restore is the point: the cheat left
   the scaled value in the struct, the walk handler's 0.2 lerp read it back
   and ran away to the cap (so "140%" was really 150%), and the skid's
   0.68-a-frame decay became 0.95 — the long slide the user reported. With
   the restore the slide keeps its vanilla 8 frames and covers 1.5x the
   distance, like Hard Mode's 2.75 pace. Off under Hard Mode. The cheat and
   the 0x8000541C/0x80005638 walk hooks are gone. Awaiting play-test.

Nothing is currently half-done. Open items are in `DOCS/HANDOFF.md`
("Randomizer work still open", "Known gaps / ideas").

## Build and deploy

- `cmake --build build-cmake --target Quest64Recompiled` (Ninja + clang-cl;
  on this machine cmake is under Visual Studio's CMake folder and is not on
  PATH, and the build must run inside `vcvars64.bat` or clang-cl cannot find
  the Windows SDK). A wrapper batch file lives in the session scratchpad
  (`build.bat`: call vcvars64, cd to the repo, run cmake); recreate it if the
  scratchpad is gone.
- After changing a hook in `us.rev0.toml`, run `./N64Recomp.exe us.rev0.toml`
  first to regenerate `RecompiledFuncs/` (git-ignored), then build. Check the
  emitted call landed where you meant with `grep -B3 -A3 <hook> RecompiledFuncs/funcs_*.c`.
- **The game runs from `D:\Games\Quest64Final\`**, not from `build-cmake/`.
  After every build copy `build-cmake/Quest64Recompiled.exe` there, plus any
  changed files under `assets/`. Check with
  `diff -rq assets D:/Games/Quest64Final/assets`.
- **RML and RCSS are loaded from `<exe dir>/assets` at runtime**, so a
  menu-text or layout change needs only the asset copied — no rebuild.
- If the exe will not copy ("Device or resource busy"), the game is running
  (`tasklist | grep -i quest64`). Don't launch the game yourself to test while
  the user may be playing; they test faster than scripted input anyway.
- Settings and logs: `%LOCALAPPDATA%\Quest64Recompiled\` (`randomizer.json`,
  `enhancements.json`, `randomizer_spoiler.txt`, `randomizer_hooks.txt`).
- Game-side C++ that touches RDRAM uses the `MEM_W/MEM_H/MEM_B` macros from
  `recomp.h` and needs a parameter named `rdram` in scope.
- No Python on this machine; perl is. The xlsx is written as a raw XML zip by
  perl (`IO::Compress::Zip`, `fullCalcOnLoad`), no spreadsheet library.

## Layout

- `src/game/debug.cpp` — cheats, the queued map warp, and the per-frame hook
  `quest64_cheats_frame` on `func_80026658`, which also drives the enhancements
  and the speedrun timer.
- `src/game/enhancements.cpp` / `include/enhancements.h` — Enhancements tab.
  ROM-level changes in `apply_at_boot` (called after the randomizer so the two
  stack), RAM-level ones in `on_frame`, and the Faster walking hooks at the
  bottom. Settings in `enhancements.json`; the tab reads `active_options()`
  (the boot copy), so changes apply on the next launch.
- `src/game/hardmode/` — Hard Mode. `hardmode.cpp` applies the patch, `hooks.cpp`
  is one native hook per code-segment patch site (registered in `us.rev0.toml`),
  `recompiled/payload.c` is GENERATED by `tools/hardmode/recompile_payload.pl`
  (needs `quest64.us.z64`, `RecompiledFuncs/funcs.h` and N64Recomp) — never
  edit it by hand. `include/hardmode.h` explains the design.
- `src/game/speedrun.cpp` — run timer. Its overlay is `assets/notifications.rml`,
  an RmlUi context with input and mouse capture off that it shares with the
  **notification stack** (`include/notify.h`, `src/game/notify.cpp`: any
  feature posts a line; `recompui::update_notifications` appends it under
  the ones showing top-left, holds 3 s, fades; the timer, when top-left, is
  the first item of that column so notices sit under it).
- `src/game/randomizer/` — Merrow port. **GENERATED, never hand-edit:**
  `merrow_data.cpp`, `merrow_mapdata.cpp` (`tools/convert_merrow_*.pl`) and
  `enemy_progression_data.cpp` (`tools/enemyrandologic.pl`). Applied to the
  in-memory ROM at boot via `quest64_on_init` in `src/main/main.cpp`.
  - `enemy_progression.{h,cpp}` — the Enemy Randomizer plan, `rescale()`,
    `scaled_value()`; constants `spread_down/up = 7`, `shape_exponent = 0.5`,
    `caps[]`, mirrored on the sheet's Settings tab (B4–B6, B21–B23).
  - `native_hooks.cpp` — randomizer options that patch code rather than data,
    including `quest64_randomizer_enemy_scale_table`. Each hook writes a
    register in the window between the instruction that sets it and the one
    that uses it.
  - `randomizer.cpp` — `shuffle_enemies()` does the set placement and pack
    rerolls; `patch_enemies()` writes the spoiler.
- `src/game/map_table.cpp` — map/submap/entrance tables extracted from the ROM.
- `src/game/widescreen.cpp` — display-list post-processor hooked in
  `nnScExecuteGraphics` before `osSpTaskLoad`; F3DEX 1.23 opcodes.
- `src/ui/ui_config.cpp` + `assets/config_menu/*.rml` — config tabs. Cheats,
  Randomizer and Enhancements use the debug-tab styling (`config-debug*`).
  `assets/recomp.rcss` is compiled from `assets/scss/`; edit both.
- `patches/` — game-side C compiled to MIPS (RECOMP_PATCH). Separate toolchain,
  see `patches/Makefile`; it includes libultra headers from
  `../../reference/sf64decomp`.
- `tools/enemyrandologic.pl` — one script writes both the design workbook and
  the progression data; edit it, then `perl tools/enemyrandologic.pl`, then
  copy the xlsx to `D:\Downloads` if the user wants it.

## Conventions that bit us

- **Look in `Quest64Syms/*.toml` before reverse engineering anything.** Three
  separate features were solved by a named symbol that was there all along
  (`gBattleState`, `gInventory`, `gMsg_Shannon_Mammon_has_been_destroyed`).
- **`gGameMode` is not a battle indicator.** A battle is running when bit 0 of
  `gBattleState` (0x8008C592) is set — `func_8001C5F4` tests exactly that.
- `apply_map_warp` leaves a request **queued** when the game is not ready.
  Anything that should be "now or never" must pass `drop_if_busy`.
- The recomp never executes ROM code: ROM patches that change instructions do
  nothing; those need native hooks. Conversely, values the game keeps in RAM
  cannot be patched in the ROM at all and need `on_frame` or a hook.
- Hook functions named in `us.rev0.toml` are implicitly declared in the
  generated C; define them `extern "C"` and rerun N64Recomp.
- **One hook per address.** Two features that need the same site go in one `text = "a(rdram, ctx); b(rdram, ctx);"`.
- A hook cannot sit on a jump's delay slot: N64Recomp emits the slot inside
  the call, so the hook lands in dead code after it. Hook the callee's entry
  or the jump itself instead. A hook's text may `return;` to skip the rest of
  the generated function.
- Float registers in `recomp_context` are a union (`ctx->f0.d` for a double
  loaded by `ldc1`, `.fl` for a float, `.u32l` for the raw word). GPRs are
  `r0..r31` (at=r1, v0=r2, a0..a3=r4..r7, t4..t7=r12..r15, s0=r16, t8=r24,
  t9=r25, sp=r29); write them with `S32(...)` for sign extension.
- Values the game computes from other values don't need a second fix: the
  enemy damage base already follows ATK, so scaling ATK scaled damage, and a
  damage hook on top double-counted. Check what a routine's inputs already
  carry before hooking it.
- Scale a table where every reader sees it (on file load), not at one consumer
  (battle set-up) — Soul Searcher read the unscaled table until this moved.
- perl one-liners: `\n` inside a replacement string in a double-quoted shell
  argument becomes a literal newline in the source. Use the Edit tool for
  anything containing escapes. A heredoc'd perl script with `\Q...\E` on
  strings built in variables is the safe way to edit CRLF files.
- RmlUi: a document needs `rml.rcss` and `recomp.rcss` linked or it has no base
  styles, and only `latolatin` and `promptfont` are registered faces. A made-up
  `font-family` renders nothing. Select labels are 196dp wide, 20dp uppercase
  with 2.8dp letter-spacing: a single word longer than ~11 letters overflows
  into the select (inline `font-size: 16dp` fixes it).
- RmlUi range inputs fire `change` while a document loads (a `min="1"` slider
  clamps its initial 0), so setters bound to game memory must ignore changes
  until the game has reported real values.
- RT64: a real extended scissor changes the frame's detected aspect and
  stretches everything; use `gEXSetScissorAlign` with a `-320` right offset
  (Zelda64Recomp's pattern) around extended draws instead.
- Don't extend the game's inset frame clear (`8..311`); it paints the clear
  colour into the border the 3D viewport never covers.
- Line endings are mixed: `ui_config.cpp`, `randomizer.rml`, `recomp.rcss` are
  CRLF; `enhancements.rml`, `enhancements.{h,cpp}`, `debug.cpp`, `cheats.rml`,
  `general.rml`, `us.rev0.toml`, `CLAUDE.md` are LF. Check with `file` first.
- Editing with perl: `\Q...\E` does **not** interpret `\r\n`, so build the
  search and replacement in variables first. `${CMAKE_SOURCE_DIR}` and friends
  interpolate as perl variables — use the Edit tool for `CMakeLists.txt`.
- Commit messages end with `Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>`.
  Push with `git push fork local/submodule-url-fix` when the user asks to
  "save to / update GitHub".

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

`DOCS/HANDOFF.md` records the findings, including the JP spell table offset
and why the stat-gain sparkle effect cannot be ported by injecting a call.

## Reference material

- `Quest64Syms/dump.toml` and `data_dump.toml` — function/data symbols.
- Merrow randomizer (MIT): github.com/hangedmandesign/merrow, checked out at
  `../reference/merrow`.
- Hard Mode source (partial, June 2024): `../reference/Quest64-Hack-HM`; the
  released `.rup` in `tools/hardmode/` is the authority.
- Zelda64Recomp `patches/ui_patches.c` — reference for RT64 extended-GBI use.
- Movement constants (func_8000534C, data at 0x80071068..): 0.025 stick
  scale, 0.4 turn lerp, 0.2 accel lerp (D_800710B0), target speed 2.0 as an
  immediate at 0x80005418. Letting go of the stick multiplies the velocity
  by 0.9 (D_800710B8) **once** and enters skid state 4 for 8 frames
  (`func_80003F98`: move, then velocity ×= 0.68, D_80070F50 — the constant
  Merrow's Celtland Drift raises above 1). State 1 is the battle-movement
  twin `func_80004E58`; every state moves through `func_80005748`.
