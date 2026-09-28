#!/usr/bin/perl
# Writes DOCS/archipelago_logic.xlsx: a workbook for planning the Quest 64
# Archipelago world's logic in Archipelago's Rule Builder style
# (rule_builder.rules: Has, HasAll, HasAny, CanReachRegion, CanReachLocation,
# OptionFilter, combined with & and |; see worlds/apquest/rules.py).
#
# Every location, item and option comes from Quest64Checks.pm and the same
# tables tools/archipelago_world.pl uses, so the names here are the names the
# apworld has. The Region and Rule columns start out holding the logic the
# apworld has today; the yellow cells are the ones to edit.
#
#   perl tools/archipelago_logic_sheet.pl
#
# Nothing reads this file back: it is a planning sheet. Once the logic is
# settled it is written into archipelago_world.pl's Regions.py and Rules.py.
use strict;
use warnings;
use File::Basename qw(dirname);
use IO::Compress::Zip qw($ZipError);

use lib dirname(__FILE__);
use Quest64Checks;

my $root = dirname(__FILE__) . '/..';
my $out  = "$root/DOCS/archipelago_logic.xlsx";
# The workbook is now what archipelago_world.pl builds the logic from, and
# may hold edits this script knows nothing about: only a fresh start with
# --force writes over it.
die "$out already exists and is the world's logic; pass --force to start it again from scratch\n"
    if -e $out && !grep { $_ eq '--force' } @ARGV;
my $checks = Quest64Checks::gather($root);

my $BASE = 0x51640000;
my %GROUP = (chest => 0x1000, giver => 0x2000, enemy => 0x3000, boss => 0x4000, spirit => 0x5000, item => 0x0000);
my $LAST = 400;   # how far down the lookup ranges and dropdowns reach

# ---------------------------------------------------------------- xlsx bits
sub esc {
    my $s = shift;
    $s =~ s/&/&amp;/g; $s =~ s/</&lt;/g; $s =~ s/>/&gt;/g; $s =~ s/"/&quot;/g;
    return $s;
}
sub col { my $n = shift; my $s = ''; while ($n > 0) { my $r = ($n - 1) % 26; $s = chr(65 + $r) . $s; $n = int(($n - 1) / 26) } $s }

# Styles: 0 plain, 1 header, 2 input (blue on yellow), 3 wrap, 4 title,
# 5 input wrap, 6 section heading, 7 formula, 8 wrap bold, 9 code (Consolas).
sub sheet_xml {
    my ($rows, $widths, $freeze, %opt) = @_;
    my $x = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
        '<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">';
    $x .= '<sheetViews><sheetView workbookViewId="0">';
    $x .= "<pane ySplit=\"$freeze\" topLeftCell=\"A" . ($freeze + 1) . "\" activePane=\"bottomLeft\" state=\"frozen\"/>" if $freeze;
    $x .= '</sheetView></sheetViews>';
    $x .= '<cols>' . join('', map { sprintf('<col min="%d" max="%d" width="%s" customWidth="1"/>', $_ + 1, $_ + 1, $widths->[$_]) } 0 .. $#$widths) . '</cols>';
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
            elsif ($v =~ /^-?\d+(\.\d+)?$/ && !(ref $cell && $cell->{text})) {
                $x .= "<c r=\"$ref\" s=\"$s\"><v>$v</v></c>";
            }
            else {
                $x .= "<c r=\"$ref\" s=\"$s\" t=\"inlineStr\"><is><t xml:space=\"preserve\">" . esc($v) . '</t></is></c>';
            }
        }
        $x .= '</row>';
    }
    $x .= '</sheetData>';
    $x .= "<autoFilter ref=\"$opt{filter}\"/>" if $opt{filter};
    if ($opt{lists} && @{ $opt{lists} }) {
        $x .= '<dataValidations count="' . scalar(@{ $opt{lists} }) . '">';
        for my $l (@{ $opt{lists} }) {
            $x .= "<dataValidation type=\"list\" allowBlank=\"1\" showErrorMessage=\"0\" sqref=\"$l->[0]\"><formula1>" . esc($l->[1]) . '</formula1></dataValidation>';
        }
        $x .= '</dataValidations>';
    }
    $x .= '</worksheet>';
    return $x;
}

my $styles = <<'XML';
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
<fonts count="6">
<font><sz val="10"/><name val="Arial"/></font>
<font><b/><sz val="10"/><name val="Arial"/></font>
<font><sz val="10"/><color rgb="FF0000FF"/><name val="Arial"/></font>
<font><b/><sz val="14"/><name val="Arial"/></font>
<font><b/><sz val="11"/><color rgb="FF1F3864"/><name val="Arial"/></font>
<font><sz val="10"/><color rgb="FF0000FF"/><name val="Consolas"/></font>
</fonts>
<fills count="4">
<fill><patternFill patternType="none"/></fill>
<fill><patternFill patternType="gray125"/></fill>
<fill><patternFill patternType="solid"><fgColor rgb="FFD9D9D9"/></patternFill></fill>
<fill><patternFill patternType="solid"><fgColor rgb="FFFFFF00"/></patternFill></fill>
</fills>
<borders count="1"><border><left/><right/><top/><bottom/><diagonal/></border></borders>
<cellStyleXfs count="1"><xf numFmtId="0" fontId="0" fillId="0" borderId="0"/></cellStyleXfs>
<cellXfs count="10">
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0"/>
<xf numFmtId="0" fontId="1" fillId="2" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="2" fillId="3" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment vertical="top"/></xf>
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="3" fillId="0" borderId="0" xfId="0" applyFont="1"/>
<xf numFmtId="0" fontId="2" fillId="3" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="4" fillId="0" borderId="0" xfId="0" applyFont="1"/>
<xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0" applyAlignment="1"><alignment vertical="top"/></xf>
<xf numFmtId="0" fontId="1" fillId="0" borderId="0" xfId="0" applyFont="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
<xf numFmtId="0" fontId="5" fillId="3" borderId="0" xfId="0" applyFont="1" applyFill="1" applyAlignment="1"><alignment wrapText="1" vertical="top"/></xf>
</cellXfs>
<cellStyles count="1"><cellStyle name="Normal" xfId="0" builtinId="0"/></cellStyles>
</styleSheet>
XML

sub H  { map { { v => $_, s => 1 } } @_ }   # header
sub I  { { v => $_[0], s => 2 } }           # input
sub IW { { v => $_[0], s => 5 } }           # input, wrapped
sub C  { { v => $_[0], s => 9 } }           # input, a Rule Builder expression
sub W  { { v => $_[0], s => 3 } }           # wrapped text
sub T  { { v => $_[0], s => 4 } }           # title
sub S  { { v => $_[0], s => 6 } }           # section heading
sub B  { { v => $_[0], s => 8 } }           # bold, wrapped
sub F  { { f => $_[0], s => 7 } }           # formula
sub X  { { v => $_[0], s => 7, text => 1 } }   # text that looks like a number

sub py_str { my $s = shift; $s =~ s/\\/\\\\/g; $s =~ s/"/\\"/g; return "\"$s\"" }

# ---------------------------------------------------------------- the logic today
# The same regions, gates and option switches as archipelago_world.pl.
my @regions = (
    [ 'Menu',      "Archipelago's start region. Leads straight to the Overworld." ],
    [ 'Overworld', 'Melrode and everything open from the start: Holy Plains, Connor Forest, the spirits, the gift NPCs.' ],
    [ 'Boss 1',    'Solvaring (Connor Forest).' ],
    [ 'Early',     'What opens once the first gems are in: Dondoran, Glencoe, West Carmaugh.' ],
    [ 'Boss 2',    'Zelse (Windward Forest). Behind the Earth Orb.' ],
    [ 'Boss 3',    'Nepty (Blue Cave). Behind the Wind Jade.' ],
    [ 'Mid',       'Isle of Skye, Baragoon Tunnel, Dindom Dries. Behind the Wind Jade.' ],
    [ 'Boss 4',    'Shilf (Baragoon Tunnel). Behind the Water Jewel.' ],
    [ 'Boss 5',    'Fargo (Boil Hole). Behind the Water Jewel.' ],
    [ 'Late',      'Baragoon Moor, Brannoch Castle. Behind the Fire Ruby.' ],
    [ 'Boss 6',    'Guilty (Brannoch Castle). Behind the Fire Ruby.' ],
    [ 'Boss 7',    'Beigis (Brannoch Castle roof). Behind the Fire Ruby.' ],
    [ 'Endgame',   "Mammon's World. Behind Eletale's Book, or the mammon_portal condition." ],
    [ 'Boss 8',    "Mammon. Behind the Dark Gaol Key." ],
);
my %exits = (
    'Menu' => ['Overworld'], 'Overworld' => ['Boss 1', 'Early'], 'Boss 1' => ['Boss 2'], 'Early' => ['Boss 2'],
    'Boss 2' => ['Boss 3'], 'Boss 3' => ['Mid', 'Boss 4'], 'Mid' => ['Boss 5'], 'Boss 4' => ['Boss 5'],
    'Boss 5' => ['Late', 'Boss 6'], 'Late' => ['Boss 7'], 'Boss 6' => ['Boss 7'], 'Boss 7' => ['Endgame'],
    'Endgame' => ['Boss 8'], 'Boss 8' => [],
);
my %gate = (
    'Boss 2' => 'EARTH_ORB_GATE', 'Boss 3' => 'WIND_JADE_GATE', 'Mid' => 'WIND_JADE_GATE',
    'Boss 4' => 'WATER_JEWEL_GATE', 'Boss 5' => 'WATER_JEWEL_GATE', 'Boss 6' => 'FIRE_RUBY_GATE',
    'Boss 7' => 'FIRE_RUBY_GATE', 'Late' => 'FIRE_RUBY_GATE', 'Endgame' => 'ENDGAME_DOOR',
    'Boss 8' => 'Has("Dark Gaol Key")',
);
my @arena = ('Connor Forest', 'Windward Forest', 'Blue Cave', 'Baragoon Tunnel', 'Boil Hole',
             'Brannoch Castle', 'Brannoch Castle roof', "Mammon's World");

sub region_for_stage { my $r = shift; return $r >= 5 ? 'Late' : $r >= 3 ? 'Mid' : $r >= 1 ? 'Early' : 'Overworld' }
sub region_for_tier  { my $t = shift; return '' eq ($t // '') ? 'Overworld' : $t >= 7 ? 'Late' : $t >= 4 ? 'Mid' : $t >= 2 ? 'Early' : 'Overworld' }

# ---------------------------------------------------------------- locations
my @bosses = @{ $checks->{bosses} };
my @loc;
for my $c (@{ $checks->{chests} }) {
    push @loc, [ Quest64Checks::chest_name($c), $BASE + $GROUP{chest} + $c->{idx}, 'Chest', $c->{area},
                 'chestsanity', region_for_stage($c->{region}), '', "Vanilla: $c->{item}" ];
}
for my $g (@{ $checks->{givers} }) {
    my $region = $g->{kind} ne 'Shannon (endgame)' ? 'Overworld'
               : $g->{item} eq "Eletale's Book"    ? 'Boss 7'
               :                                     'Endgame';
    my $when = $g->{item} eq "Eletale's Book" ? 'giftsanity, or mammon_portal not vanilla' : 'giftsanity';
    push @loc, [ Quest64Checks::giver_name($g), $BASE + $GROUP{giver} + $g->{idx}, 'Gift NPC', $g->{where},
                 $when, $region, '', "Vanilla: $g->{item}" ];
}
for my $m (@{ $checks->{monsters} }) {
    push @loc, [ Quest64Checks::enemy_name($m), $BASE + $GROUP{enemy} + $m->{id}, 'Monster', $m->{first} || '(unplaced)',
                 'enemysanity', region_for_tier($m->{tier}), '', 'Any one kill counts; first met in this area' ];
}
for my $b (@bosses) {
    my $soul = "$b->{name}'s Soul";
    my $rule = $b->{order} >= 8
        ? "Has(\"$soul\") | OptionFilter(BossSouls, 2, operator=\"lt\")"
        : "Has(\"$soul\") | OptionFilter(BossSouls, 0)";
    push @loc, [ Quest64Checks::boss_name($b), $BASE + $GROUP{boss} + $b->{order}, 'Boss', $arena[ $b->{order} - 1 ],
                 'always', 'Boss ' . $b->{order}, $rule, "Vanilla: $b->{reward}. The Soul matters only with boss_souls on" ];
}
for my $s (@{ $checks->{spirits} }) {
    push @loc, [ Quest64Checks::spirit_name($s), $BASE + $GROUP{spirit} + $s->{id}, 'Spirit', $s->{map_name},
                 'spiritsanity', 'Overworld', '', 'Vanilla: Level Up' ];
}

# ---------------------------------------------------------------- items
my %progression = map { $_ => 1 } ("Earth Orb", "Wind Jade", "Water Jewel", "Fire Ruby", "Eletale's Book", "Dark Gaol Key");
my %useful = map { $_ => 1 } ('White Wings', 'Yellow Wings', 'Blue Wings', 'Green Wings', 'Red Wings', 'Black Wings');
my @item_rows;
push @item_rows, [ 'Level Up', $BASE + 0xF00, 'progression', scalar @{ $checks->{spirits} },
                   'spiritsanity, or extra_level_ups > 0', 'The element-choice screen a spirit used to give' ];
for my $b (@bosses) {
    push @item_rows, [ "$b->{name}'s Soul", $BASE + 0xE00 + $b->{order}, 'progression', 1,
                       $b->{order} >= 8 ? 'boss_souls = with_mammon' : 'boss_souls not off',
                       "Until it arrives $b->{name} is not in the game" ];
}
my @items = @{ $checks->{items} };
for my $i (0 .. $#items) {
    my $n = $items[$i];
    next if $n eq 'Nothing' || $n eq '';
    my $type = $progression{$n} ? 'progression' : $useful{$n} ? 'useful' : 'filler';
    my $count = $progression{$n} ? 1 : $useful{$n} ? 2 : 0;
    my $when = $n eq "Eletale's Book" ? 'mammon_portal = vanilla' : 'always';
    push @item_rows, [ $n, $BASE + $i, $type, $count, $when, $count ? '' : 'Pads the pool out as filler' ];
}

# ---------------------------------------------------------------- Guide
my @guide = (
    [ T('Quest 64 Recompiled - Archipelago logic planner') ],
    [ W('A place to lay out the world\'s logic before it is written into rules.py with Archipelago\'s Rule Builder. Every sheet starts out holding the logic the apworld has today (tools/archipelago_world.pl), so it doubles as a record of it.') ],
    [],
    [ S('How to fill it in') ],
    [ W('Yellow cells are the ones to edit; everything else is looked up or counted from them. Blue text on yellow is an input. Rule cells are Rule Builder expressions, written the way worlds/apquest/rules.py writes them.') ],
    [ W('1. Regions: one row per region. A region is a place a set of checks share, not a rule; rules go on the entrances between regions.') ],
    [ W('2. Entrances: one row per connection, From -> To, with the rule the player must meet to go through it. AP assumes the way back is always open.') ],
    [ W('3. Locations: every check the world has, with the region it sits in and, only where reaching the region is not enough, a rule of its own (a boss needing his Soul, for instance).') ],
    [ W('4. Named Rules: rules used in more than one place, given a name, as APQuest does with HAS_KEY. Write the name in a Rule cell and define it once here.') ],
    [ W('5. Items and Options are for reference. Items counts how many rules mention each item, so an item marked progression that no rule uses stands out, and one used in a rule but classed filler would make seeds unwinnable.') ],
    [],
    [ S('Rule Builder cheat sheet (from rule_builder.rules and rule_builder.options)') ],
    [ H('Write', 'Means', 'Example') ],
    [ W('Has(item)'), W('Has one of the item.'), W('Has("Earth Orb")') ],
    [ W('Has(item, count=n)'), W('Has at least n of it.'), W('Has("Level Up", count=10)') ],
    [ W('HasAll(a, b, ...)'), W('Has every one of them.'), W('HasAll("Fire Ruby", "Water Jewel")') ],
    [ W('HasAny(a, b, ...)'), W('Has at least one of them.'), W('HasAny("Red Wings", "Black Wings")') ],
    [ W('HasGroup(group)'), W('Has one item from an item group (item_name_groups).'), W('HasGroup("Gems")') ],
    [ W('CanReachRegion(name)'), W('The region is reachable. Rule Builder registers the indirect condition itself.'), W('CanReachRegion("Late")') ],
    [ W('CanReachLocation(name)'), W('The location is reachable (its region and its own rule).'), W('CanReachLocation("Boss - Fargo")') ],
    [ W('OptionFilter(Option, value, operator=...)'), W('True when the yaml option compares true: operator eq (default), ne, gt, ge, lt, le, contains. Only means something inside & or |.'), W('OptionFilter(BossSouls, 1, operator="ge")') ],
    [ W('True_() / False_()'), W('Always / never.'), W('True_()') ],
    [ W('a & b'), W('Both.'), W('Has("Fire Ruby") & Has("Water Jewel")') ],
    [ W('a | b'), W('Either.'), W('Has("Earth Orb") | OptionFilter(BossSouls, 1, operator="ge")') ],
    [ W('(blank)'), W('No rule of its own: reaching the region is enough.'), W('') ],
    [],
    [ S('How the sheet becomes code') ],
    [ W('Regions -> Region(name, player, multiworld); Entrances -> from_region.connect(to_region, "From to To") and world.set_rule(world.get_entrance("From to To"), rule); Locations -> region.add_locations(...) and world.set_rule(world.get_location(name), rule) where a rule is given; completion -> world.set_completion_rule(rule).') ],
    [ W('Named Rules become module-level constants in rules.py. Option classes (BossSouls, MammonPortal, ...) are the ones in Options.py, listed on the Options sheet.') ],
    [ W('If every rule is a Rule Builder rule, the world class can subclass CachedRuleBuilderWorld instead of World for faster generation (APQuest\'s note: try it and time a few seeds).') ],
    [],
    [ S('Completion') ],
    [ B('Completion rule'), C('CanReachLocation("Boss - Mammon")') ],
    [ W('Today the goal is reaching Mammon\'s check. APQuest\'s pattern is an event location "Mammon Defeated" holding a "Victory" event item, with Has("Victory") as the completion rule; either works.') ],
);

# ---------------------------------------------------------------- Regions
my @region_rows = (
    [ T('Regions') ],
    [ W('One row per region. Add new ones in the empty yellow rows; the Locations and Entrances dropdowns list whatever is in column A.') ],
    [ H('Region', 'What it stands for', 'Locations in it', 'Entrances in', 'Notes') ],
);
for my $r (@regions) {
    my $n = scalar(@region_rows) + 1;
    push @region_rows, [ I($r->[0]), IW($r->[1]),
        F("IF(A$n=\"\",\"\",COUNTIF(Locations!\$F\$4:\$F\$$LAST,A$n))"),
        F("IF(A$n=\"\",\"\",COUNTIF(Entrances!\$C\$4:\$C\$$LAST,A$n))"), IW('') ];
}
for (1 .. 40) {
    my $n = scalar(@region_rows) + 1;
    push @region_rows, [ I(''), IW(''),
        F("IF(A$n=\"\",\"\",COUNTIF(Locations!\$F\$4:\$F\$$LAST,A$n))"),
        F("IF(A$n=\"\",\"\",COUNTIF(Entrances!\$C\$4:\$C\$$LAST,A$n))"), IW('') ];
}
my $region_list = 'Regions!$A$4:$A$' . scalar(@region_rows);

# ---------------------------------------------------------------- Entrances
my @entrance_rows = (
    [ T('Entrances') ],
    [ W('One row per connection. The name is "From to To", which is what world.get_entrance() takes. Rule is a Rule Builder expression or a name from Named Rules; blank means always open.') ],
    [ H('Entrance', 'From', 'To', 'Rule', 'Notes') ],
);
for my $r (@regions) {
    for my $to (@{ $exits{ $r->[0] } }) {
        my $n = scalar(@entrance_rows) + 1;
        push @entrance_rows, [ F("IF(OR(B$n=\"\",C$n=\"\"),\"\",B$n&\" to \"&C$n)"), I($r->[0]), I($to),
                               C($gate{$to} // ''), IW('') ];
    }
}
for (1 .. 40) {
    my $n = scalar(@entrance_rows) + 1;
    push @entrance_rows, [ F("IF(OR(B$n=\"\",C$n=\"\"),\"\",B$n&\" to \"&C$n)"), I(''), I(''), C(''), IW('') ];
}

# ---------------------------------------------------------------- Locations
my @loc_rows = (
    [ T('Locations') ],
    [ W('Every check the apworld has. Region and Rule are today\'s logic, ready to change: pick a region from the dropdown, and give a Rule only where reaching the region is not enough. The names and ids are fixed - renaming a location breaks every seed made with it.') ],
    [ H('Location', 'ID', 'Group', 'Game area', 'Exists when', 'Region', 'Rule', 'Notes') ],
);
for my $l (@loc) {
    push @loc_rows, [ $l->[0], X(sprintf('0x%08X', $l->[1])), $l->[2], $l->[3], W($l->[4]), I($l->[5]), C($l->[6]), IW($l->[7]) ];
}
my $loc_end = scalar @loc_rows;

# ---------------------------------------------------------------- Named Rules
my $all_bosses = join(' & ', map { 'CanReachLocation(' . py_str(Quest64Checks::boss_name($_)) . ')' } grep { $_->{order} < 8 } @bosses);
my $all_monsters = join(' & ', map { "CanReachRegion(\"$_\")" } qw(Overworld Early Mid Late));
my @named = (
    [ 'EARTH_ORB_GATE',   'Has("Earth Orb") | OptionFilter(BossSouls, 1, operator="ge")',
      'With Boss Souls on, a boss may not be there to drop his gem, so the game opens the gem locks and the gem stops gating anything.' ],
    [ 'WIND_JADE_GATE',   'Has("Wind Jade") | OptionFilter(BossSouls, 1, operator="ge")', 'As EARTH_ORB_GATE.' ],
    [ 'WATER_JEWEL_GATE', 'Has("Water Jewel") | OptionFilter(BossSouls, 1, operator="ge")', 'As EARTH_ORB_GATE.' ],
    [ 'FIRE_RUBY_GATE',   'Has("Fire Ruby") | OptionFilter(BossSouls, 1, operator="ge")', 'As EARTH_ORB_GATE.' ],
    [ 'ALL_BOSSES_BEATEN', $all_bosses, "mammon_portal all_bosses: the seven bosses before Mammon. Each boss's own rule already asks for his Soul." ],
    [ 'ALL_MONSTER_AREAS', $all_monsters, 'mammon_portal all_monsters: every region a monster lives in.' ],
    [ 'ENDGAME_DOOR',
      '(OptionFilter(MammonPortal, 0) & Has("Eletale\'s Book")) | (OptionFilter(MammonPortal, 1) & ALL_BOSSES_BEATEN) | (OptionFilter(MammonPortal, 2) & ALL_MONSTER_AREAS) | (OptionFilter(MammonPortal, 3) & ALL_BOSSES_BEATEN & ALL_MONSTER_AREAS)',
      "The way into Mammon's World: the Book in the vanilla game, the portal condition otherwise (the Book is then not in the pool at all)." ],
);
my @named_rows = (
    [ T('Named Rules') ],
    [ W('Rules used in more than one place. Each becomes a constant in rules.py; write its name in any Rule cell. "Used" counts the Rule cells that mention it.') ],
    [ H('Name', 'Rule', 'Used', 'Notes') ],
);
for my $r (@named) {
    my $n = scalar(@named_rows) + 1;
    push @named_rows, [ I($r->[0]), C($r->[1]),
        F("IF(A$n=\"\",\"\",COUNTIF(Entrances!\$D\$4:\$D\$$LAST,\"*\"&A$n&\"*\")+COUNTIF(Locations!\$G\$4:\$G\$$LAST,\"*\"&A$n&\"*\")+COUNTIF(\$B\$4:\$B\$$LAST,\"*\"&A$n&\"*\"))"),
        IW($r->[2]) ];
}
for (1 .. 30) {
    my $n = scalar(@named_rows) + 1;
    push @named_rows, [ I(''), C(''),
        F("IF(A$n=\"\",\"\",COUNTIF(Entrances!\$D\$4:\$D\$$LAST,\"*\"&A$n&\"*\")+COUNTIF(Locations!\$G\$4:\$G\$$LAST,\"*\"&A$n&\"*\")+COUNTIF(\$B\$4:\$B\$$LAST,\"*\"&A$n&\"*\"))"),
        IW('') ];
}

# ---------------------------------------------------------------- Items
my @item_sheet = (
    [ T('Items') ],
    [ W('Every item the world can put in the pool. "In rules" counts the Rule cells (Entrances, Locations, Named Rules) that name it in quotes. A progression item with 0 is never required; a filler item above 0 would make seeds unwinnable. Count 0 means created only as filler.') ],
    [ H('Item', 'ID', 'Classification', 'Count', 'Exists when', 'In rules', 'Notes') ],
);
for my $r (@item_rows) {
    my $n = scalar(@item_sheet) + 1;
    # The name inside quotes, so "Wings" does not also count "Red Wings".
    my $q = "\"*\"\"\"&A$n&\"\"\"*\"";
    push @item_sheet, [ $r->[0], X(sprintf('0x%08X', $r->[1])), $r->[2], $r->[3], W($r->[4]),
        F("COUNTIF(Entrances!\$D\$4:\$D\$$LAST,$q)+COUNTIF(Locations!\$G\$4:\$G\$$LAST,$q)+COUNTIF('Named Rules'!\$B\$4:\$B\$$LAST,$q)"),
        W($r->[5]) ];
}

# ---------------------------------------------------------------- Options
my @options = (
    [ 'mammon_portal', 'MammonPortal', 'vanilla 0, all_bosses 1, all_monsters 2, both 3', "What opens Mammon's World: ENDGAME_DOOR. Also makes the Book's Shannon a check and takes the Book out of the pool." ],
    [ 'boss_souls', 'BossSouls', 'off 0, bosses 1, with_mammon 2', "Each boss's check needs his Soul; the gem gates open (the *_GATE rules)." ],
    [ 'shuffle_orbs', 'ShuffleOrbs', 'off / on', 'Off: the six gate items stay at their vanilla checks (locked in pre_fill). No rule changes.' ],
    [ 'chestsanity', 'Chestsanity', 'off / on', 'Whether chests are locations.' ],
    [ 'giftsanity', 'Giftsanity', 'off / on', 'Whether gift NPCs are locations.' ],
    [ 'enemysanity', 'Enemysanity', 'off / on', 'Whether each monster kind is a location.' ],
    [ 'spiritsanity', 'Spiritsanity', 'off / on', 'Whether spirits are locations; also makes one Level Up per spirit.' ],
    [ 'extra_level_ups', 'ExtraLevelUps', '0 - 99', 'Extra Level Ups in the pool.' ],
    [ 'wingsmith_wings', 'WingsmithWings', 'off / on', 'A wingsmith also hands over his wings. No rule changes.' ],
    [ 'shuffle_boss_order', 'ShuffleBossOrder', 'off / on', 'The game\'s own setting. Not in the logic today, although it moves which boss stands where - worth a look if bosses get their own regions.' ],
    [ 'goal', 'Goal', 'mammon 0', 'The only goal.' ],
);
my @option_rows = (
    [ T('Options') ],
    [ W('The yaml options, with the class name OptionFilter takes. The game\'s own settings (spells, palettes, enhancements) are left out: none of them touch the logic.') ],
    [ H('Option', 'Class', 'Values', 'Effect on logic', 'Notes') ],
);
push @option_rows, [ $_->[0], $_->[1], W($_->[2]), W($_->[3]), IW('') ] for @options;

# ---------------------------------------------------------------- write
my @sheets = (
    [ 'Guide',       \@guide,         [ 42, 70, 60 ], 0 ],
    [ 'Regions',     \@region_rows,   [ 18, 70, 14, 14, 40 ], 3, filter => "A3:E" . scalar(@region_rows) ],
    [ 'Entrances',   \@entrance_rows, [ 26, 16, 16, 70, 40 ], 3, filter => "A3:E" . scalar(@entrance_rows),
      lists => [ [ 'B4:C' . scalar(@entrance_rows), $region_list ] ] ],
    [ 'Locations',   \@loc_rows,      [ 44, 12, 10, 24, 22, 14, 60, 40 ], 3, filter => "A3:H$loc_end",
      lists => [ [ "F4:F$loc_end", $region_list ] ] ],
    [ 'Named Rules', \@named_rows,    [ 22, 90, 8, 50 ], 3 ],
    [ 'Items',       \@item_sheet,    [ 26, 12, 14, 8, 30, 9, 44 ], 3, filter => "A3:G" . scalar(@item_sheet) ],
    [ 'Options',     \@option_rows,   [ 20, 18, 40, 70, 30 ], 3 ],
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
my $defined = join('', map {
    my ($i, $s) = ($_, $sheets[$_]);
    my %o = @$s[ 4 .. $#$s ];
    $o{filter} ? do {
        my ($a, $b) = split /:/, $o{filter};
        (my $ca = $a) =~ s/(\d+)/\$$1/; (my $cb = $b) =~ s/(\d+)/\$$1/;
        $ca =~ s/^([A-Z]+)/\$$1/; $cb =~ s/^([A-Z]+)/\$$1/;
        my $q = $s->[0] =~ / / ? "'$s->[0]'" : $s->[0];
        "<definedName name=\"_xlnm._FilterDatabase\" localSheetId=\"$i\" hidden=\"1\">$q!$ca:$cb</definedName>";
    } : '';
} 0 .. $#sheets);
$files{'xl/workbook.xml'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">' .
    '<sheets>' . join('', map { sprintf('<sheet name="%s" sheetId="%d" r:id="rId%d"/>', $sheets[$_][0], $_ + 1, $_ + 1) } 0 .. $#sheets) . '</sheets>' .
    ($defined ? "<definedNames>$defined</definedNames>" : '') .
    '<calcPr calcId="191029" fullCalcOnLoad="1"/></workbook>';
$files{'xl/_rels/workbook.xml.rels'} = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' .
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' .
    join('', map { sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet%d.xml"/>', $_ + 1, $_ + 1) } 0 .. $#sheets) .
    sprintf('<Relationship Id="rId%d" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>', @sheets + 1) .
    '</Relationships>';
$files{'xl/styles.xml'} = $styles;
for my $i (0 .. $#sheets) {
    my ($name, $rows, $widths, $freeze, %o) = @{ $sheets[$i] };
    $files{"xl/worksheets/sheet" . ($i + 1) . ".xml"} = sheet_xml($rows, $widths, $freeze, %o);
}

my @order = ('[Content_Types].xml', '_rels/.rels', 'xl/workbook.xml', 'xl/_rels/workbook.xml.rels', 'xl/styles.xml',
             map { "xl/worksheets/sheet$_.xml" } 1 .. @sheets);
my $z = IO::Compress::Zip->new($out, Name => $order[0]) or die "zip: $ZipError";
for my $i (0 .. $#order) {
    $z->newStream(Name => $order[$i]) if $i > 0;
    $z->print($files{ $order[$i] });
}
$z->close;
printf "wrote %s (%d regions, %d entrances, %d locations, %d items)\n", $out, scalar @regions,
    scalar(grep { $_->[1] && ref $_->[1] && $_->[1]{v} } @entrance_rows[ 3 .. $#entrance_rows ]), scalar @loc, scalar @item_rows;
