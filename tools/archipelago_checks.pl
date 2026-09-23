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


use lib dirname(__FILE__);
use Quest64Checks;

my $checks = Quest64Checks::gather($root);
my @items   = @{ $checks->{items} };
my @chests  = @{ $checks->{chests} };
my @givers  = @{ $checks->{givers} };
my @monsters = @{ $checks->{monsters} };
my @bosses  = @{ $checks->{bosses} };
my @spirits = @{ $checks->{spirits} };
my @stage   = @{ $checks->{stage} };
my %spirit_notes = %{ $checks->{spirit_notes} };

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
