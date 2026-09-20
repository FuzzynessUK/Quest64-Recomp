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
- Stage 4 (2026-09-19, **untested**): 16 more data-only options, all pure ROM
  writes with no new hooks. Cosmetics: cloak colour, Brian's clothes, spell
  palettes, background music shuffle. World/QoL: fast Shamwood, fast Mammon's
  World, unlock progression locks, gem-locked endgame, Crystal Valley return,
  Brannoch return, lock Mammon's back door, restless NPCs, max message speed,
  HUD lock, Celtland drift, base spells at level 2. Text and staff palettes
  (Stage 3) are confirmed working in game.
- **The "still open" list below was incomplete.** Comparing our Options struct
  against Merrow's toggle list showed ~25 unported options, not the handful
  recorded. Checking which `data::` tables `randomizer.cpp` never references
  is a quick way to re-derive what is left.
- Stage 5 (2026-09-19, **untested**): reveal hidden spirits, useful Dew Drop,
  zoom out (1-4), Ivory Wings, text improvements. Corrections to the notes
  above while porting these:
  - `translate_string` in `randomizer.cpp` **already implements** Merrow's
    `TranslateString`, and gifter/wingsmith text already uses it. The "text
    cluster is blocked on the encoder" note was wrong.
  - **Shannon hints are a Lost Keys feature.** Merrow only ever assigns
    `hints[]` inside the two Lost Keys branches of `Shuffle.cs`; with Lost
    Keys off it stays all zeros, so a standalone port would emit the same
    index-0 hint for every gem. It has to wait for Lost Keys.
  - Merrow's region-aware gem placement (the `area_*` tables) is **also**
    Lost Keys only. Outside it Merrow shuffles all 67 drops uniformly, gems
    included, exactly as this port does, so the two match. Unreachable gems
    are what "unlock progression locks" is for.
- Stage 6 (2026-09-19, **untested**): **Lost Keys**, both rulesets, plus the
  Shannon hints and Lost Keys intro texts it unblocks.
  - `place_lost_keys()` ports Shuffle.cs: Progressive keeps each gem inside its
    own region (`area_earth`/`area_wind`/`area_water_nowings`/`area_fire`/
    `area_book`, or `area_bookf_beigis_nowings` with Fire Book); Open World
    uses one 104-slot pool via `area_open_beigis`. Drops widen from 67 to 74 so
    boss items shuffle in, then split back out into `lk_boss_items`.
  - `patch_lost_keys()` writes the door changes and fire gate (Progressive
    only, i.e. when progression locks are not already open), the boss items
    when boss-order shuffling is not already writing them, and the hints.
  - **Lost Keys coerces other options**, as Merrow's UI does by force-checking
    and disabling them: chests, drops and gifts on, wingsmiths off, Ivory
    Wings on. Without this a gem can land in a list that is never written and
    become unobtainable. The coercion is in `generate()` on a copy, so the
    saved settings are left alone.
  - Two Merrow bugs were not reproduced. Its non-boss-order Lost Keys path
    indexes a six-entry address table with seven bosses (out of range at
    Beigis); the addresses come from `dropdata` instead, which is where Merrow
    reads them on its other path. And `bookhints`/`bookfhints` each stop one
    entry short of the range its own roll produces, so the hint lookup is
    bounds-checked and falls back to the last wording.
- Stage 7 (2026-09-19, **untested**): **enemy shuffling**, both options.
  - The encounter data Merrow keeps as nested C# objects is now extracted by
    `tools/convert_merrow_mapdata.pl` into `merrow_mapdata.cpp`/`.h` (6 enemy
    tables, 189 packs, 72 regions, 27 areas). Like `merrow_data.cpp` it is
    GENERATED; never edit it by hand. Areas that share a pack array in Merrow
    get their own copy, matching its AreaEncounterData.Copy().
  - `shuffle_enemies()` / `patch_enemies()` port Util/AreaEncounterData.cs:
    table shuffling swaps which roster each area draws from (wrapping pack
    enemy ids into the new table's size), composition shuffling rerolls each
    region's seven presets and each pack's enemies. Baragoon Moor, Brannoch
    Castle and Mammon's World then get their ids capped to the smallest table
    in the group, because their submaps share pack addresses.
  - **The extraction was verified against the ROM**: all 189 packs (798 words)
    and all 27 area headers (162 words) match byte for byte. 70 of 72 regions
    match; the two that do not (0x85323C and 0x85329C) have preset lists that
    look swapped with each other in Merrow's data. Region data is therefore
    only written when composition shuffling actually changed it, so the bad
    pair is never propagated. Re-run the ROM comparison if the data is ever
    regenerated.

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



### Enhancements tab (2026-09-20)

Separate from the randomizer: `include/enhancements.h`, `src/game/enhancements.cpp`,
`assets/config_menu/enhancements.rml`, settings in `enhancements.json`. ROM-level
changes are applied in `apply_at_boot`, called from `quest64_on_init` *after* the
randomizer so the two stack; RAM-level ones run from `on_frame`, called by the
cheats frame hook.

- **For Fun / One Hit KO** — every monster and boss row's HP halfword (and its
  duplicate two bytes later) is set to 1 in the ROM, and Brian's max HP is held
  at 1 each frame so it also applies to a save already in progress. HP is only
  ever lowered, never raised, so a death in progress is not undone.

#### JP Buffs + Debuffs: what was found, and why it is not done

The Japanese release (Eltale Monsters) gives Magic Barrier, Silence,
Restriction, Evade, Confusion and Slow Enemy longer durations. Investigated
2026-09-20 with both ROMs in hand:

- The JP ROM is byte-swapped (.v64); swap it to big-endian before comparing.
- The JP spell table is at ROM **0xD494A8** (US: 0xD4BA60), same 60 entries,
  same 68-byte stride, same order.
- **Diffing all 60 spell blocks gives exactly one differing byte in the whole
  table**: Healing Lv2 at +0x0D (0x08 -> 0x10). The durations are therefore
  *not* in the spell data.
- Byte +0x3C is a small 1-4 value but only on Spirit Armor, Weakness and Weaken
  All, i.e. the stat-modifier spells, and is zero on all six spells above. It is
  not the status duration.
- The two versions' code does not line up at all (97% of the boot segment's
  words differ, and no US function's bytes appear anywhere in the JP ROM), so
  there is no shortcut diff at instruction level.

Conclusion: the durations are constants in the battle status code. Because the
recomp bakes instruction immediates into `RecompiledFuncs/` as C literals, this
needs a native hook like the Stage 2 randomizer options, after locating where a
status effect's turn counter is initialised.

**Update after a full US/JP code comparison (2026-09-20).** The ROMs *can* be
compared, despite an earlier note to the contrary. Exact byte matching fails
because data moved by 0x25B8 and every lui/addiu holding an address differs, but
masking out I-type immediates (keep opcode+registers, drop the low 16 bits)
aligns functions fine. `tools/` has no script for this; the throwaway one lived
in the scratchpad, but the method is:

- mask each word: R-type/COP keep whole, J/JAL keep opcode, I-type keep top 16 bits
- index every 16-instruction window of the JP ROM by masked signature
- look each US function up by the signature of its first 16 instructions

Results: **440 of 669 US functions align**, and across all of them only ~50
immediates in value-carrying opcodes (addi/addiu/slti/andi/ori) differ, none of
them duration-shaped. Meanwhile most functions in the **0x8003xxxx-0x8004xxxx
battle range did not align at all**, i.e. they were substantially rewritten
between versions.

So the JP buff/debuff behaviour is **not a constant that can be patched**: it is
different battle logic. Reproducing it means reimplementing the behaviour with
native hooks after working out the US status system, which is a much larger job
than a data or immediate patch. Ruled out by data: the spell table (identical),
byte +0x19 (set on damage spells like Power Staff and Fire Pillar too, so not a
duration) and byte +0x3C (only on Spirit Armor/Weakness/Weaken All, and behaves
like magnitude, since Weakness Lv2 is *lower* than Lv1).

**Second pass (2026-09-20), after sliding-window alignment.** Aligning only on a
function's *first* 16 instructions misses any function whose opening changed.
Sliding the 16-instruction window through each function finds an anchor in most
of the battle range too, so nearly all of the game can be compared. Three
candidate leads were chased and all eliminated:

- `func_80028624 +0x4A4/+0x4D4/+0x4E4` (1->12, 1->7, 3->7): calls into
  `func_80029B58` with sprite ids and coordinates. UI layout, not durations.
- `func_8002FD90 +0x0F4/+0x0FC` (6->10, 6->12): same shape, UI.
- `func_800386D0`, byte field +0x98, decremented once per call: this is an
  RLE repeat count in a script/bytecode interpreter (note the 0xFE escape byte
  and the advancing pointer at +0x58), not a status turn counter.

Merrow documents only two spell fields (+3 "spell rule", +11 "range"), nothing
that covers this.

**What would actually finish it.** Static analysis has run out of road; the next
step is runtime observation, which is cheap with an emulator:

1. In BizHawk (already installed with a Quest 64 US ROM), cast Silence or
   Restriction in battle and RAM-search for a value that counts down once per
   turn.
2. That gives the counter's RDRAM address and the battle actor's struct offset.
3. Search `RecompiledFuncs/` for the instruction that writes that offset; that
   is the duration being set, and it becomes a native hook exactly like the
   Stage 2 randomizer options.

Until then the Enhancements tab deliberately has no toggle for it, rather than
one that silently does nothing.

#### Kill Brian: removed 2026-09-20

The cheats tab briefly had a Kill button that set HP to zero. It never reliably
killed: nothing outside of taking damage appears to check for death, and
`gPlayerMainData` HP is only *read* by HUD code in the recompiled output (battle
damage reaches it through a pointer, so the writer cannot be found by address).
Removed at the user's request rather than left as a button that half worked.
With One Hit KO on, any hit kills Brian anyway.


#### The spell effect parameter block (found 2026-09-20)

Bytes **+0x34..+0x3F** of a 68-byte spell entry are a per-effect parameter
array: each status effect owns one slot, and a spell writes only its own.
Mapped from the 60 US entries:

| offset | effect | values |
| --- | --- | --- |
| +0x34 | Power Staff | 1, 2 |
| +0x35 | Restriction / Ice | 1, 2 |
| +0x36 | Slow Enemy, Wind Walk | 1, 2 |
| +0x37 | Evade | 1, 2 |
| +0x38 | Silence | 1 |
| +0x39 | Soul Searcher | 1 |
| +0x3A | **Magic Barrier** | 1 |
| +0x3B | Confusion | 1 |
| +0x3C | Spirit Armor / Weakness / Weaken All | 1-4 |
| +0x3E, +0x3F | Magnet Rock, Ice Wall, Cyclone, Wind Bomb | 1-3 |

The values track spell level where a spell has two (Evade 1/2, Spirit Armor
3/4), which is what makes this look like the effect's turn parameter.

**The +0x3A theory was wrong.** Writing to Magic Barrier's slot was tested
in game on 2026-09-20 and had no effect at all, so the option was removed
rather than left as a dead control. The parameter block above is still a
real structure, but +0x3A is not the barrier duration. Also ruled out:
+0x2F (3 on Fire Ball too) and +0x31 (3 on Power Staff and Homing Arrow),
so neither is status-specific.

The ROM patch mechanism itself is fine: `get_rom()` and
`set_rom_contents()` share one buffer, so the randomizer's and the
enhancements' passes compose correctly.

Searches that came up empty for the barrier timer: no code reads a spell
entry at +0x3A; no load/decrement/store of a byte or halfword field in the
0x8002C000-0x80060000 battle range except the RLE counter in
`func_800386D0`; and the near-identical US/JP battle functions differ only
in relocated pointers and jal targets, not constants. The timer is
reached through a computed offset, so it needs the runtime RAM search.

#### JP Healing Amounts

The single byte that differs across the whole 60-entry spell table between
the US and Japanese ROMs is **Healing Lv2's potency**, the halfword at entry
+0x0C: **8 in US, 16 in JP** (ROM 0xD4C50C). Healing Lv1 is 2 in both. The
toggle writes 16, so unlike the Magic Barrier option this is an exact match
for JP rather than an approximation.

#### Exit from anywhere

A Quality of Life toggle, with no menu button: it enables the bindable
**"Exit Spell"** control under Controls, which warps to the start of the
current area without owning the spell or paying the MP. `cast_exit()`
returns immediately when the toggle is off, so an unbound or disabled
setup does nothing. Two supporting changes:

- `do_map_warp` gained a `from_cheats` flag, so the cheats master switch
  only drops warps that came from the cheats tab.
- `gCurrentMap` (0x80084EEC) is cached into an atomic each frame, like the
  stat readouts, so the menu thread never touches RDRAM.

It targets submap 0, entrance 0 of the current map. That is an assumption
about where an area starts, not something read from the game's own Exit
handler, which was not located.


#### Magic Barrier +2 turns (working, 2026-09-20)

The duration is a **live value at RAM 0x8007BB42** (a byte; the address came
from the user as a GameShark-style code). It is reached through a pointer:
nothing in the recompiled output reads or writes it by absolute address, and
the only struct-relative access at the matching offset (+0xC2) is a read in
func_80002F60. So there is no instruction to hook and no ROM byte to patch,
which is why every earlier attempt through the spell table failed.

Handled in `on_frame` instead. The counter only rises when the spell is
cast and falls a turn at a time afterwards, so the bonus is applied on a
rise, which adds it once per cast rather than every frame.

#### Exit: still a warp, not the real spell

The request is for the genuine Exit spell to fire without owning it or
paying MP. Not done. The spell effect handlers do live in a jump table at
ROM 0x04D8D0 (41 entries of ascending 0x8001xxxx addresses), but the index
is not the spell entry's +0x01 byte: that field is a per-element ordering
value and collides (Compression and Exit are both 0x13). Even with the right
index, the handlers take a spell-cast context in $a0 and a target in $a1,
so they cannot be called cold from the frame hook without building that
context. What exists today warps to submap 0, entrance 0 of the current map.


#### JP stat-gain effect: the routine is absent from the US ROM

Eltale Monsters flashes a colour when a stat rises (red HP, blue MP, yellow
DEF, green AGI); the US release has nothing. Investigated 2026-09-20.

`func_80002F60` is the stat handler on both sides and they align, but JP has
an extra call the US build does not, inserted around +0x17C..+0x1B4. Of the
three functions JP calls there:

- JP 0x8000D100 = US `func_8000B740` (US already calls it)
- JP 0x8000D40C = US `func_8000BA4C` (US already calls it)
- **JP 0x80014318 has no counterpart anywhere in the US ROM**

The third is the effect, and a masked-signature search over the whole US
code region does not find it. So this cannot be done by injecting a call the
way a Stage 2 hook does: the routine to call does not exist. Recreating it
means either writing the effect as MIPS in `patches/` against the game's own
effect system, or drawing it ourselves through the display-list injection
already used by `src/game/widescreen.cpp`.

Detection is the easy half: `quest64_cheats_frame` already reads every stat
each frame for the cheats readout, so spotting a rise is trivial.


**Porting it is feasible, and here is what it needs.** The JP routine at
0x80014318 (JP ROM 0x14F18) is an effect updater: float and matrix work over
a table of entries, using JP globals around 0x80085AB0-0x80085AC0. It calls
two helpers, and **both exist in the US build**:

- JP 0x80038500 = US `func_80034F60`
- JP 0x800271B4 = US `func_800232F4`

So only the wrapper is missing, not the machinery under it. A port would
mean transcribing that routine (and whatever creates its entries) against
the US helper addresses, with our own storage in place of the JP globals,
then triggering it from the stat-up site in `func_80002F60` with a colour
per stat. `patches/` already compiles MIPS into this build, so the .nrm mod
format is not required: it is the same capability with extra packaging.

### Merrow branding: deliberately not ported (decided 2026-09-19)

Merrow replaces the title-screen logo and can stamp the seed digits over the
menu background. **Do not port this.** The tables are present in the generated
data but must stay unreferenced:

- `merrowlogo`, `merrowlogostatic`, `randologo` — title/animation logos
- `menubg`, `icondigits` — menu background and the seed-digit overlay
- `gemtexture` — only used by Merrow's own title art

This is a project decision, not an oversight, so a future pass should not treat
these as missing features. Everything else in "Randomizer work still open"
below is still fair game.

### Hard Mode (Landmine36's hack) — DONE 2026-09-20

Enhancements tab, "Hacks" group. Applied at boot from the embedded `.rup`
(`tools/hardmode/Quest64.HM.0.9.8.3A.rup`); restart to change. Uses its own
save folder (`saves/hardmode/`) and overrides the randomizer while on.
`include/hardmode.h` has the design summary; the short version:

- The hack = ~60 KB of data records + ~60 code-segment patch sites (40 are
  `j` into new code) + a 118 KB payload at ROM 0x1000000 → RAM 0x80400000
  (64 KB text, then asm shims, C code and tables). Needs the Expansion Pak
  on console; the recomp has 8 MB anyway.
- `src/game/hardmode/hardmode.cpp` parses the .rup, patches the ROM in
  memory (mirroring boot-segment writes into RAM, as the randomizer does) and
  copies the payload to RDRAM.
- `src/game/hardmode/recompiled/payload.c` is the payload's C code,
  recompiled by N64Recomp out of the patched ROM with a hand-built symbols
  section (`tools/hardmode/recompile_payload.pl`; function extents in
  `payload_funcs.txt` came from control-flow reachability over the
  disassembly). Two `teq` instructions are nop'd by instruction patches.
- `src/game/hardmode/hooks.cpp` reproduces every code-segment patch site
  as a native hook (all in `us.rev0.toml`, gated on `hardmode::active()`).
  Function-entry `j`s call the recompiled function and `return;` out of the
  generated body; the asm shims are re-done on the register context.
- Facts learned: the payload's `0x80410658` check is a **night** test on
  `gCurrentTime` (0x800859D0): outside 0x1600..0x5400 monsters get x1.5
  stats and x2 exp/stones. Save-flag bytes at 0x800869F0..F8 carry the
  hack's special items (exp doublers), second-quest boss flags and the
  encounter-rate flag. A hook cannot sit on a delay slot (N64Recomp emits it
  inside the call), hence the death-jingle hook at `UpdateBGM`'s entry.
- Not verified in-game yet: the night multipliers, level-up screen digits,
  boss rematch flag redirection, the Mammon message swap.

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
- **Lost Keys mode** and **enemy composition shuffle** are DONE (stages 6 and
  7, 2026-09-19). The entries that used to sit here are obsolete.
- Cosmetics still missing: text content shuffle and vowels. Cloak colour is
  done (stage 4); the logo and seed digits are deliberately not ported.

## Known gaps / ideas
- Warp could accept an explicit X/Y/Z (`D_80085370 = -1` + pos in
  `D_8007BA40..48`, as `func_8000BB68` does).
- Stat sliders write straight into the struct; battle may keep its own copy.
