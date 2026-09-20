#!/usr/bin/perl
# Builds DOCS/enemyrandologic.xlsx: the design for a progression-aware enemy
# randomizer, with the game's real area rosters and monster stats pulled out
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
my %S = (
    down      => 'Settings!$B$4',   # tiers below an area a monster may come from
    up        => 'Settings!$B$5',   # tiers above
    k         => 'Settings!$B$6',   # scaling strength
    fliers    => 'Settings!$B$7',
    dangerous => 'Settings!$B$8',
    bulk      => 'Settings!$B$9',
    threat    => 'Settings!$B$10',
    thresholds=> 'Settings!$B$13:$B$19',   # T2..T8 lower bounds
    hpcap     => 'Settings!$B$23',
    statcap   => 'Settings!$B$24',
);
my $TIERS = 8;

my @settings = (
    [ T('Settings (inputs)') ],
    [ W('Yellow cells are the knobs the design refers to. Everything else on the other tabs is calculated from these and the game data.') ],
    [ H('Setting', 'Value', 'What it does') ],
    [ 'Spread down (tiers below)', I(7), W('An area may take monsters from this many tiers BELOW its own (easier monsters, scaled up). 7 = any tier (the default: placement is unrestricted, the scaling does the balancing). 0 = none.') ],
    [ 'Spread up (tiers above)', I(1), W('An area may take monsters from this many tiers ABOVE its own (harder monsters, scaled down). 0 = none. 7 = anything (Merrow\'s behaviour).') ],
    [ 'Scaling strength k', I(1), W('0 = keep home stats, 0.5 = halfway to the destination area\'s average, 1 = full fit. EXP and the progression guard always use 1.') ],
    [ 'Max fliers per area', I(99), W('Not enforced (any monster may appear anywhere); kept for reference. The ROM flags only Wyvern, Will-o\x27-Wisp and Pixie as flying.') ],
    [ 'Dangerous: keep at native tier or above', I('N'), W('Off by default: Cockatrice / Flamed Mane may appear anywhere, rescaled like everything else. Y pins them at or above their native tier.') ],
    [ 'Bulk: DEF weight', I(25), W('Bulk = HP * (1 + DEF / this). Larger = DEF matters less.') ],
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
    [ W('Power = SQRT(Bulk * Threat). Vanilla tier averages under the eight tiers: T1 15, T2 48, T3 119, T4 111, T5 187, T6 329, T7 397, T8 504 (T3 and T4 overlap: East Limelin is tougher by numbers than Windward/Blue Cave). Adjust and watch Monsters column P.') ],
    [],
    [ H('Stat caps', 'Cap', '') ],
    [ 'HP cap', I(999), W('Vanilla monsters stay under 999; Hard Mode raises Brian\'s caps to 999 too.') ],
    [ 'ATK/DEF/AGI cap', I(255), W('Stored as bytes in the game\'s battle struct.') ],
);

# ---------------------------------------------------------------- Logic
my @logic = (
    [ T('Enemy randomizer: progression-aware placement (design)') ],
    [ W('Quest 64 Recomp, 2026-09-20. Data on the Areas / Roster / Monsters tabs is the US ROM\'s, read out of the Merrow tables the randomizer already ships. Yellow cells are inputs; change them and the sheet recalculates.') ],
    [],
    [ S('The problem') ],
    [ W('Merrow\'s two enemy options are blind to difficulty. "Enemy tables" swaps whole monster tables between areas at random, so the Holy Plains can draw the Dindom Dries table (Red Wyvern, 550 HP, against a 50 HP Brian) while Brannoch Castle gets Were Hares. "Enemy composition" only rerolls packs inside an area\'s own table, so nothing actually changes hands. Neither touches stats, so a moved monster keeps its home numbers.') ],
    [],
    [ S('The idea, in one line') ],
    [ W('Give every area a tier (1-8, story order) and every monster a native tier (the tier of the area it comes from). An area of tier T draws its roster only from monsters whose native tier is within a chosen spread of T, and every placed monster has its stats rescaled toward the destination area\'s budget, so the shape of the monster (glass cannon, tank, fast) survives but its numbers match where it now lives. The budget never goes down from one area to the next, so the game always gets progressively harder regardless of what the seed placed where. Exp and stones follow the destination budget, so levelling pace is unchanged.') ],
    [],
    [ S('Rules') ],
    [ W('1. Tiers (Areas tab, column C): T1 Holy Plains / Connor Forest, T2 Dondoran Flats / Glencoe Forest / West Carmaugh, T3 Cull Hazard / East Limelin, T4 Windward Forest / Blue Cave / Isle of Skye, T5 Baragoon Tunnel, T6 Dindom Dries / Boil Hole, T7 Baragoon Moor / Brannoch Castle, T8 Mammon\'s World. Editable.') ],
    [ W('2. Monster native tier = tier of its home area (first area it appears in, by progression). The Monsters tab also computes a Power score from stats and a Power tier from thresholds (Settings), so you can see where a monster really sits versus where the game puts it (e.g. Cockatrice is T2 by area but T3 by numbers).') ],
    [ W('3. Spread (Settings B4 down / B5 up). Default 7 / 7: any monster set can appear in any area, the scaling below is what balances it. Narrower spreads (e.g. 1 / 1) keep an area to sets whose monsters are within that many tiers of its own; 0 / 0 is a reshuffle within difficulty bands. Note the game\x27s monsters come in six fixed sets that are placed whole (see Implementation), so the window applies to sets, not single monsters.') ],
    [ W('4. Roster size stays the same. Each area keeps its vanilla number of distinct monsters (Areas column E), because the pack definitions reference table slots 0..N-1 and the packs\' shapes (how many of each, min + extra) are kept as they are. The randomizer fills those slots from the allowed pool, no repeats within an area, and prefers monsters not already used by a neighbouring tier so the game does not become the same eight enemies everywhere.') ],
    [ W('5. Stat scaling (Settings B6). For a monster placed in area A: factor = AreaAvg(A, stat) / AreaAvg(home, stat); new = round(own * factor ^ k * guard(A)). k = 1.0 fits the monster fully to the new area, 0.5 meets halfway, 0 leaves stats alone (not recommended). EXP always uses k = 1. HP capped at 999, ATK/DEF/AGI at 255. See ScalingExample for a live worked example.') ],
    [ W('6. Progression guard (Areas columns M-N). Each area gets a budget = MAX(its own vanilla average power, the previous area\'s budget), so budgets never fall as you move through the game. guard(A) = budget / own average power, which is 1.0 wherever vanilla already climbs and > 1 where it dips (East Limelin -> Windward Forest is the one real dip). Applied to every placed monster, this is what makes the randomized game strictly progressively harder.') ],
    [ W('7. Flying and dangerous flags (Monsters K-L) are informational only: nothing is excluded from any area. Cockatrice keeps its petrify wherever it lands, so it is the one monster whose danger the numbers do not capture; Settings B8 = Y pins it (and Flamed Mane) at their native tier or above if that proves too harsh early.') ],
    [ W('8. Bosses are out of scope. The boss logic that already exists in the randomizer stays as it is; nothing here moves, rescales or reads the boss entries (monster ids 67+).') ],
    [ W('9. Same seed, same result: the placement is a pure function of the seed and these settings, so a seed can be shared like any other randomizer seed, and the spoiler log lists each area\'s new roster with the scaled stats.') ],
    [],
    [ S('Why this is cheap to build') ],
    [ W('Everything it changes is ROM data the randomizer already writes at boot: the area\'s table index (map header), the pack member ids, and each monster\'s six stat halfwords (Monsters column S has the ROM address). No native hooks and no recompiled code. The one engine question to verify is whether a table can mix monsters from different world chunks (the Implementation tab explains).') ],
);

# ---------------------------------------------------------------- Areas
# Columns: A Rank, B Area, C Tier, D Table, E Roster size, F Roster, G-K avg
# stats, L avg power, M progression budget, N guard, O-P allowed tiers,
# Q candidates, R packs, S notes.
my @areas_rows = (
    [ T('Areas: progression order, tier, vanilla roster, averages and progression budget') ],
    [ W('Tier (C) is an input. Averages (G-L) are over the area\'s vanilla roster (Roster tab). Budget (M) = MAX(own average power, previous budget), so it never falls; Guard (N) = budget / own average, the multiplier applied to everything placed here. Allowed tiers (O-P) follow the spread settings. Q counts how many monsters could be placed here under the current settings; E is how many the randomizer must place.') ],
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
    [ W('Stats are the ROM\'s (HP, ATK, DEF, AGI, EXP; Drop is the item drop slot). Native tier comes from the home area\'s tier on the Areas tab. Flying is the ROM\'s own flag (only Wyvern, Will-o\'-Wisp and Pixie carry it), not a visual judgement. Power/Power tier use the Settings weights and thresholds. Q-R are the area tiers this monster may be placed in under the current spread (Q uses spread-up: a monster may go to areas up to that many tiers below it; R uses spread-down); dangerous monsters are pinned at or above their native tier. S is where the six stat halfwords live in the ROM. Bosses (ids 67+) are not listed: they are not part of this.') ],
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
        F("IF(AND(L$mr=\"Y\",$S{dangerous}=\"Y\"),D$mr,MAX(1,D$mr-$S{up}))", 0),
        F("MIN($TIERS,D$mr+$S{down})", 0),
        $m->{addr},
    ];
}

# ---------------------------------------------------------------- Scaling example
my @ex = (
    [ T('Scaling example: one monster dropped into one area') ],
    [ W('Pick a monster and a destination area (yellow). The table shows its home stats, the two areas\' vanilla averages, the factor between them, the destination\'s progression guard, and the rescaled stats under Settings!B6. The grid below previews the same monster in every area.') ],
    [ 'Monster', I('RED WYVERN') ],
    [ 'Destination area', I('Holy Plains') ],
    [ 'Home area', F('INDEX(Monsters!$C:$C,MATCH($B$3,Monsters!$B:$B,0))', 0) ],
    [ 'Home tier', F('INDEX(Monsters!$D:$D,MATCH($B$3,Monsters!$B:$B,0))', 0), 'Destination tier', F('INDEX(Areas!$C:$C,MATCH($B$4,Areas!$B:$B,0))', 0) ],
    [ 'Within allowed spread?', F('IF(AND(D6>=INDEX(Monsters!$Q:$Q,MATCH($B$3,Monsters!$B:$B,0)),D6<=INDEX(Monsters!$R:$R,MATCH($B$3,Monsters!$B:$B,0))),"yes","no (outside spread)")', 0) ],
    [ 'Destination guard x', F('INDEX(Areas!$N:$N,MATCH($B$4,Areas!$B:$B,0))') ],
    [ H('Stat', 'Own (home)', 'Home area avg', 'Destination avg', 'Factor', 'k used', 'Scaled (with guard)', 'Cap') ],
);
my @statcols = ( [ 'HP', 'E', 'G', $S{hpcap} ], [ 'ATK', 'F', 'H', $S{statcap} ], [ 'DEF', 'G', 'I', $S{statcap} ], [ 'AGI', 'H', 'J', $S{statcap} ], [ 'EXP', 'I', 'K', '99999' ] );
my $er = 9;
for my $sc (@statcols) {
    $er++;
    my ($label, $mcol, $acol, $cap) = @$sc;
    my $k = $label eq 'EXP' ? '1' : $S{k};
    push @ex, [
        $label,
        F("INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))", 0),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0))"),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$4,Areas!\$B:\$B,0))"),
        F("D$er/C$er"),
        F($k),
        F("MIN(H$er,ROUND(B$er*E$er^F$er*\$B\$8,0))", 0),
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
        my $k = $label eq 'EXP' ? '1' : $S{k};
        push @cells, F("MIN($cap,ROUND(INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))*(INDEX(Areas!\$$acol:\$$acol,MATCH(\$A$pr,Areas!\$B:\$B,0))/INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0)))^$k*INDEX(Areas!\$N:\$N,MATCH(\$A$pr,Areas!\$B:\$B,0)),0))", 0);
    }
    push @cells, F("IF(AND(B$pr>=INDEX(Monsters!\$Q:\$Q,MATCH(\$B\$3,Monsters!\$B:\$B,0)),B$pr<=INDEX(Monsters!\$R:\$R,MATCH(\$B\$3,Monsters!\$B:\$B,0))),\"yes\",\"no\")", 0);
    push @ex, \@cells;
}

# ---------------------------------------------------------------- Implementation
my @impl = (
    [ T('Implementation notes (for the recomp\'s randomizer)') ],
    [ S('What the game stores') ],
    [ W('Six monster tables, one per world chunk: table 0 (ROM 0xADC090) Holy Plains + West Carmaugh, 1 (0xB63378) Connor / Dondoran Flats / Glencoe, 2 (0xBBDE98) Cull Hazard / Blue Cave / East Limelin, 3 (0xC317E8) Dindom / Boil Hole / Baragoon Moor / Brannoch, 4 (0xC9BE50) Windward / Isle of Skye / Baragoon Tunnel, 5 Mammon\'s World. Each area\'s 0x18-byte map header holds the table index it uses (merrow_mapdata.cpp AreaMap.table_index).') ],
    [ W('Encounters are packs: up to four (table slot, always-count, extra-count) entries, written as packCount*3 words at the pack\'s ROM address. Regions pick from up to seven pack presets. Baragoon Moor 1-2, Brannoch 1-6 and Mammon 1-6 share pack definitions, so those submaps must be treated as one area (the sheet already merges them).') ],
    [ W('Monster stats are six halfwords per monster at the addresses in Monsters!S (HP is stored twice in a row; Merrow\'s one-hit-KO writes both). The randomizer already writes them (options.monster_stats / monster_scale), so rescaled stats are the same Write records.') ],
    [ S('Algorithm (boot time, pure function of seed + settings)') ],
    [ W('1. Read tiers, spread, k from the options. 2. For each area in progression order: pool = monsters with Placeable-from <= tier <= Placeable-to, minus monsters already placed in this tier and its neighbours where possible, minus fliers over the cap. 3. Draw roster-size monsters from the pool (seeded RNG). 4. Build the area\'s table: either (a) point the area at the table that already holds those monsters when they all come from one chunk, or (b) write a new table into free ROM space listing the chosen monsters\' entries. 5. Keep the pack shapes, remap each pack member\'s slot to the new roster. 6. For each placed monster, write scaled stats. 7. Spoiler log: area -> roster with scaled stats.') ],
    [ S('The one thing to verify first') ],
    [ W('Whether a table can mix monsters from different chunks, i.e. whether a monster\'s model and animation data is reachable from any area or only from its own chunk\'s overlay. Merrow\'s table swap already makes an area use another chunk\'s whole table, which suggests monsters load from wherever their table entry points; a 10-minute test is to hand-write one table entry (e.g. put a Wyvern into table 0) and walk into the Holy Plains. If cross-chunk placement is not possible, the fallback is spread = 0 within each chunk plus stat scaling, which still fixes the difficulty problem but with smaller pools.') ],
    [ S('Where it goes in the code') ],
    [ W('src/game/randomizer/randomizer.cpp shuffle_enemies() already owns the table index / pack writes; add the tier pool selection there and the stat writes next to the existing monster_stats writes. Options: enemy_progression (bool), enemy_spread_down and enemy_spread_up (int), enemy_scale_strength (0/50/100 %), enemy_max_fliers. The per-area tiers and the progression budgets are constants derived from the Areas tab. UI: four controls under the existing "Enemy shuffling" group on the Randomizer tab. The tier table itself can be a constant array (one tier per area, from the Areas tab).') ],
    [ S('Open questions') ],
    [ W('Should exp scale with k or always fully (this sheet says always)? Do the story bosses\' areas (Connor, Windward, Blue Cave, Boil Hole, Moor, Brannoch) need the boss\'s own minions kept vanilla for the fight leading up to them?') ],
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
    $cpp .= sprintf("    { \"%s\", %d, %s, %s },\n", $m->{name}, $area_index{ $m->{home} } // -1,
        $m->{dangerous} eq 'Y' ? 'true' : 'false', $m->{flying} eq 'Y' ? 'true' : 'false');
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
