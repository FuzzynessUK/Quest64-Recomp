#!/usr/bin/perl
# Builds DOCS/enemyrandologic.xlsx: the design of the Enemy Randomizer option
# (as implemented), with the game's real area rosters and monster stats pulled out
# of the Merrow data (src/game/randomizer/merrow_data.cpp, merrow_mapdata.cpp)
# so the numbers in the sheet are the ROM's. No spreadsheet library on this
# machine, so the .xlsx (a zip of XML) is written directly; formulas are
# left for Excel to calculate on open (fullCalcOnLoad).
#
#   perl tools/enemyrandologic.pl
use strict;
use warnings;
use FindBin;
use IO::Compress::Zip qw(zip $ZipError);

my $root = "$FindBin::Bin/..";
my $out  = "$root/DOCS/enemyrandologic.xlsx";

# ---------------------------------------------------------------- game data
my ($map, $data);
{
    local $/;
    open my $f, '<', "$root/src/game/randomizer/merrow_mapdata.cpp" or die $!;
    $map = <$f>;
    open $f, '<', "$root/src/game/randomizer/merrow_data.cpp" or die $!;
    $data = <$f>;
}

my @tables;
while ($map =~ /\{ 0x[0-9A-F]+, \{ ((?:"[^"]*",? ?)+) \} \}/g) {
    my @e = ($1 =~ /"([^"]*)"/g);
    push @tables, \@e;
    last if @tables == 6;
}
my @packs;
my ($pk) = ($map =~ /const std::vector<MonsterPack> packs = \{(.*?)\n    \};/s);
while ($pk =~ /\{ 0x[0-9A-F]+, \{ (.*?) \} \},?\n/g) {
    my @mem;
    my $s = $1;
    while ($s =~ /\{ (\d+), (\d+), (\d+) \}/g) { push @mem, [ $1, $2, $3 ] }
    push @packs, \@mem;
}
my @raw_areas;
my ($ar) = ($map =~ /const std::vector<Area> areas = \{(.*?)\n    \};/s);
while ($ar =~ /\{ "([^"]+)", \{ (?:0x[0-9A-F]+, ){6}0x([0-9A-F]+), 0x[0-9A-F]+ \}, (\d+), (\d+), (\d+), (\d+) \}/g) {
    push @raw_areas, { name => $1, table => hex($2), ps => $3, pc => $4 };
}
die "unexpected map data" unless @tables == 6 && @raw_areas == 27;

my ($n) = ($data =~ /monsternames = \{(.*?)\};/s);
my @names = ($n =~ /"([^"]*)","[0-9A-F]+"/g);
my ($st) = ($data =~ /monsterstatvanilla = \{[^\n]*\n(.*?)\};/s);
my @stats = ($st =~ /(\d+)/g);
my ($fl) = ($data =~ /monsterflyingvanilla = \{(.*?)\};/s);
my @flypairs = ($fl =~ /"([^"]*)"/g);
my ($lc) = ($data =~ /monsterstatlocations = \{(.*?)\};/s);
my @locs = ($lc =~ /\{ *"([0-9A-Fa-f]+)"/g);
die "unexpected monster data" unless @names == 75 && @stats >= 450;

my %flying;
for (my $i = 0; $i + 1 < @flypairs; $i += 2) {
    $flying{ $flypairs[$i] } = hex($flypairs[ $i + 1 ]);
}

# Progression order and suggested tiers. Submaps that share one roster
# (Baragoon Moor, Brannoch Castle, Mammon's World) are merged.
my @progression = (
    [ 'Holy Plains',      1, 'Melrode / Dondoran start' ],
    [ 'Connor Forest',    1, 'Solvaring' ],
    [ 'Dondoran Flats',   2, '' ],
    [ 'Glencoe Forest',   2, '' ],
    [ 'West Carmaugh',    2, 'after Larapool' ],
    [ 'Cull Hazard',      3, '' ],
    [ 'East Limelin',     3, 'Normoon side; by vanilla numbers it is tougher than the T4 areas, the progression guard covers this' ],
    [ 'Windward Forest',  4, 'Zelse' ],
    [ 'Blue Cave',        4, 'Nepty' ],
    [ 'Isle of Skye',     4, '' ],
    [ 'Baragoon Tunnel',  5, '' ],
    [ 'Dindom Dries',     6, 'Shamwood side' ],
    [ 'Boil Hole',        6, 'Fargo' ],
    [ 'Baragoon Moor',    7, 'Shilf; submaps 1-2 share one roster' ],
    [ 'Brannoch Castle',  7, 'Guilty, Beigis; submaps 1-6 share one roster' ],
    [ "Mammon's World",   8, 'submaps 1-6 share one roster' ],
);

my %merge = map { ("$_ 1" => $_, "$_ 2" => $_, "$_ 3" => $_, "$_ 4" => $_, "$_ 5" => $_, "$_ 6" => $_) }
    ('Baragoon Moor', 'Brannoch Castle', "Mammon's World");

# Per area: table, roster (table slot -> name), packs as text.
my %area;
for my $a (@raw_areas) {
    my $name = $merge{ $a->{name} } // $a->{name};
    my $e = $area{$name} //= { table => $a->{table}, slots => {}, packs => [] };
    next if $e->{done};
    for my $p ($a->{ps} .. $a->{ps} + $a->{pc} - 1) {
        my @m = map {
            sprintf('%s x%d+%d', $tables[ $a->{table} ][ $_->[0] ] // "?$_->[0]", $_->[1], $_->[2])
        } @{ $packs[$p] };
        push @{ $e->{packs} }, '[' . join(', ', @m) . ']' if @m;
        $e->{slots}{ $_->[0] } = 1 for @{ $packs[$p] };
    }
    $e->{done} = 1;
}

my @monsters;    # ids 0..66 are the regular monsters; 67+ are bosses
my %home;
for my $p (@progression) {
    my $e = $area{ $p->[0] } or die "no area $p->[0]";
    for my $slot (sort { $a <=> $b } keys %{ $e->{slots} }) {
        my $nm = $tables[ $e->{table} ][$slot];
        $home{$nm} //= $p->[0];
    }
}
my %dangerous = ('COCKATRICE' => 1, 'FLAMED MANE' => 1);
for my $i (0 .. 66) {
    my @s = @stats[ $i * 6 .. $i * 6 + 5 ];
    push @monsters, {
        id => $i, name => $names[$i], home => $home{ $names[$i] } // '(unused)',
        hp => $s[0], atk => $s[1], def => $s[2], agi => $s[3], exp => $s[4], drop => $s[5],
        flying => ($flying{ $locs[$i] =~ s/4$/0/r } ? 'Y' : 'N'),
        dangerous => ($dangerous{ $names[$i] } ? 'Y' : 'N'),
        addr => $locs[$i],
    };
}

# ---------------------------------------------------------------- xlsx bits
sub esc {
    my $s = shift;
    $s =~ s/&/&amp;/g; $s =~ s/</&lt;/g; $s =~ s/>/&gt;/g; $s =~ s/"/&quot;/g;
    return $s;
}
sub col { my $n = shift; my $s = ''; while ($n > 0) { my $r = ($n - 1) % 26; $s = chr(65 + $r) . $s; $n = int(($n - 1) / 26) } $s }

# A cell is a scalar (string or number) or a hashref { f => formula } /
# { v => value, s => style } / { f => formula, s => style }.
# Styles: 0 plain, 1 header, 2 input, 3 wrap, 4 title, 5 number 0.0,
# 6 section heading, 7 formula number 0.0 (black), 8 wrap bold.
sub sheet_xml {
    my ($rows, $widths, $freeze) = @_;
    my $x = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
        '<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">';
    $x .= '<sheetViews><sheetView workbookViewId="0"' . ($rows->[0] ? '' : '') . '>';
    $x .= "<pane ySplit=\"$freeze\" topLeftCell=\"A" . ($freeze + 1) . "\" activePane=\"bottomLeft\" state=\"frozen\"/>" if $freeze;
    $x .= '</sheetView></sheetViews>';
    if ($widths && @$widths) {
        $x .= '<cols>';
        for my $i (0 .. $#$widths) {
            $x .= sprintf('<col min="%d" max="%d" width="%s" customWidth="1"/>', $i + 1, $i + 1, $widths->[$i]);
        }
        $x .= '</cols>';
    }
    $x .= '<sheetData>';
    my $r = 0;
    for my $row (@$rows) {
        $r++;
        next unless defined $row;
        $x .= "<row r=\"$r\">";
        my $c = 0;
        for my $cell (@$row) {
            $c++;
            next unless defined $cell;
            my $ref = col($c) . $r;
            my ($v, $f, $s) = ($cell, undef, 0);
            if (ref $cell) { $v = $cell->{v}; $f = $cell->{f}; $s = $cell->{s} // ($f ? 7 : 0) }
            if (defined $f) {
                $x .= "<c r=\"$ref\" s=\"$s\"><f>" . esc($f) . '</f></c>';
            }
            elsif (!defined $v || $v eq '') {
                $x .= "<c r=\"$ref\" s=\"$s\"/>" if $s;
            }
            elsif ($v =~ /^-?\d+(\.\d+)?$/) {
                $x .= "<c r=\"$ref\" s=\"$s\"><v>$v</v></c>";
            }
            else {
                $x .= "<c r=\"$ref\" s=\"$s\" t=\"inlineStr\"><is><t xml:space=\"preserve\">" . esc($v) . '</t></is></c>';
            }
        }
        $x .= '</row>';
    }
    $x .= '</sheetData></worksheet>';
    return $x;
}

my $styles = <<'XML';
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
<numFmts count="1"><numFmt numFmtId="164" formatCode="0.0"/></numFmts>
<fonts count="5">
<font><sz val="10"/><name val="Arial"/></font>
<font><b/><sz val="10"/><name val="Arial"/></font>
<font><sz val="10"/><color rgb="FF0000FF"/><name val="Arial"/></font>
<font><b/><sz val="14"/><name val="Arial"/></font>
<font><b/><sz val="11"/><color rgb="FF1F3864"/><name val="Arial"/></font>
</fonts>
<fills count="4">
<fill><patternFill patternType="none"/></fill>
<fill><patternFill patternType="gray125"/></fill>
<fill><patternFill patternType="solid"><fgColor rgb="FFD9D9D9"/></patternFill></fill>
<fill><patternFill patternType="solid"><fgColor rgb="FFFFFF00"/></patternFill></fill>
</fills>
<borders count="1"><border><left/><right/><top/><bottom/><diagonal/></border></borders>
<cellStyleXfs count="1"><xf numFmtId="0" fontId="0" fillId="0" borderId="0"/></cellStyleXfs>
<cellXfs count="9">
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0"/>
<xf numFmtId="0" fontId="1" fillId="2" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="2" fillId="3" borderId="0" xfId="0" applyFont="1" applyFill="1"/>
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="3" fillId="0" borderId="0" xfId="0" applyFont="1"/>
<xf numFmtId="164" fontId="0" fillId="0" borderId="0" xfId="0" applyNumberFormat="1"/>
<xf numFmtId="0" fontId="4" fillId="0" borderId="0" xfId="0" applyFont="1"/>
<xf numFmtId="164" fontId="0" fillId="0" borderId="0" xfId="0" applyNumberFormat="1"/>
<xf numFmtId="0" fontId="1" fillId="0" borderId="0" xfId="0" applyFont="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
</cellXfs>
<cellStyles count="1"><cellStyle name="Normal" xfId="0" builtinId="0"/></cellStyles>
</styleSheet>
XML

sub H { map { { v => $_, s => 1 } } @_ }
sub I { { v => $_[0], s => 2 } }
sub W { { v => $_[0], s => 3 } }
sub T { { v => $_[0], s => 4 } }
sub S { { v => $_[0], s => 6 } }
sub F { { f => $_[0], s => ($_[1] // 7) } }

# ---------------------------------------------------------------- Settings
# Cell addresses of the inputs, so every formula below refers to one place.
# The recomp does not read this file: the values here mirror the constants in
# src/game/randomizer/enemy_progression.cpp, and the sheet exists to explain
# and preview what that code does.
my %S = (
    down      => 'Settings!$B$4',   # tiers below an area a set may come from
    up        => 'Settings!$B$5',   # tiers above
    shape     => 'Settings!$B$6',   # shape exponent s
    bulk      => 'Settings!$B$7',
    threat    => 'Settings!$B$8',
    thresholds=> 'Settings!$B$11:$B$17',   # T2..T8 lower bounds
    hpcap     => 'Settings!$B$21',
    statcap   => 'Settings!$B$22',
    expcap    => 'Settings!$B$23',
);
my $TIERS = 8;

my @settings = (
    [ T('Settings (the constants the code uses)') ],
    [ W('Yellow cells are the knobs the design refers to; the other tabs recalculate from them. The game itself uses the same values as constants in enemy_progression.cpp (spread_down, spread_up, shape_exponent, caps[]), so changing a cell here previews a change, it does not apply one.') ],
    [ H('Setting', 'Value', 'What it does') ],
    [ 'Spread down (tiers below)', I(7), W('An area may take a monster set from this many tiers BELOW its own (easier monsters, scaled up). 7 = any tier. The shipped value is 7: placement is unrestricted and the scaling does the balancing.') ],
    [ 'Spread up (tiers above)', I(7), W('An area may take a set from this many tiers ABOVE its own (harder monsters, scaled down). 7 = any tier (shipped). 0 would keep an area to sets no harder than its own tier.') ],
    [ 'Shape exponent s', I(0.5), W('Rule 5: how much of a monster\'s deviation from its home average survives the move. 0.5 = square root (a monster at 3x its home average lands at 1.7x the destination\'s); 1 keeps the exact ratio; 0 makes every monster the destination average. Shipped: 0.5, chosen after Ork Jr (3x its home HP) hit the 999 cap in Mammon\'s World at s = 1.') ],
    [ 'Bulk: DEF weight', I(25), W('Bulk = HP * (1 + DEF / this). Larger = DEF matters less. Feeds the Power score, which the guard (rule 6) is built on.') ],
    [ 'Threat: AGI weight', I(50), W('Threat = ATK * (1 + AGI / this). Larger = AGI matters less.') ],
    [],
    [ H('Power tier thresholds', 'Power at least', 'Tier') ],
    [ 'T2 from', I(30), 2 ],
    [ 'T3 from', I(80), 3 ],
    [ 'T4 from', I(100), 4 ],
    [ 'T5 from', I(160), 5 ],
    [ 'T6 from', I(250), 6 ],
    [ 'T7 from', I(360), 7 ],
    [ 'T8 from', I(450), 8 ],
    [ W('Power = SQRT(Bulk * Threat). Vanilla tier averages under the eight tiers: T1 15, T2 48, T3 119, T4 111, T5 187, T6 329, T7 397, T8 504 (T3 and T4 overlap: East Limelin is tougher by numbers than Windward/Blue Cave). Informational: the code does not use power tiers, only the area averages and the guard.') ],
    [],
    [ H('Stat caps', 'Cap', '') ],
    [ 'HP cap', I(999), W('Vanilla monsters stay under 999; Hard Mode raises Brian\'s caps to 999 too.') ],
    [ 'ATK/DEF/AGI cap', I(255), W('Stored as bytes in the game\'s battle struct.') ],
    [ 'EXP / Stones cap', I(65535), W('EXP and Stones are words in the table; capped generously. Every scaled value is also floored at 1.') ],
);

# ---------------------------------------------------------------- Logic
my @logic = (
    [ T('Enemy Randomizer: final logic (as implemented)') ],
    [ W('Quest 64 Recomp, 2026-09-21. This is the shipped behaviour of the "Enemy Randomizer" switch on the Randomizer tab, one option that replaces Merrow\x27s "Shuffle Encounters" and the earlier "Enemy Progression". Data on the Areas / Roster / Monsters tabs is the US ROM\x27s, read out of the Merrow tables the randomizer ships. Yellow cells on Settings mirror the code\x27s constants; change them and the sheet previews the effect.') ],
    [],
    [ S('The problem it solves') ],
    [ W('Merrow\'s enemy options are blind to difficulty. Swapping whole monster tables at random lets the Holy Plains draw the Dindom Dries set (Red Wyvern, 550 HP, against a 50 HP Brian) while Brannoch Castle gets Were Hares, and nothing touches stats, so a moved monster keeps its home numbers. The Enemy Randomizer keeps the "anything anywhere" variety but rewrites every placed monster\'s numbers for where it now lives, and guarantees the game gets progressively harder whatever the seed placed where.') ],
    [],
    [ S('In one line') ],
    [ W('Every area is given one of the game\'s six monster sets at random (any set in any area) and its encounter packs are rerolled; when the set is loaded for that area, each monster\'s HP / ATK / DEF / AGI / EXP / Stones are moved to the area\'s vanilla average, keeping the square root of the monster\'s own deviation from its home average, times a guard that never lets an area\'s numbers fall below the previous area\'s.') ],
    [],
    [ S('Rules') ],
    [ W('1. Tiers (Areas tab, column C): T1 Holy Plains / Connor Forest, T2 Dondoran Flats / Glencoe Forest / West Carmaugh, T3 Cull Hazard / East Limelin, T4 Windward Forest / Blue Cave / Isle of Skye, T5 Baragoon Tunnel, T6 Dindom Dries / Boil Hole, T7 Baragoon Moor / Brannoch Castle, T8 Mammon\'s World. Story order; the Areas rows are in this order and the guard (rule 6) walks it.') ],
    [ W('2. Monster native tier = tier of its home area (first area it appears in, by progression). The Monsters tab also computes a Power score from stats and a Power tier from the Settings thresholds, so you can see where a monster really sits versus where the game puts it (e.g. Cockatrice is T2 by area but T3 by numbers). The code only uses the home area, for the averages.') ],
    [ W('3. Placement is per monster set, not per monster. The game keeps its monsters in six files (Implementation tab), and every file loads to the same RAM address, so an area can only ever hold one whole set. For each of the 16 areas in progression order the seed picks one of the six sets; with spread 7 / 7 (Settings B4-B5) every set is a candidate for every area. When there is a choice, the set the previous area got is skipped, so consecutive areas look different. Submaps that share pack definitions (Baragoon Moor 1-2, Brannoch 1-6, Mammon 1-6) are one area and get one set.') ],
    [ W('4. Packs are rerolled. Each area keeps its vanilla number of packs and each pack its shape (how many members, always-count + extra-count), but every member slot is refilled from the new set with no repeats inside a pack, and every region gets a full seven presets drawn from the area\'s packs (Merrow\'s composition logic). So the roster an area shows is the whole placed set, in new combinations.') ],
    [ W('5. Stat scaling. For a monster of home area H placed in area A: new = AreaAvg(A, stat) * (own / AreaAvg(H, stat)) ^ s * guard(A), with s = Settings B6 (0.5). The monster is moved to the destination\'s average and keeps its own deviation from its home average, compressed: a monster three times its home average lands at 1.7 times the destination\'s, so home-area bruisers like Ork Jr do not become super-bosses late. Applies to HP, ATK, DEF, AGI, EXP and Stones alike, then rounds, floors at 1 and caps (HP 999, ATK/DEF/AGI 255, EXP 65535). See ScalingExample.') ],
    [ W('6. Progression guard (Areas columns M-N). Each area gets a budget = MAX(its own vanilla average power, the previous area\'s budget), so budgets never fall as you move through the game. guard(A) = budget / own average power, which is 1.0 wherever vanilla already climbs and > 1 where it dips (East Limelin -> Windward Forest is the one real dip). Applied to every placed monster, this is what makes the randomized game strictly progressively harder.') ],
    [ W('7. Damage needs nothing extra. The game computes an enemy\'s spell / attack damage from the ATK in its table entry (the base passed to the damage routine already follows it), so scaling ATK scales damage. An earlier build also multiplied the damage itself and double-counted (a Were Hare hit for 592 in Mammon\'s World); that hook is gone. Brian\'s own damage is never touched.') ],
    [ W('8. Flying and dangerous flags (Monsters K-L) are informational only: nothing is excluded from any area. Cockatrice keeps its petrify wherever it lands, so it is the one monster whose danger the numbers do not capture.') ],
    [ W('9. Bosses are out of scope. The boss logic that already exists in the randomizer stays as it is; nothing here moves, rescales or reads the boss entries (monster ids 67+), and the boss files are separate from the six sets.') ],
    [ W('10. Same seed, same result. Set choice and pack rerolls come from the seed\'s RNG, and the scaling is a pure function of the data on these tabs, so a seed can be shared like any other. The spoiler log (%LOCALAPPDATA%\\Quest64Recompiled\\randomizer_spoiler.txt) lists each area\'s set, guard and every monster\'s vanilla -> scaled HP/ATK/DEF/AGI/EXP.') ],
    [ W('11. Stacking with other options. The "Stats" randomizer option (Merrow\'s monster stat shuffle) writes the ROM at boot, and the Enemy Randomizer scales whatever the file holds when it loads, so the two stack: Stats perturbs a monster\'s own numbers, the Enemy Randomizer then fits them to the area. Hard Mode overrides the whole randomizer, so with Hard Mode on none of this runs.') ],
);

# ---------------------------------------------------------------- Areas
# Columns: A Rank, B Area, C Tier, D Table, E Roster size, F Roster, G-K avg
# stats, L avg power, M progression budget, N guard, O-P allowed tiers,
# Q candidates, R packs, S notes.
my @areas_rows = (
    [ T('Areas: progression order, tier, vanilla roster, averages and progression budget') ],
    [ W('Tier (C) is an input. Averages (G-L) are over the area\'s vanilla roster (Roster tab). Budget (M) = MAX(own average power, previous budget), so it never falls; Guard (N) = budget / own average, the multiplier applied to everything placed here. Allowed tiers (O-P) follow the spread settings. Q counts how many monsters could be placed here under the current settings (all 67 with 7 / 7); E is the vanilla roster size, which the placed set replaces whole.') ],
    [ H('Rank', 'Area', 'Tier', 'Monster table', 'Roster size', 'Vanilla roster', 'Avg HP', 'Avg ATK', 'Avg DEF', 'Avg AGI', 'Avg EXP', 'Avg Power', 'Progression budget', 'Guard x', 'Allowed tier min', 'Allowed tier max', 'Candidate monsters', 'Packs (vanilla: monster x always + extra)', 'Notes') ],
);
my $first_area_row = 4;
my $rank = 0;
my @area_order;
for my $p (@progression) {
    $rank++;
    my ($name, $tier, $note) = @$p;
    my $e = $area{$name};
    my $r = $first_area_row + $rank - 1;
    my $prev = $r - 1;
    my @roster = map { $tables[ $e->{table} ][$_] } sort { $a <=> $b } keys %{ $e->{slots} };
    push @area_order, $name;
    push @areas_rows, [
        $rank, $name, I($tier), $e->{table}, scalar(@roster), W(join(', ', @roster)),
        F("AVERAGEIFS(Roster!\$D:\$D,Roster!\$A:\$A,\$B$r)"),
        F("AVERAGEIFS(Roster!\$E:\$E,Roster!\$A:\$A,\$B$r)"),
        F("AVERAGEIFS(Roster!\$F:\$F,Roster!\$A:\$A,\$B$r)"),
        F("AVERAGEIFS(Roster!\$G:\$G,Roster!\$A:\$A,\$B$r)"),
        F("AVERAGEIFS(Roster!\$H:\$H,Roster!\$A:\$A,\$B$r)"),
        F("AVERAGEIFS(Roster!\$I:\$I,Roster!\$A:\$A,\$B$r)"),
        ($rank == 1 ? F("L$r") : F("MAX(L$r,M$prev)")),
        F("M$r/L$r", 7),
        F("MAX(1,C$r-$S{down})", 0),
        F("MIN($TIERS,C$r+$S{up})", 0),
        F("COUNTIFS(Monsters!\$Q:\$Q,\"<=\"&C$r,Monsters!\$R:\$R,\">=\"&C$r)", 0),
        W(join(' ', @{ $e->{packs} })),
        W($note),
    ];
}
my $last_area_row = $first_area_row + $rank - 1;

# ---------------------------------------------------------------- Roster
my @roster_rows = (
    [ T('Roster: one row per (area, monster) in the vanilla game') ],
    [ W('Stats are looked up from the Monsters tab. This is the membership the Areas averages are built on, and the shape a randomized roster would be written back into (slot = position in the area\'s monster table).') ],
    [ H('Area', 'Monster', 'Table slot', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Power', 'Native tier', 'Area tier', 'Tier gap') ],
);
my $rr = 3;
for my $name (@area_order) {
    my $e = $area{$name};
    for my $slot (sort { $a <=> $b } keys %{ $e->{slots} }) {
        $rr++;
        my $mn = $tables[ $e->{table} ][$slot];
        push @roster_rows, [
            $name, $mn, $slot,
            (map { F("INDEX(Monsters!\$$_:\$$_,MATCH(\$B$rr,Monsters!\$B:\$B,0))", 0) } qw(E F G H I)),
            F("INDEX(Monsters!\$O:\$O,MATCH(\$B$rr,Monsters!\$B:\$B,0))"),
            F("INDEX(Monsters!\$D:\$D,MATCH(\$B$rr,Monsters!\$B:\$B,0))", 0),
            F("INDEX(Areas!\$C:\$C,MATCH(\$A$rr,Areas!\$B:\$B,0))", 0),
            F("K$rr-J$rr", 0),
        ];
    }
}

# ---------------------------------------------------------------- Monsters
my @monster_rows = (
    [ T('Monsters: vanilla stats, home area, computed power and placement window') ],
    [ W('Stats are the ROM\'s (HP, ATK, DEF, AGI, EXP; Drop is the item drop slot). Native tier comes from the home area\'s tier on the Areas tab. Flying is the ROM\'s own flag (only Wyvern, Will-o\'-Wisp and Pixie carry it), not a visual judgement. Power/Power tier use the Settings weights and thresholds. Q-R are the area tiers this monster may be placed in under the current spread (Q uses spread-up: a monster may go to areas up to that many tiers below it; R uses spread-down); with the shipped 7 / 7 that is every tier. Dangerous (L) is informational. S is where the six stat halfwords live in the ROM. Bosses (ids 67+) are not listed: they are not part of this.') ],
    [ H('Id', 'Monster', 'Home area', 'Native tier', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Drop', 'Flying', 'Dangerous', 'Bulk', 'Threat', 'Power', 'Power tier', 'Placeable from tier', 'Placeable to tier', 'Stat ROM address') ],
);
my $mr = 3;
for my $m (@monsters) {
    $mr++;
    push @monster_rows, [
        $m->{id}, $m->{name}, $m->{home},
        F("IFERROR(INDEX(Areas!\$C:\$C,MATCH(\$C$mr,Areas!\$B:\$B,0)),\"\")", 0),
        $m->{hp}, $m->{atk}, $m->{def}, $m->{agi}, $m->{exp}, $m->{drop},
        $m->{flying}, I($m->{dangerous}),
        F("E$mr*(1+G$mr/$S{bulk})"),
        F("F$mr*(1+H$mr/$S{threat})"),
        F("ROUND(SQRT(M$mr*N$mr),1)"),
        F("1+COUNTIF($S{thresholds},\"<=\"&O$mr)", 0),
        F("MAX(1,D$mr-$S{up})", 0),
        F("MIN($TIERS,D$mr+$S{down})", 0),
        $m->{addr},
    ];
}

# ---------------------------------------------------------------- Scaling example
my @ex = (
    [ T('Scaling example: one monster dropped into one area') ],
    [ W('Pick a monster and a destination area (yellow). The table shows its home stats, the two areas\' vanilla averages, the factor between them, the destination\'s progression guard, and the rescaled stats under the shape exponent (Settings B6) with the guard applied. The grid below previews the same monster in every area.') ],
    [ 'Monster', I('RED WYVERN') ],
    [ 'Destination area', I('Holy Plains') ],
    [ 'Home area', F('INDEX(Monsters!$C:$C,MATCH($B$3,Monsters!$B:$B,0))', 0) ],
    [ 'Home tier', F('INDEX(Monsters!$D:$D,MATCH($B$3,Monsters!$B:$B,0))', 0), 'Destination tier', F('INDEX(Areas!$C:$C,MATCH($B$4,Areas!$B:$B,0))', 0) ],
    [ 'Within allowed spread?', F('IF(AND(D6>=INDEX(Monsters!$Q:$Q,MATCH($B$3,Monsters!$B:$B,0)),D6<=INDEX(Monsters!$R:$R,MATCH($B$3,Monsters!$B:$B,0))),"yes","no (outside spread)")', 0) ],
    [ 'Destination guard x', F('INDEX(Areas!$N:$N,MATCH($B$4,Areas!$B:$B,0))') ],
    [ H('Stat', 'Own (home)', 'Home area avg', 'Destination avg', 'Own / home avg', 'Shape ^ s', 'Scaled (with guard)', 'Cap') ],
);
my @statcols = ( [ 'HP', 'E', 'G', $S{hpcap} ], [ 'ATK', 'F', 'H', $S{statcap} ], [ 'DEF', 'G', 'I', $S{statcap} ], [ 'AGI', 'H', 'J', $S{statcap} ], [ 'EXP', 'I', 'K', $S{expcap} ] );
my $er = 9;
for my $sc (@statcols) {
    $er++;
    my ($label, $mcol, $acol, $cap) = @$sc;
    push @ex, [
        $label,
        F("INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))", 0),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0))"),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$4,Areas!\$B:\$B,0))"),
        F("B$er/C$er"),
        F("E$er^$S{shape}"),
        F("MIN(H$er,ROUND(D$er*F$er*\$B\$8,0))", 0),
        F($cap, 0),
    ];
}
push @ex, [], [ S('Preview: the chosen monster in every area (guard included)') ],
    [ H('Area', 'Tier', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Allowed?') ];
my $pr = $er + 3;
for my $name (@area_order) {
    $pr++;
    my @cells = ( $name, F("INDEX(Areas!\$C:\$C,MATCH(\$A$pr,Areas!\$B:\$B,0))", 0) );
    for my $sc (@statcols) {
        my ($label, $mcol, $acol, $cap) = @$sc;
        push @cells, F("MIN($cap,ROUND(INDEX(Areas!\$$acol:\$$acol,MATCH(\$A$pr,Areas!\$B:\$B,0))*(INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))/INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0)))^$S{shape}*INDEX(Areas!\$N:\$N,MATCH(\$A$pr,Areas!\$B:\$B,0)),0))", 0);
    }
    push @cells, F("IF(AND(B$pr>=INDEX(Monsters!\$Q:\$Q,MATCH(\$B\$3,Monsters!\$B:\$B,0)),B$pr<=INDEX(Monsters!\$R:\$R,MATCH(\$B\$3,Monsters!\$B:\$B,0))),\"yes\",\"no\")", 0);
    push @ex, \@cells;
}

# ---------------------------------------------------------------- Implementation
my @impl = (
    [ T('Implementation notes (how the recomp does it)') ],
    [ S('What the game stores') ],
    [ W('Six monster files, one per world chunk: file 0 (ROM 0xADC090) Holy Plains + West Carmaugh, 1 (0xB63378) Connor / Dondoran Flats / Glencoe, 2 (0xBBDE98) Cull Hazard / Blue Cave / East Limelin, 3 (0xC317E8) Dindom / Boil Hole / Baragoon Moor / Brannoch, 4 (0xC9BE50) Windward / Isle of Skye / Baragoon Tunnel, 5 Mammon\'s World. Each area\'s 0x18-byte map header holds the file index it uses (merrow_mapdata.cpp AreaMap.table_index). The file table at 0x80054160 has one 20-byte row per file: ROM start, ROM end, then three RAM pointers into the loaded file (table end at +8, table start at +0xC). Every file is DMA\'d to the same RAM base (0x8020E6F0), which is why placement is per set: two sets cannot be in memory at once.') ],
    [ W('Encounters are packs: up to four (table slot, always-count, extra-count) entries, written as packCount*3 words at the pack\'s ROM address. Regions pick from up to seven pack presets. Baragoon Moor 1-2, Brannoch 1-6 and Mammon 1-6 share pack definitions, so those submaps are treated as one area (the sheet merges them the same way).') ],
    [ W('The loaded monster table is 0x38 bytes per entry: +2 index, +4 and +6 HP (stored twice), +0xC AGI, +0xE DEF, +0x10 EXP (word), +0x14 Stones (word), +0x2A ATK. The battle set-up routine (func_80008FE0) copies an entry into the battle struct, and Soul Searcher reads the table directly, so the table is the one place that must hold the scaled numbers.') ],
    [ S('Boot (randomizer.cpp shuffle_enemies / patch_enemies, seed-driven)') ],
    [ W('1. For each of the 16 merged areas in progression order, candidate_tables() lists the files whose monsters fall inside the spread (all six with 7 / 7); the previous area\'s file is dropped when there is a choice; one is drawn with the seed\'s RNG. 2. The file index is written into every raw submap header of the area, and pack member ids are wrapped to the new file\'s size. 3. Packs are rerolled: every region gets seven presets drawn from the area\'s packs, and each pack\'s member slots are refilled from the file\'s entries with no repeats (Merrow\'s composition). 4. make_plan() turns the per-area file choice into a Plan: for each game map id, the (home area, destination area) pair of every entry of the file it uses, and writes the spoiler (vanilla -> scaled stats). The Plan is installed with set_active() and the ROM writes go through the randomizer\'s normal Write records.') ],
    [ S('Play time (native_hooks.cpp quest64_randomizer_enemy_scale_table)') ],
    [ W('One hook in func_80008EF4, the routine that DMAs a monster file, placed right after its dma call (before_vram 0x80008F5C). It reads the file-table row the game just loaded (row pointer at 0x8007D0BC), walks the table between the row\'s start and end pointers, and rewrites each entry in place with scaled_value(map, entry, stat, own) for HP (both halfwords), AGI, DEF, ATK, EXP and Stones. The map is gCurrentMap (0x80084EEC); if the plan has nothing for it (a load that happens during a transition) gNextMap (0x80084EE4) is used instead. Nothing runs unless the randomizer is the active mode, the Enemy Randomizer switch is on and Hard Mode is off.') ],
    [ W('Scaling in RAM rather than in the ROM is forced by the shared RAM base: the same file serves several areas with different averages, so its stats cannot be fixed at boot. Scaling on load rather than at battle set-up is what makes Soul Searcher and the battle agree.') ],
    [ S('Formula (enemy_progression.cpp rescale)') ],
    [ W('new = round(AreaAvg(dest, stat) * (own / AreaAvg(home, stat)) ^ 0.5 * guard(dest)), then clamped to 1 .. cap (HP 999, ATK/DEF/AGI 255, EXP/Stones 65535). guard(dest) = budget / avg_power with budget = MAX(own avg power, previous area\'s budget), computed once from the Areas data (columns L-N). The area averages, home areas and file rosters are generated into enemy_progression_data.cpp by tools/enemyrandologic.pl from the same Merrow data this sheet is built from, so the sheet and the code cannot drift apart.') ],
    [ S('What was tried and dropped') ],
    [ W('Scaling per monster at battle set-up (func_80008FE0): battle numbers were right but Soul Searcher showed the unscaled table. Keying on gNextMap: wrong during the load that happens after a transition completes. A separate damage hook in func_80006BEC multiplying the damage base by the ATK ratio: the base already follows the scaled ATK, so it double-counted (x14.8 in Mammon\'s World). Shape exponent 1 (keep the exact ratio): Ork Jr, three times its home average, pinned at 999 HP in Mammon\'s World; 0.5 lands it around 780.') ],
    [ S('Files') ],
    [ W('src/game/randomizer/enemy_progression.h/.cpp (plan, rescale, scaled_value), enemy_progression_data.h/.cpp (GENERATED), native_hooks.cpp (the load hook), randomizer.cpp (placement, packs, spoiler, option), us.rev0.toml (the hook entry), include/randomizer.h (Options::enemy_randomizer), src/ui/ui_config.cpp + assets/config_menu/randomizer.rml (the switch), tools/enemyrandologic.pl (this sheet and the data). Setting stored as "enemy_randomizer" in randomizer.json.') ],
);

# ---------------------------------------------------------------- write
my @sheets = (
    [ 'Logic',          \@logic,         [ 120 ],                                   0 ],
    [ 'Settings',       \@settings,      [ 34, 12, 90 ],                            0 ],
    [ 'Areas',          \@areas_rows,    [ 6, 18, 6, 8, 8, 44, 8, 8, 8, 8, 8, 10, 9, 9, 10, 70, 30 ], 3 ],
    [ 'Roster',         \@roster_rows,   [ 18, 20, 8, 7, 7, 7, 7, 7, 8, 9, 9, 8 ],  3 ],
    [ 'Monsters',       \@monster_rows,  [ 5, 20, 18, 9, 7, 7, 7, 7, 7, 6, 7, 10, 9, 9, 9, 9, 10, 10, 14 ], 3 ],
    [ 'ScalingExample', \@ex,            [ 20, 14, 16, 16, 10, 8, 10, 8 ],          0 ],
    [ 'Implementation', \@impl,          [ 120 ],                                   0 ],
);

my %files;
$files{'[Content_Types].xml'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">' .
    '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>' .
    '<Default Extension="xml" ContentType="application/xml"/>' .
    '<Override PartName="/xl/workbook.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>' .
    '<Override PartName="/xl/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml"/>' .
    join('', map { "<Override PartName=\"/xl/worksheets/sheet$_.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>" } 1 .. @sheets) .
    '</Types>';
$files{'_rels/.rels'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' .
    '<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="xl/workbook.xml"/>' .
    '</Relationships>';
$files{'xl/workbook.xml'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">' .
    '<sheets>' . join('', map { sprintf('<sheet name="%s" sheetId="%d" r:id="rId%d"/>', $sheets[$_][0], $_ + 1, $_ + 1) } 0 .. $#sheets) . '</sheets>' .
    '<calcPr calcId="191029" fullCalcOnLoad="1"/></workbook>';
$files{'xl/_rels/workbook.xml.rels'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' .
    join('', map { sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet%d.xml"/>', $_ + 1, $_ + 1) } 0 .. $#sheets) .
    sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>', @sheets + 1) .
    '</Relationships>';
$files{'xl/styles.xml'} = $styles;
for my $i (0 .. $#sheets) {
    $files{"xl/worksheets/sheet" . ($i + 1) . ".xml"} = sheet_xml($sheets[$i][1], $sheets[$i][2], $sheets[$i][3]);
}

my $zip;
my @names_in_order = ('[Content_Types].xml', '_rels/.rels', 'xl/workbook.xml', 'xl/_rels/workbook.xml.rels', 'xl/styles.xml',
    map { "xl/worksheets/sheet$_.xml" } 1 .. @sheets);
my $z = IO::Compress::Zip->new($out, Name => $names_in_order[0]) or die "zip: $ZipError";
for my $i (0 .. $#names_in_order) {
    $z->newStream(Name => $names_in_order[$i]) if $i > 0;
    $z->print($files{ $names_in_order[$i] });
}
$z->close;
printf "wrote %s (%d areas, %d monsters, %d roster rows)\n", $out, scalar @area_order, scalar @monsters, $rr - 3;

# ---------------------------------------------------------------- C++ data
# The same tiers, averages and flags as the sheet, for the randomizer's
# enemy progression option (src/game/randomizer/enemy_progression.cpp).
# Settings defaults mirror the Settings tab.
my %map_id = (    # area -> game map id; each area's map header lies inside
    'Holy Plains' => 2, 'Connor Forest' => 31, 'Dondoran Flats' => 3,        # exactly one entry of the
    'Glencoe Forest' => 32, 'West Carmaugh' => 5, 'Cull Hazard' => 27,       # ROM map table at
    'East Limelin' => 7, 'Windward Forest' => 33, 'Blue Cave' => 26,         # 0x80054F10 (0x44 bytes
    'Isle of Skye' => 12, 'Baragoon Tunnel' => 28, 'Dindom Dries' => 9,      # each, +4 rom start,
    'Boil Hole' => 29, 'Baragoon Moor' => 11, 'Brannoch Castle' => 30,        # +8 rom end)
    "Mammon's World" => 34,
);
my %raw_range;    # merged area -> [first, last] index into mapdata::areas
for my $i (0 .. $#raw_areas) {
    my $name = $merge{ $raw_areas[$i]{name} } // $raw_areas[$i]{name};
    $raw_range{$name} //= [ $i, $i ];
    $raw_range{$name}[1] = $i;
}
my %stat_of = map { $_->{name} => $_ } @monsters;
my ($W_BULK, $W_THREAT) = (25, 50);
sub power { my $m = shift; sqrt($m->{hp} * (1 + $m->{def} / $W_BULK) * $m->{atk} * (1 + $m->{agi} / $W_THREAT)) }

my %area_index = map { $area_order[$_] => $_ } 0 .. $#area_order;
my $cpp = "// GENERATED by tools/enemyrandologic.pl from the Merrow tables; do not edit.\n" .
    "// The same numbers as DOCS/enemyrandologic.xlsx (Areas / Monsters tabs).\n" .
    "#include \"enemy_progression_data.h\"\n\nnamespace merrow::progression {\n\n" .
    "const std::vector<AreaInfo> areas = {\n";
for my $name (@area_order) {
    my $e = $area{$name};
    my @roster = map { $stat_of{ $tables[ $e->{table} ][$_] } } sort { $a <=> $b } keys %{ $e->{slots} };
    my %avg;
    for my $k (qw(hp atk def agi exp)) { my $s = 0; $s += $_->{$k} for @roster; $avg{$k} = $s / @roster }
    my $pw = 0; $pw += power($_) for @roster; $pw /= @roster;
    my ($tier) = map { $_->[1] } grep { $_->[0] eq $name } @progression;
    $cpp .= sprintf("    { \"%s\", %d, %d, %d, %d, %d, { %.3f, %.3f, %.3f, %.3f, %.3f }, %.3f },\n",
        $name, $map_id{$name}, $tier, $e->{table}, $raw_range{$name}[0], $raw_range{$name}[1],
        @avg{qw(hp atk def agi exp)}, $pw);
}
$cpp .= "};\n\nconst std::vector<MonsterInfo> monsters = {\n";
for my $m (@monsters) {
    $cpp .= sprintf("    { \"%s\", %d, %s, %s, { %d, %d, %d, %d, %d } },\n", $m->{name}, $area_index{ $m->{home} } // -1,
        $m->{dangerous} eq 'Y' ? 'true' : 'false', $m->{flying} eq 'Y' ? 'true' : 'false',
        @{$m}{qw(hp atk def agi exp)});
}
$cpp .= "};\n\n// Monster id (index into monsters) for each entry of each of the six files.\n" .
    "const std::vector<std::vector<int>> table_monsters = {\n";
my %id_of = map { $monsters[$_]{name} => $_ } 0 .. $#monsters;
for my $t (@tables) {
    $cpp .= "    { " . join(", ", map { $id_of{$_} // -1 } @$t) . " },\n";
}
$cpp .= "};\n\n}\n";
open my $cf, '>', "$root/src/game/randomizer/enemy_progression_data.cpp" or die $!;
print $cf $cpp;
close $cf;
print "wrote src/game/randomizer/enemy_progression_data.cpp\n";
