#!/usr/bin/perl
# Writes DOCS/archipelago_checks.xlsx: every location an Archipelago world
# for Quest 64 could hand out an item from, read from the Merrow tables the
# randomizer ships (src/game/randomizer/merrow_data.cpp, merrow_mapdata.cpp)
# the enemy progression data and, for the spirits, the ROM itself
# (quest64.us.z64 in the repo root), so the sheet is the ROM's, not typed in.
#
#   perl tools/archipelago_checks.pl            (also copies to D:\Downloads)
#
# No spreadsheet library on this machine, so the workbook is written as the
# raw OOXML zip, the same way tools/enemyrandologic.pl does it.
use strict;
use warnings;
use File::Basename qw(dirname);
use File::Copy qw(copy);
use IO::Compress::Zip qw(zip $ZipError);

my $root = dirname(__FILE__) . '/..';
my $out  = "$root/DOCS/archipelago_checks.xlsx";
my $copy = 'D:/Downloads/archipelago_checks.xlsx';

my ($data, $map, $prog, $rom);
{
    local $/;
    open my $f, '<', "$root/src/game/randomizer/merrow_data.cpp" or die $!;
    $data = <$f>; close $f;
    open $f, '<', "$root/src/game/randomizer/merrow_mapdata.cpp" or die $!;
    $map = <$f>; close $f;
    open $f, '<', "$root/src/game/randomizer/enemy_progression_data.cpp" or die $!;
    $prog = <$f>; close $f;
    open $f, '<:raw', "$root/quest64.us.z64" or die "quest64.us.z64: $!";
    $rom = <$f>; close $f;
}
$_ =~ s/\r//g for ($data, $map, $prog);

sub table { my ($src, $name) = @_; my ($body) = ($src =~ /\b\Q$name\E = \{(.*?)\n\s*\};/s); die "no table $name" unless defined $body; return $body }
sub strings { my $b = shift; return ($b =~ /"([^"]*)"/g) }
sub numbers { my $b = shift; $b =~ s{//[^\n]*}{}g; return ($b =~ /(\d+)/g) }

# ---------------------------------------------------------------- items
my @item_raw = strings(table($data, 'items'));
my @items;
for (my $i = 0; $i + 2 < @item_raw; $i += 3) { push @items, nice($item_raw[$i]) }
sub item_name { my $id = shift; return 'nothing' if $id == 255; return $items[$id] // "item $id" }
sub nice { my $s = shift; $s =~ s/`/'/g; return join ' ', map { ucfirst lc } split / /, $s }

# ---------------------------------------------------------------- chests
# chestdata: ROM address of the chest record (item byte at +33), vanilla
# item, Merrow's logical path order, "boss region" (how many bosses the
# vanilla route has beaten by the time the chest is reachable).
my @chest_names = strings(table($data, 'chestlocnames'));
my @chest_vals  = numbers(table($data, 'chestdata'));
die "chest tables disagree" unless @chest_vals == @chest_names * 4;
my @stage = ('Start', 'After Solvaring (Earth Orb)', 'After Zelse (Wind Jade)', 'After Nepty (Water Jewel)',
             'After Shilf', 'After Fargo (Fire Ruby)');
my @chests;
for my $i (0 .. $#chest_names) {
    my ($addr, $item, $order, $region) = @chest_vals[ $i * 4 .. $i * 4 + 3 ];
    (my $area = $chest_names[$i]) =~ s/ #\d+$//;
    push @chests, { idx => $i, name => $chest_names[$i], area => $area, item => item_name($item), item_id => $item,
                    order => $order, region => $region, addr => $addr };
}

# ---------------------------------------------------------------- givers
my @gifter_names = strings(table($data, 'gifternames'));
my @granters = numbers(table($data, 'itemgranters'));
my @givers;
for my $i (0 .. $#gifter_names) {
    my ($addr, $item) = @granters[ $i * 2, $i * 2 + 1 ];
    my ($who, $where) = ($gifter_names[$i] =~ /^(.*?) \((.*)\)$/);
    my $kind = $i >= 10 ? 'Wingsmith' : $i >= 8 ? 'Shannon (endgame)' : 'Gift NPC';
    push @givers, { idx => $i, who => $who, where => $where, kind => $kind, item => item_name($item), item_id => $item, addr => $addr };
}

# ---------------------------------------------------------------- monsters
my @mon_raw = strings(table($data, 'monsternames'));
my @monster_names;
for (my $i = 0; $i + 1 < @mon_raw; $i += 2) { push @monster_names, nice($mon_raw[$i]) }
my @stats = numbers(table($data, 'monsterstatvanilla'));
my @drops = numbers(table($data, 'dropdata'));
die "monster tables disagree" unless @monster_names == 75 && @stats >= 450 && @drops == 150;

# Areas and which monsters each one rolls, from the encounter packs.
my @tables;
my $mt = table($map, 'monster_tables');
while ($mt =~ /\{ 0x[0-9A-F]+, \{ (.*?) \} \}/g) { my @e = ($1 =~ /"([^"]*)"/g); push @tables, \@e }
my @packs;
my $pk = table($map, 'packs');
while ($pk =~ /\{ 0x[0-9A-F]+, \{ (.*?) \} \},?\n/g) {
    my @mem; my $s = $1;
    while ($s =~ /\{ (\d+), (\d+), (\d+) \}/g) { push @mem, $1 }
    push @packs, \@mem;
}
my @areas;
my $ar = table($map, 'areas');
while ($ar =~ /\{ "([^"]+)", \{ (?:0x[0-9A-F]+, ){6}0x([0-9A-F]+), 0x[0-9A-F]+ \}, (\d+), (\d+), (\d+), (\d+) \}/g) {
    push @areas, { name => $1, table => hex($2), ps => $3, pc => $4 };
}
die "unexpected map data (@{[scalar @tables]} tables, @{[scalar @areas]} areas)" unless @tables == 6 && @areas == 27;
my %id_of; $id_of{ uc $monster_names[$_] } = $_ for 0 .. $#monster_names;
my %areas_of;   # monster id -> { area name => 1 }
for my $a (@areas) {
    (my $base = $a->{name}) =~ s/ \d+$//;
    my $names = $tables[ $a->{table} ];
    for my $p ($a->{ps} .. $a->{ps} + $a->{pc} - 1) {
        for my $slot (@{ $packs[$p] }) {
            my $id = $id_of{ $names->[$slot] };
            $areas_of{$id}{$base} = 1 if defined $id;
        }
    }
}
# Story order of the areas, from the progression data.
my %tier; my $n = 0;
my $pa = table($prog, 'areas');
while ($pa =~ /\{ "([^"]+)", (\d+), (\d+),/g) { $tier{$1} = { order => $n++, tier => $3, map => $2 } }
my %prog_flags;
my $pm = table($prog, 'monsters');
while ($pm =~ /\{ "([^"]+)", (\d+), (true|false), (true|false),/g) { $prog_flags{ nice($1) } = { dangerous => $3, flying => $4 } }

my @monsters;
for my $i (0 .. 66) {
    my @s = @stats[ $i * 6 .. $i * 6 + 5 ];
    my @where = sort { ($tier{$a}{order} // 99) <=> ($tier{$b}{order} // 99) } keys %{ $areas_of{$i} || {} };
    my $f = $prog_flags{ $monster_names[$i] } || {};
    push @monsters, { id => $i, name => $monster_names[$i], areas => \@where,
                      first => $where[0] // '', tier => ($where[0] ? $tier{ $where[0] }{tier} : ''),
                      hp => $s[0], atk => $s[1], def => $s[2], agi => $s[3], exp => $s[4],
                      drop => item_name($drops[ $i * 2 + 1 ]), drop_id => $drops[ $i * 2 + 1 ], drop_addr => $drops[ $i * 2 ],
                      flying => ($f->{flying} // '') eq 'true' ? 'Y' : 'N', dangerous => ($f->{dangerous} // '') eq 'true' ? 'Y' : 'N' };
}

# ---------------------------------------------------------------- bosses
# Ids 67-74 in the monster list; the vanilla order, arenas and rewards are
# the game's (the reward ids match randomizer.cpp's vanilla_boss_items).
my @boss_info = (
    [ 'Solvaring', 'Connor Forest (Solvaring\'s Lair)', 20, 'Earth Orb', 1 ],
    [ 'Zelse',     'Windward Forest',                    21, 'Wind Jade', 2 ],
    [ 'Nepty',     'Blue Cave',                          22, 'Water Jewel', 3 ],
    [ 'Shilf',     'Baragoon Tunnel',                    255, '', 4 ],
    [ 'Fargo',     'Boil Hole',                          23, 'Fire Ruby', 5 ],
    [ 'Guilty',    'Brannoch Castle',                    255, '', 6 ],
    [ 'Beigis',    'Brannoch Castle roof (shrine)',      255, '', 7 ],
    [ 'Mammon',    'Mammon\'s World',                    255, '(ending)', 8 ],
);
my @bosses;
for my $k (0 .. 7) {
    my $i = 67 + $k;
    my @s = @stats[ $i * 6 .. $i * 6 + 5 ];
    my ($name, $arena, $item, $reward, $order) = @{ $boss_info[$k] };
    die "boss $k is $monster_names[$i], not $name" unless lc $monster_names[$i] eq lc $name;
    push @bosses, { id => $i, name => $name, arena => $arena, order => $order, reward => $reward, reward_id => $item,
                    hp => $s[0], atk => $s[1], def => $s[2], agi => $s[3], exp => $s[4],
                    drop => item_name($drops[ $i * 2 + 1 ]), drop_addr => $drops[ $i * 2 ] };
}

# ---------------------------------------------------------------- spirits
# Not in Merrow. Each map file ends with its spirit list: 12-byte records of
# { f32 x, f32 z, u16 id << 8, u16 0 } (some maps pad them to 16), the ids
# unique across the game and consecutive within a map. 98 in all, ids 0-97,
# which is also the cap on the game's collected-spirit counter. The map
# table at 0x80054F10 (0x44 bytes a map, ROM range at +4/+8) says which
# file is which; names are src/game/map_table.cpp's.
my @map_names = (
    'Melrode', 'Dondoran', 'Holy Plains', 'Dondoran Flats', 'Larapool', 'West Carmagh (Greenoch)', 'Normoon',
    'West Limelin', 'Limelin', 'Dindom Dries', 'Shamwood', 'Brannoch / Baragoon Moor', 'Isle of Skye',
    'Melrode Monastery', 'Dondoran Castle', 'Melrode buildings', 'Dondoran buildings', 'Larapool buildings',
    'Larapool & Greenoch buildings', 'Normoon buildings (small set)', 'Normoon buildings', 'Limelin Castle',
    'Limelin buildings', 'Dindom Dries & Brannoch buildings', 'Shamwood buildings', 'Hidden rooms & shrines',
    'Blue Cave', 'Cull Hazard', 'Baragoon Tunnel (Shilf)', 'Boil Hole (Fargo)', 'Brannoch Castle',
    'Connor Forest (Solvaring)', 'Glencoe Forest', 'Windward Forest (Zelse)', "Mammon's World", "Nepty's arena (Isle of Skye)",
);
my %spirit_notes = (
    83 => 'Hidden inside the return ship (Limelin to Dondoran); "Reveal hidden spirits" moves it out',
    97 => 'Hidden inside the Limelin Castle library; "Reveal hidden spirits" moves it out',
);
my $spirit_count = 98;
my $map_table = 0x80054F10 - 0x80000450 + 0x1050;
my $sane = sub { my $v = shift; return 0 unless $v == $v; return 1 if $v == 0; return abs($v) >= 0.01 && abs($v) < 20000 };
my (@spirits, @runs);
for my $m (0 .. 35) {
    my ($start, $end) = unpack('NN', substr($rom, $map_table + $m * 0x44 + 4, 8));
    my @found;
    for (my $o = $end - 0x400; $o + 12 <= $end; $o += 4) {
        my ($x, $z, $id, $pad) = unpack('f>f>nn', substr($rom, $o, 12));
        next unless $pad == 0 && ($id & 0xFF) == 0 && ($id >> 8) < $spirit_count;
        next unless $sane->($x) && $sane->($z);
        push @found, { id => $id >> 8, x => $x, z => $z, addr => $o };
    }
    # Group into runs of consecutive ids (records sit 12-24 bytes apart);
    # other data can look like a record too, so the runs are settled below.
    my @run;
    for my $e (@found) {
        if (@run && $e->{id} == $run[-1]{id} + 1 && $e->{addr} - $run[-1]{addr} <= 24) { push @run, $e }
        else { push @runs, [ $m, @run ] if @run; @run = ($e) }
    }
    push @runs, [ $m, @run ] if @run;
}
# Ids are unique across the game, so the longest runs are the real lists
# and anything that collides with one of them is not a spirit.
my %claimed;
for my $r (sort { @$b <=> @$a || $a->[0] <=> $b->[0] } @runs) {
    my ($m, @recs) = @$r;
    next if grep { $claimed{ $_->{id} } } @recs;
    $claimed{ $_->{id} } = 1 for @recs;
    push @spirits, { %$_, map => $m, map_name => $map_names[$m] } for @recs;
}
@spirits = sort { $a->{id} <=> $b->{id} } @spirits;
my %nth;
$_->{nth} = ++$nth{ $_->{map} } for @spirits;
{
    my %ids = map { $_->{id} => 1 } @spirits;
    my @missing = grep { !$ids{$_} } 0 .. $spirit_count - 1;
    die "spirit scan: @{[scalar @spirits]} found, missing @missing" if @missing || @spirits != $spirit_count;
}

# ---------------------------------------------------------------- xlsx bits
sub esc { my $s = shift; $s =~ s/&/&amp;/g; $s =~ s/</&lt;/g; $s =~ s/>/&gt;/g; $s =~ s/"/&quot;/g; return $s }
sub col { my $n = shift; my $s = ''; while ($n > 0) { my $r = ($n - 1) % 26; $s = chr(65 + $r) . $s; $n = int(($n - 1) / 26) } $s }
sub sheet_xml {
    my ($rows, $widths, $freeze, $filter_cols) = @_;
    my $x = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
        '<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">';
    $x .= '<sheetViews><sheetView workbookViewId="0">';
    $x .= "<pane ySplit=\"$freeze\" topLeftCell=\"A" . ($freeze + 1) . "\" activePane=\"bottomLeft\" state=\"frozen\"/>" if $freeze;
    $x .= '</sheetView></sheetViews>';
    if ($widths && @$widths) {
        $x .= '<cols>';
        $x .= sprintf('<col min="%d" max="%d" width="%s" customWidth="1"/>', $_ + 1, $_ + 1, $widths->[$_]) for 0 .. $#$widths;
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
            my ($v, $s) = ($cell, 0);
            if (ref $cell) { $v = $cell->{v}; $s = $cell->{s} // 0 }
            if (!defined $v || $v eq '') { $x .= "<c r=\"$ref\" s=\"$s\"/>" if $s }
            elsif ($v =~ /^-?\d+(\.\d+)?$/) { $x .= "<c r=\"$ref\" s=\"$s\"><v>$v</v></c>" }
            else { $x .= "<c r=\"$ref\" s=\"$s\" t=\"inlineStr\"><is><t xml:space=\"preserve\">" . esc($v) . '</t></is></c>' }
        }
        $x .= '</row>';
    }
    $x .= '</sheetData>';
    $x .= sprintf('<autoFilter ref="A%d:%s%d"/>', $freeze, col($filter_cols), scalar @$rows) if $freeze && $filter_cols;
    $x .= '</worksheet>';
    return $x;
}
my $styles = <<'XML';
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
<fonts count="4">
<font><sz val="10"/><name val="Arial"/></font>
<font><b/><sz val="10"/><name val="Arial"/></font>
<font><b/><sz val="14"/><name val="Arial"/></font>
<font><b/><sz val="11"/><color rgb="FF1F3864"/><name val="Arial"/></font>
</fonts>
<fills count="3">
<fill><patternFill patternType="none"/></fill>
<fill><patternFill patternType="gray125"/></fill>
<fill><patternFill patternType="solid"><fgColor rgb="FFD9D9D9"/></patternFill></fill>
</fills>
<borders count="1"><border><left/><right/><top/><bottom/><diagonal/></border></borders>
<cellStyleXfs count="1"><xf numFmtId="0" fontId="0" fillId="0" borderId="0"/></cellStyleXfs>
<cellXfs count="5">
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0"/>
<xf numFmtId="0" fontId="1" fillId="2" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="2" fillId="0" borderId="0" xfId="0" applyFont="1"/>
<xf numFmtId="0" fontId="3" fillId="0" borderId="0" xfId="0" applyFont="1"/>
</cellXfs>
<cellStyles count="1"><cellStyle name="Normal" xfId="0" builtinId="0"/></cellStyles>
</styleSheet>
XML
sub H { map { { v => $_, s => 1 } } @_ }
sub W { { v => $_[0], s => 2 } }
sub T { { v => $_[0], s => 3 } }
sub S { { v => $_[0], s => 4 } }

# ---------------------------------------------------------------- sheets
my $hex = sub { sprintf '0x%06X', $_[0] };

my @overview = (
    [ T('Quest 64 - Archipelago checks') ],
    [ W('Every location the game can hand an item out from, read out of the Merrow tables this recomp ships (the US ROM\'s data). One row per check. Generated by tools/archipelago_checks.pl; rerun it rather than editing by hand.') ],
    [],
    [ H('Category', 'Checks', 'Sheet', 'Notes') ],
    [ 'Chests', scalar @chests, 'Chests', W('Every treasure chest. "Stage" is how far along the vanilla route the chest is (how many bosses are down); "Path order" is Merrow\'s logical order.') ],
    [ 'Givers', scalar @givers, 'Givers', W('NPCs that hand over an item: the eight gift NPCs, the two endgame Shannons (Eletale\'s Book, Dark Gaol Key) and the six wingsmiths.') ],
    [ 'Defeat Enemies', scalar @monsters, 'Enemies', W('The 67 regular monsters. Each has one drop slot the randomizer can fill; where it appears comes from the encounter packs. "Tier" is the story tier of its earliest area (1 Holy Plains .. 8 Mammon\'s World).') ],
    [ 'Defeat Bosses', scalar @bosses, 'Bosses', W('The eight bosses in story order with their arena and vanilla reward (the four gems, the Fire Ruby via Fargo, and the ending).') ],
    [ 'Spirits', scalar @spirits, 'Spirits', W('Every spirit pickup, read from the spirit list at the end of each map file in the ROM: its map, world position and record address. 98 in all, which is also the cap on the game\'s collected-spirit counter.') ],
    [ 'Total', scalar(@chests) + scalar(@givers) + scalar(@monsters) + scalar(@bosses) + scalar(@spirits) ],
    [],
    [ S('Suggested location names') ],
    [ W('Each sheet has a "Suggested name" column in the form "Category - Place" that is unique across the whole set, so it can be pasted straight into a locations table. Rename freely; the ROM addresses next to them are what a patcher needs.') ],
    [],
    [ S('Vanilla item pool (ids the tables use)') ],
    [ H('Id', 'Item') ],
    map { [ $_, $items[$_] ] } 0 .. $#items,
);

my @chest_rows = (
    [ H('#', 'Suggested name', 'Location', 'Area', 'Vanilla item', 'Item id', 'Stage', 'Stage (vanilla route)', 'Path order', 'ROM address (record)', 'Item byte at') ],
);
for my $c (sort { $a->{order} <=> $b->{order} } @chests) {
    push @chest_rows, [ $c->{idx}, "Chest - $c->{name}", $c->{name}, $c->{area}, $c->{item}, $c->{item_id}, $c->{region}, $stage[ $c->{region} ], $c->{order}, $hex->($c->{addr}), $hex->($c->{addr} + 33) ];
}

my @giver_rows = (
    [ H('#', 'Suggested name', 'NPC', 'Where', 'Kind', 'Vanilla item', 'Item id', 'Item byte at') ],
);
for my $g (@givers) {
    push @giver_rows, [ $g->{idx}, "Giver - $g->{who} ($g->{where})", $g->{who}, $g->{where}, $g->{kind}, $g->{item}, $g->{item_id}, $hex->($g->{addr}) ];
}

my @enemy_rows = (
    [ H('Id', 'Suggested name', 'Monster', 'First area', 'Tier', 'All areas', 'Vanilla drop', 'Drop id', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Flying', 'Dangerous', 'Drop byte at') ],
);
for my $m (@monsters) {
    push @enemy_rows, [ $m->{id}, "Defeat - $m->{name}", $m->{name}, $m->{first}, $m->{tier}, join('; ', @{ $m->{areas} }), $m->{drop}, $m->{drop_id},
                        $m->{hp}, $m->{atk}, $m->{def}, $m->{agi}, $m->{exp}, $m->{flying}, $m->{dangerous}, $hex->($m->{drop_addr}) ];
}

my @boss_rows = (
    [ H('Order', 'Suggested name', 'Boss', 'Arena', 'Vanilla reward', 'Reward id', 'Monster id', 'HP', 'ATK', 'DEF', 'AGI', 'EXP', 'Drop byte at') ],
);
for my $b (@bosses) {
    push @boss_rows, [ $b->{order}, "Boss - $b->{name}", $b->{name}, $b->{arena}, $b->{reward}, ($b->{reward_id} == 255 ? '' : $b->{reward_id}), $b->{id},
                       $b->{hp}, $b->{atk}, $b->{def}, $b->{agi}, $b->{exp}, $hex->($b->{drop_addr}) ];
}

my @spirit_rows = (
    [ H('Id', 'Suggested name', 'Map', 'Map #', 'Nth in map', 'X', 'Z', 'Record at', 'Notes') ],
);
for my $sp (@spirits) {
    push @spirit_rows, [ $sp->{id}, "Spirit - $sp->{map_name} #$sp->{nth}", $sp->{map_name}, $sp->{map}, $sp->{nth}, $sp->{x}, $sp->{z}, $hex->($sp->{addr}), $spirit_notes{ $sp->{id} } // '' ];
}

my @sheets = (
    [ 'Overview', \@overview,   [ 18, 10, 12, 100 ],                                          0, 0 ],
    [ 'Chests',   \@chest_rows, [ 5, 44, 40, 30, 18, 8, 7, 28, 10, 20, 14 ],                   1, 11 ],
    [ 'Givers',   \@giver_rows, [ 5, 40, 12, 22, 18, 18, 8, 14 ],                              1, 8 ],
    [ 'Enemies',  \@enemy_rows, [ 5, 28, 18, 18, 6, 44, 18, 8, 7, 7, 7, 7, 7, 7, 10, 14 ],     1, 16 ],
    [ 'Bosses',   \@boss_rows,  [ 7, 20, 12, 34, 16, 10, 10, 7, 7, 7, 7, 7, 14 ],              1, 13 ],
    [ 'Spirits',  \@spirit_rows, [ 5, 40, 30, 7, 10, 9, 9, 12, 70 ],                          1, 9 ],
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
    '</workbook>';
$files{'xl/_rels/workbook.xml.rels'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' .
    join('', map { sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet%d.xml"/>', $_ + 1, $_ + 1) } 0 .. $#sheets) .
    sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>', @sheets + 1) .
    '</Relationships>';
$files{'xl/styles.xml'} = $styles;
$files{"xl/worksheets/sheet" . ($_ + 1) . ".xml"} = sheet_xml(@{ $sheets[$_] }[1 .. 4]) for 0 .. $#sheets;

my @order = ('[Content_Types].xml', '_rels/.rels', 'xl/workbook.xml', 'xl/_rels/workbook.xml.rels', 'xl/styles.xml',
    map { "xl/worksheets/sheet$_.xml" } 1 .. @sheets);
my $z = IO::Compress::Zip->new($out, Name => $order[0]) or die "zip: $ZipError";
for my $i (0 .. $#order) {
    $z->newStream(Name => $order[$i]) if $i > 0;
    $z->print($files{ $order[$i] });
}
$z->close;
printf "wrote %s: %d chests, %d givers, %d enemies, %d bosses, %d spirits\n", $out, scalar @chests, scalar @givers, scalar @monsters, scalar @bosses, scalar @spirits;
if (-d dirname($copy)) { copy($out, $copy) or die "copy: $!"; print "copied to $copy\n" }
