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
    [ 'East Limelin',     3, 'Normoon side' ],
    [ 'Windward Forest',  3, 'Zelse' ],
    [ 'Blue Cave',        4, 'Nepty' ],
    [ 'Isle of Skye',     4, '' ],
    [ 'Baragoon Tunnel',  4, '' ],
    [ 'Dindom Dries',     5, 'Shamwood side' ],
    [ 'Boil Hole',        5, 'Fargo' ],
    [ 'Baragoon Moor',    5, 'Shilf; submaps 1-2 share one roster' ],
    [ 'Brannoch Castle',  6, 'Guilty, Beigis; submaps 1-6 share one roster' ],
    [ "Mammon's World",   7, 'submaps 1-6 share one roster' ],
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

# ---------------------------------------------------------------- Logic
my @logic = (
    [ T('Enemy randomizer: progression-aware placement (design)') ],
    [ W('Quest 64 Recomp, 2026-09-20. Data on the Areas / Roster / Monsters tabs is the US ROM\'s, read out of the Merrow tables the randomizer already ships. Yellow cells are inputs; change them and the sheet recalculates.') ],
    [],
    [ S('The problem') ],
    [ W('Merrow\'s two enemy options are blind to difficulty. "Enemy tables" swaps whole monster tables between areas at random, so the Holy Plains can draw the Dindom Dries table (Red Wyvern, 550 HP, against a 50 HP Brian) while Brannoch Castle gets Were Hares. "Enemy composition" only rerolls packs inside an area\'s own table, so nothing actually changes hands. Neither touches stats, so a moved monster keeps its home numbers.') ],
    [],
    [ S('The idea, in one line') ],
    [ W('Give every area a tier (1-7, story order) and every monster a native tier (the tier of the area it comes from). An area of tier T draws its roster only from monsters whose native tier is within a chosen spread of T, and any monster placed outside its native tier has its stats rescaled toward the destination area\'s vanilla averages, so the shape of the monster (glass cannon, tank, fast) survives but its budget matches where it now lives. Exp and stones always scale with the destination, so levelling pace is unchanged.') ],
    [],
    [ S('Rules') ],
    [ W('1. Tiers. Areas tab, column C. Suggested: T1 Holy Plains/Connor, T2 Dondoran Flats/Glencoe/West Carmaugh, T3 Cull Hazard/East Limelin/Windward, T4 Blue Cave/Isle of Skye/Baragoon Tunnel, T5 Dindom Dries/Boil Hole/Baragoon Moor, T6 Brannoch, T7 Mammon\'s World. Editable.') ],
    [ W('2. Monster native tier = tier of its home area (first area it appears in, by progression). The Monsters tab also computes a Power score from stats and a Power tier from thresholds (Settings), so you can see where a monster really sits versus where the game puts it (e.g. Cockatrice is T2 by area but T3 by numbers).') ],
    [ W('3. Tier spread (Settings B4). A tier-T area may take monsters of native tier T-spread .. T+spread. 0 = same tier only (a reshuffle within difficulty bands), 1 = the recommended default, 2 = wild, 6 = anything goes (Merrow\'s behaviour). Dangerous monsters (Monsters column L: Cockatrice petrifies, Flamed Mane) never go below their native tier whatever the spread.') ],
    [ W('4. Roster size stays the same. Each area keeps its vanilla number of distinct monsters (Areas column E), because the pack definitions reference table slots 0..N-1 and the packs\' shapes (how many of each, min + extra) are kept as they are. The randomizer fills those slots from the allowed pool, no repeats within an area, and prefers monsters not already used by a neighbouring tier so the game does not become the same eight enemies everywhere.') ],
    [ W('5. Stat scaling (Settings B5). For a monster placed in area A: factor = AreaAvg(A, stat) / AreaAvg(home, stat); new = round(own * factor ^ k). k = 1.0 fits the monster fully to the new area, 0.5 meets halfway, 0 leaves stats alone (not recommended). EXP always uses k = 1. HP capped at 999, ATK/DEF/AGI at 255. See ScalingExample for a live worked example.') ],
    [ W('6. Flying (Monsters column K). The vanilla game limits fliers because some spells cannot reach them; keep at most Settings B6 fliers per area and none in tier 1.') ],
    [ W('7. Bosses are never moved or rescaled here; Merrow\'s boss options already cover them.') ],
    [ W('8. Same seed, same result: the placement is a pure function of the seed and these settings, so a seed can be shared like any other randomizer seed, and the spoiler log lists each area\'s new roster with the scaled stats.') ],
    [],
    [ S('Why this is cheap to build') ],
    [ W('Everything it changes is ROM data the randomizer already writes at boot: the area\'s table index (map header), the pack member ids, and each monster\'s six stat halfwords (Monsters column S has the ROM address). No native hooks and no recompiled code. The one engine question to verify is whether a table can mix monsters from different world chunks (the Implementation tab explains).') ],
);

# ---------------------------------------------------------------- Settings
my @settings = (
    [ T('Settings (inputs)') ],
    [ W('Yellow cells are the knobs the design refers to. Everything else on the other tabs is calculated from these and the game data.') ],
    [ H('Setting', 'Value', 'What it does') ],
    [ 'Tier spread', I(1), W('How many tiers away from an area\'s tier a monster may come from. 0 strict, 1 recommended, 2 wild, 6 = anything (Merrow).') ],
    [ 'Scaling strength k', I(1), W('0 = keep home stats, 0.5 = halfway to the destination area\'s average, 1 = full fit. EXP always uses 1.') ],
    [ 'Max fliers per area', I(2), W('Upper bound on flying monsters in one roster; tier 1 gets none.') ],
    [ 'Dangerous: keep at native tier or above', I('Y'), W('Cockatrice / Flamed Mane never appear earlier than vanilla.') ],
    [ 'Bulk: DEF weight', I(25), W('Bulk = HP * (1 + DEF / this). Larger = DEF matters less.') ],
    [ 'Threat: AGI weight', I(50), W('Threat = ATK * (1 + AGI / this). Larger = AGI matters less.') ],
    [],
    [ H('Power tier thresholds', 'Power at least', 'Tier') ],
    [ 'T2 from', I(30), 2 ],
    [ 'T3 from', I(90), 3 ],
    [ 'T4 from', I(180), 4 ],
    [ 'T5 from', I(260), 5 ],
    [ 'T6 from', I(330), 6 ],
    [ 'T7 from', I(400), 7 ],
    [ W('Power = SQRT(Bulk * Threat). Thresholds were chosen so the vanilla areas land on their story tiers; adjust and watch Monsters column P.') ],
    [],
    [ H('Stat caps', 'Cap', '') ],
    [ 'HP cap', I(999), W('Vanilla monsters stay under 999; Hard Mode raises Brian\'s caps to 999 too.') ],
    [ 'ATK/DEF/AGI cap', I(255), W('Stored as bytes in the game\'s battle struct.') ],
);

# ---------------------------------------------------------------- Areas
my @areas_rows = (
    [ T('Areas: progression order, tier, vanilla roster and averages') ],
    [ W('Tier (C) is an input. Averages (G-K) are over the area\'s vanilla roster (Roster tab). Allowed tiers (M-N) follow Settings!B4. O counts how many monsters could be placed here under the current settings; P is the vanilla roster size the randomizer must fill.') ],
    [ H('Rank', 'Area', 'Tier', 'Monster table', 'Roster size', 'Vanilla roster', 'Avg HP', 'Avg ATK', 'Avg DEF', 'Avg AGI', 'Avg EXP', 'Avg Power', 'Allowed tier min', 'Allowed tier max', 'Candidate monsters', 'Packs (vanilla: monster x always + extra)', 'Notes') ],
);
my $first_area_row = 4;
my $rank = 0;
my @area_order;
for my $p (@progression) {
    $rank++;
    my ($name, $tier, $note) = @$p;
    my $e = $area{$name};
    my $r = $first_area_row + $rank - 1;
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
        F("MAX(1,C$r-Settings!\$B\$4)", 0),
        F("MIN(7,C$r+Settings!\$B\$4)", 0),
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
    [ W('Stats are the ROM\'s (HP, ATK, DEF, AGI, EXP; Drop is the item drop slot). Native tier comes from the home area\'s tier on the Areas tab. Flying is the ROM\'s own flag (only Wyvern, Will-o\'-Wisp and Pixie carry it), not a visual judgement. Power/Power tier use the Settings weights and thresholds. Q-R are the area tiers this monster may be placed in under the current spread; dangerous monsters are pinned at or above their native tier. S is where the six stat halfwords live in the ROM.') ],
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
        F("E$mr*(1+G$mr/Settings!\$B\$8)"),
        F("F$mr*(1+H$mr/Settings!\$B\$9)"),
        F("ROUND(SQRT(M$mr*N$mr),1)"),
        F("1+COUNTIF(Settings!\$B\$12:\$B\$17,\"<=\"&O$mr)", 0),
        F("IF(AND(L$mr=\"Y\",Settings!\$B\$7=\"Y\"),D$mr,MAX(1,D$mr-Settings!\$B\$4))", 0),
        F("MIN(7,D$mr+Settings!\$B\$4)", 0),
        $m->{addr},
    ];
}

# ---------------------------------------------------------------- Scaling example
my @ex = (
    [ T('Scaling example: one monster dropped into one area') ],
    [ W('Pick a monster and a destination area (yellow). The table shows its home stats, the two areas\' vanilla averages, the factor between them, and the rescaled stats under Settings!B5. The grid below previews the same monster in every area.') ],
    [ 'Monster', I('RED WYVERN') ],
    [ 'Destination area', I('Holy Plains') ],
    [ 'Home area', F('INDEX(Monsters!$C:$C,MATCH($B$3,Monsters!$B:$B,0))', 0) ],
    [ 'Home tier', F('INDEX(Monsters!$D:$D,MATCH($B$3,Monsters!$B:$B,0))', 0), 'Destination tier', F('INDEX(Areas!$C:$C,MATCH($B$4,Areas!$B:$B,0))', 0) ],
    [ 'Within allowed spread?', F('IF(AND(D6>=INDEX(Monsters!$Q:$Q,MATCH($B$3,Monsters!$B:$B,0)),D6<=INDEX(Monsters!$R:$R,MATCH($B$3,Monsters!$B:$B,0))),"yes","no (outside spread)")', 0) ],
    [],
    [ H('Stat', 'Own (home)', 'Home area avg', 'Destination avg', 'Factor', 'k used', 'Scaled', 'Cap') ],
);
my @statcols = ( [ 'HP', 'E', 'G', 'Settings!$B$21' ], [ 'ATK', 'F', 'H', 'Settings!$B$22' ], [ 'DEF', 'G', 'I', 'Settings!$B$22' ], [ 'AGI', 'H', 'J', 'Settings!$B$22' ], [ 'EXP', 'I', 'K', '99999' ] );
my $er = 9;
for my $sc (@statcols) {
    $er++;
    my ($label, $mcol, $acol, $cap) = @$sc;
    my $k = $label eq 'EXP' ? '1' : 'Settings!$B$5';
    push @ex, [
        $label,
        F("INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))", 0),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0))"),
        F("INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$4,Areas!\$B:\$B,0))"),
        F("D$er/C$er"),
        F($k),
        F("MIN(H$er,ROUND(B$er*E$er^F$er,0))", 0),
        F($cap, 0),
    ];
}
push @ex, [], [ S('Preview: the chosen monster in every area') ],
    [ H('Area', 'Tier', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Allowed?') ];
my $pr = $er + 3;
for my $name (@area_order) {
    $pr++;
    my @cells = ( $name, F("INDEX(Areas!\$C:\$C,MATCH(\$A$pr,Areas!\$B:\$B,0))", 0) );
    my $ci = 2;
    for my $sc (@statcols) {
        my ($label, $mcol, $acol, $cap) = @$sc;
        my $k = $label eq 'EXP' ? '1' : 'Settings!$B$5';
        push @cells, F("MIN($cap,ROUND(INDEX(Monsters!\$$mcol:\$$mcol,MATCH(\$B\$3,Monsters!\$B:\$B,0))*(INDEX(Areas!\$$acol:\$$acol,MATCH(\$A$pr,Areas!\$B:\$B,0))/INDEX(Areas!\$$acol:\$$acol,MATCH(\$B\$5,Areas!\$B:\$B,0)))^$k,0))", 0);
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
    [ W('src/game/randomizer/randomizer.cpp shuffle_enemies() already owns the table index / pack writes; add the tier pool selection there and the stat writes next to the existing monster_stats writes. Options: enemy_progression (bool), enemy_tier_spread (int), enemy_scale_strength (0/50/100 %), enemy_max_fliers. UI: three controls under the existing "Enemy shuffling" group on the Randomizer tab. The tier table itself can be a constant array (one tier per area, from the Areas tab).') ],
    [ S('Open questions') ],
    [ W('Should the spread be asymmetric (allow one tier down, two up) so the game gets harder rather than easier? Should exp scale with k or always fully (this sheet says always)? Do the story bosses\' areas (Connor, Windward, Blue Cave, Boil Hole, Moor, Brannoch) need the boss\'s own minions kept vanilla for the fight leading up to them?') ],
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
