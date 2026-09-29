#!/usr/bin/perl
# Writes tools/archipelago/quest64.apworld and the player template
# tools/archipelago/Quest64Recompiled.yaml, for Archipelago 0.6.7.
#
# The locations and the vanilla item pool come from Quest64Checks.pm, the
# same module DOCS/archipelago_checks.xlsx is built from, so a location's
# name is decided once and the sheet and the world always agree. An AP
# location name is a save-compatibility contract - renaming one breaks every
# seed that used it - so it is worth the indirection.
#
# Only Locations.py and Items.py are generated. The rest of the world
# (__init__.py, Options.py, Regions.py, Rules.py) is written out below as
# plain text because it is logic, not data.
#
#   perl tools/archipelago_world.pl
#   copy tools/archipelago/quest64.apworld to Archipelago/custom_worlds/
#
# No Python on this machine, so nothing here is run; ArchipelagoGenerate.exe
# is what proves it.
use strict;
use warnings;
use File::Basename qw(dirname);
use File::Path qw(make_path);
use IO::Compress::Zip qw(zip $ZipError);

use lib dirname(__FILE__);
use Quest64Checks;
use Quest64Logic;

my $root = dirname(__FILE__) . '/..';
my $out  = "$root/tools/archipelago";
my $game = 'Quest 64 Recompiled';

my $checks = Quest64Checks::gather($root);

# Archipelago ids have to be unique within a world and stable forever. 0x5164
# is "Q64"; the next nibble separates the groups so a group can grow without
# disturbing another.
my $BASE = 0x51640000;
my %GROUP = (chest => 0x1000, giver => 0x2000, enemy => 0x3000, boss => 0x4000, spirit => 0x5000, item => 0x0000);

sub py_str { my $s = shift; $s =~ s/\\/\\\\/g; $s =~ s/"/\\"/g; return "\"$s\"" }

# ---------------------------------------------------------------- locations
# The names, ids and groups are fixed here; which region each location is in,
# and any rule of its own, come from the logic workbook (see "logic" below).
my @loc;
for my $c (@{ $checks->{chests} }) {
    push @loc, { name => Quest64Checks::chest_name($c), id => $BASE + $GROUP{chest} + $c->{idx}, group => 'chest' };
}
for my $g (@{ $checks->{givers} }) {
    # With a mammon_portal condition the Book opens nothing, so the Shannon
    # who hands it over is a check of her own even without giftsanity.
    my $create = $g->{item} eq "Eletale's Book" ? 'book_giver' : 'giver';
    push @loc, { name => Quest64Checks::giver_name($g), id => $BASE + $GROUP{giver} + $g->{idx},
                 group => 'giver', create => $create };
}
for my $m (@{ $checks->{monsters} }) {
    push @loc, { name => Quest64Checks::enemy_name($m), id => $BASE + $GROUP{enemy} + $m->{id}, group => 'enemy' };
}
for my $b (@{ $checks->{bosses} }) {
    push @loc, { name => Quest64Checks::boss_name($b), id => $BASE + $GROUP{boss} + $b->{order}, group => 'boss' };
}
for my $s (@{ $checks->{spirits} }) {
    push @loc, { name => Quest64Checks::spirit_name($s), id => $BASE + $GROUP{spirit} + $s->{id}, group => 'spirit' };
}

my %by_group;
$by_group{ $_->{group} }++ for @loc;

# ---------------------------------------------------------------- items
# The vanilla pool, plus one Level Up per spirit. A spirit check hands out
# whatever Archipelago put there; Level Up is what a spirit used to give, so
# there are as many of them as there are spirits.
my @items = @{ $checks->{items} };
my @item_rows;

# The six progression items: the four gems, the book and the key. Everything
# else is filler unless it is one of these.
my %progression = map { $_ => 1 } ("Earth Orb", "Wind Jade", "Water Jewel", "Fire Ruby", "Eletale's Book", "Dark Gaol Key");
# Wings warp you about; useful, not required.
my %useful = map { $_ => 1 } ('White Wings', 'Yellow Wings', 'Blue Wings', 'Green Wings', 'Red Wings', 'Black Wings');

# These names are the ROM's. A typo in one quietly demotes a key item to
# filler and generates unwinnable seeds, which is what "Eletale Book" did
# before it was spelled "Eletale's Book". Check them.
{
    my %have = map { $_ => 1 } @items;
    my @missing = grep { !$have{$_} } (sort keys %progression), (sort keys %useful);
    die "item names not in the ROM's list: @missing\n" if @missing;
}

# Where the six gate items are in the unshuffled game, for the yaml option
# that leaves them there. Read from the tables rather than typed in, and
# checked: each one has exactly one home.
my %vanilla_of;
for my $b (@{ $checks->{bosses} }) {
    $vanilla_of{ $b->{reward} } = Quest64Checks::boss_name($b) if $progression{ $b->{reward} };
}
for my $g (@{ $checks->{givers} }) {
    $vanilla_of{ $g->{item} } = Quest64Checks::giver_name($g) if $progression{ $g->{item} };
}
{
    my @lost = grep { !$vanilla_of{$_} } sort keys %progression;
    die "no vanilla location for: @lost\n" if @lost;
}

my $spirit_count = scalar @{ $checks->{spirits} };
push @item_rows, { name => 'Level Up', id => $BASE + $GROUP{item} + 0xF00, type => 'progression', count => $spirit_count,
                   create => 'lambda options: bool(options.spiritsanity) or options.extra_level_ups.value > 0',
                   note => 'What a spirit used to give: the element-choice screen' };

# One Soul per boss, for the boss_souls option. These are Archipelago's
# alone - the game has no such item and never sees one - so they sit in the
# item group at 0xE00 + the boss's number, the same number the boss's own
# check uses. Mammon's is only made when the yaml asks for it.
for my $b (@{ $checks->{bosses} }) {
    my $name = "$b->{name}'s Soul";
    my $needs = $b->{order} >= 8 ? 2 : 1;   # Mammon only with with_mammon
    push @item_rows, { name => $name, id => $BASE + $GROUP{item} + 0xE00 + $b->{order},
                       type => 'progression', count => 1,
                       create => "lambda options: options.boss_souls.value >= $needs",
                       note => "Until this arrives, $b->{name} is not in the game" };
}

my $next = 1;
for my $i (0 .. $#items) {
    my $n = $items[$i];
    next if $n eq 'Nothing' || $n eq '';
    my $type = $progression{$n} ? 'progression' : $useful{$n} ? 'useful' : 'filler';
    push @item_rows, { name => $n, id => $BASE + $GROUP{item} + $i, type => $type, count => 0, vanilla_id => $i };
}

# Only the items that have to exist are counted here: the six that open the
# way on, two of each pair of wings, and one Level Up per spirit. Everything
# else is filler, created at generation time to pad the pool out to however
# many locations the sanity options actually switched on - which is why the
# counts here do not have to add up to the location count.
my $locations = scalar @loc;
for my $r (@item_rows) {
    next if $r->{create};   # Level Up and the Souls set their own
    $r->{count} = $progression{ $r->{name} } ? 1 : $useful{ $r->{name} } ? 2 : 0;
}
# With a mammon_portal condition the condition opens the door in the Book's
# place, so the Book would be a key to nothing: it is left out of the pool.
for my $r (grep { $_->{name} eq "Eletale's Book" } @item_rows) {
    $r->{create} = 'lambda options: options.mammon_portal.value == 0';
}

# ---------------------------------------------------------------- logic
# Regions, entrances, each location's region and every rule come from the
# logic workbook, which Quest64Logic.pm reads and checks against the names
# this world really has: the items above, the locations above, and the
# option classes in Options.py below.
my $logic_path = "$root/DOCS/archipelago_logic.xlsx";
my %option_class;
{
    open my $self, '<', __FILE__ or die __FILE__ . ": $!";
    local $/;
    my $src = <$self>;
    $option_class{$1} = 1 while $src =~ /^class (\w+)\((?:Toggle|DefaultOnToggle|Choice|Range)\):/mg;
}
my $logic = Quest64Logic::read_logic($logic_path, {
    items     => { map { $_->{name} => $_->{type} } @item_rows },
    locations => { map { $_->{name} => 1 } @loc },
    options   => \%option_class,
});
$_->{region} = $logic->{locations}{ $_->{name} }{region} for @loc;

# ---------------------------------------------------------------- Python
my $stamp = "# Generated by tools/archipelago_world.pl. Do not edit by hand.\n"
          . "# The names and ids come from Quest64Checks.pm, which DOCS/archipelago_checks.xlsx\n"
          . "# is also built from, so this file and that sheet cannot disagree.\n";

my $locations_py = $stamp . <<'PY';
from typing import Callable, Dict, NamedTuple, Optional

from BaseClasses import Location


class Q64Location(Location):
    game = "Quest 64 Recompiled"


class Q64LocationData(NamedTuple):
    region: str
    address: Optional[int] = None
    group: str = ""
    can_create: Callable = lambda options: True


def _on(group: str) -> Callable:
    return {
        "chest": lambda options: bool(options.chestsanity),
        "giver": lambda options: bool(options.giftsanity),
        "book_giver": lambda options: bool(options.giftsanity) or options.mammon_portal.value != 0,
        "enemy": lambda options: bool(options.enemysanity),
        "boss": lambda options: True,
        "spirit": lambda options: bool(options.spiritsanity),
    }[group]


location_data_table: Dict[str, Q64LocationData] = {
PY
for my $l (@loc) {
    $locations_py .= sprintf("    %s: Q64LocationData(region=%s, address=0x%08X, group=%s, can_create=_on(%s)),\n",
        py_str($l->{name}), py_str($l->{region}), $l->{id}, py_str($l->{group}), py_str($l->{create} // $l->{group}));
}
$locations_py .= <<"PY";
}

location_table: Dict[str, int] = {
    name: data.address for name, data in location_data_table.items() if data.address is not None
}
code_to_location_table: Dict[int, str] = {code: name for name, code in location_table.items()}
locked_locations: Dict[str, Q64LocationData] = {}

# Where the gate items sit when the orbs are not shuffled.
vanilla_locations: Dict[str, str] = {
@{[ join qq{
}, map { sprintf('    %s: %s,', py_str($_), py_str($vanilla_of{$_})) } sort keys %vanilla_of ]}
}
PY

my $items_py = $stamp . <<'PY';
from typing import Callable, Dict, NamedTuple, Optional

from BaseClasses import Item, ItemClassification


class Q64Item(Item):
    game = "Quest 64 Recompiled"


class Q64ItemData(NamedTuple):
    code: Optional[int] = None
    type: ItemClassification = ItemClassification.filler
    num_exist: int = 1
    can_create: Callable = lambda options: True


item_data_table: Dict[str, Q64ItemData] = {
PY
my %cls = (progression => 'ItemClassification.progression', useful => 'ItemClassification.useful', filler => 'ItemClassification.filler');
for my $r (@item_rows) {
    my $create = $r->{create} // 'lambda options: True';
    $items_py .= sprintf("    %s: Q64ItemData(code=0x%08X, type=%s, num_exist=%d, can_create=%s),\n",
        py_str($r->{name}), $r->{id}, $cls{ $r->{type} }, $r->{count}, $create);
}
$items_py .= <<'PY';
}

item_table: Dict[str, int] = {name: data.code for name, data in item_data_table.items() if data.code is not None}
code_to_item_table: Dict[int, str] = {code: name for name, code in item_table.items()}
filler_items = [name for name, data in item_data_table.items() if data.type == ItemClassification.filler]
PY

# ---------------------------------------------------------------- seed settings
# The game's own settings, chosen in the yaml instead of the menus. They go to
# the game in slot_data under "settings", keyed by these names; when the game
# is connected as it starts, it plays with these and hides the Randomizer and
# Enhancements menus. Every one is off unless the yaml says otherwise.
#   [ option name, class, section, display name, description, kind ]
# kind "toggle" is Off/On (0/1); "random" is Off/Randomized (0/1). The choice
# is called "randomized" because "random" is a reserved option value in
# Archipelago.
my @seed_settings = (
    [ 'shuffle_spells', 'ShuffleSpells', 'Randomizer', 'Shuffled Spells',
      'Shuffles which spell each element teaches, with the spell damage rebalance, Invalidity, and the fixes that keep shuffled spell combinations from crashing.', 'toggle' ],
    [ 'early_healing', 'EarlyHealing', 'Randomizer', 'Early Healing',
      'A healing spell is guaranteed early.', 'toggle' ],
    [ 'enemy_randomizer', 'EnemyRandomizer', 'Randomizer', 'Enemy Randomizer',
      'Any monster set can land in any area, and every monster is scaled to the area it appears in.', 'toggle' ],
    [ 'shuffle_boss_order', 'ShuffleBossOrder', 'Randomizer', 'Shuffle Boss Order',
      'The seven bosses before Mammon swap arenas.', 'toggle' ],
    [ 'random_guilty_element', 'RandomGuiltyElement', 'Randomizer', 'Randomize Guilty Element',
      'Guilty takes a random element instead of his own.', 'toggle' ],
    [ 'faster_areas', 'FasterAreas', 'Randomizer', 'Faster Areas',
      "Fast Monastery, Fast Blue Cave, Fast Shamwood and Fast Mammon's World together: the long empty stretches are skipped.", 'toggle' ],
    [ 'wings_never_expire', 'WingsNeverExpire', 'Randomizer', 'Wings Never Expire',
      'Using a pair of wings does not use them up.', 'toggle' ],
    [ 'no_enemy_drop_limit', 'NoEnemyDropLimit', 'Randomizer', 'No Enemy Drop Limit',
      'Monsters keep dropping items past the vanilla limit.', 'toggle' ],
    [ 'element_cap_99', 'ElementCap99', 'Randomizer', 'Element Cap 99',
      'Each element can be raised to 99 instead of 50, by spirits, level-ups and Level Up items alike.', 'toggle' ],
    [ 'jp_healing', 'JPHealing', 'Enhancements', 'JP Healing Amounts',
      'Healing Lv2 restores 16 HP instead of 8, as in the Japanese release.', 'toggle' ],
    [ 'jp_magic_barrier', 'JPMagicBarrier', 'Enhancements', 'JP Magic Barrier',
      'Magic Barrier holds two turns longer, as in the Japanese release.', 'toggle' ],
    [ 'jp_stat_up_effect', 'JPStatUpEffect', 'Enhancements', 'JP Stat Up Effect',
      'A colour burst over Brian when a stat rises, as in the Japanese release.', 'toggle' ],
    [ 'exit_from_anywhere', 'ExitFromAnywhere', 'Enhancements', 'Exit from Anywhere',
      'The bound Exit Spell control warps out of an area without the spell or the MP.', 'toggle' ],
    [ 'fast_walking', 'FastWalking', 'Enhancements', 'Fast Walking',
      'Brian walks 50% faster.', 'toggle' ],
    [ 'text_improvements', 'TextImprovements', 'Enhancements', 'Text Improvements',
      "Merrow's clearer wording for a number of the game's messages.", 'toggle' ],
    [ 'text_palette', 'TextPalette', 'Cosmetics', 'Text Palette',
      'The colour of the text boxes.', 'random' ],
    [ 'staff_palette', 'StaffPalette', 'Cosmetics', 'Staff Palette',
      "The colour of Brian's staff.", 'random' ],
    [ 'cloak_colour', 'CloakColour', 'Cosmetics', 'Cloak Colour',
      "The colour of Brian's cloak.", 'random' ],
    [ 'brian_clothes', 'BrianClothes', 'Cosmetics', "Brian's Clothes",
      "The colours of Brian's clothes.", 'random' ],
    [ 'spell_palettes', 'SpellPalettes', 'Cosmetics', 'Spell Palettes',
      'The colours of the spell effects.', 'random' ],
);

sub wrap_words {
    my ($text, $width, $first, $rest) = @_;
    my @out; my $line = $first;
    for my $w (split / /, $text) {
        if (length($line) + length($w) + 1 > $width && $line ne $first && $line ne $rest) { push @out, $line; $line = "$rest$w" }
        else { $line .= ($line eq $first || $line eq $rest ? '' : ' ') . $w }
    }
    push @out, $line;
    return @out;
}

my $seed_classes = '';
for my $o (@seed_settings) {
    my ($name, $class, $section, $display, $doc, $kind) = @$o;
    my $body = join("\n", wrap_words("$section. $doc", 76, '    """', '    '));
    if ($kind eq 'toggle') {
        $seed_classes .= "class $class(Toggle):\n$body\"\"\"\n    display_name = \"$display\"\n\n\n";
    }
    else {
        $seed_classes .= "class $class(Choice):\n$body\"\"\"\n    display_name = \"$display\"\n    option_off = 0\n    option_randomized = 1\n    default = 0\n\n\n";
    }
}
my $seed_fields = join('', map { "    $_->[0]: $_->[1]\n" } @seed_settings);
my $seed_slot = join('', map { "                \"$_->[0]\": self.options.$_->[0].value,\n" } @seed_settings);
my $seed_yaml = '';
{
    my $section = '';
    for my $o (@seed_settings) {
        my ($name, $class, $sec, $display, $doc, $kind) = @$o;
        if ($sec ne $section) {
            $section = $sec;
            $seed_yaml .= "\n  # ==== $sec\n";
            $seed_yaml .= "  # These three groups are the game's own settings. When the game is\n"
                        . "  # connected to the room as it starts, it plays with these instead of\n"
                        . "  # its menus, and hides the Randomizer and Enhancements menus.\n"
                if $sec eq 'Randomizer';
        }
        $seed_yaml .= "\n" . join("\n", wrap_words("$display: $doc", 78, '  # ', '  # ')) . "\n";
        $seed_yaml .= $kind eq 'toggle'
            ? "  $name:\n    'false': 1\n    'true': 0\n"
            : "  $name:\n    off: 1\n    randomized: 0\n";
    }
}

my $options_py = <<'PY';
from dataclasses import dataclass

from Options import Choice, DefaultOnToggle, PerGameCommonOptions, Range, Toggle


class Chestsanity(DefaultOnToggle):
    """Treasure chests are Archipelago checks."""
    display_name = "Chestsanity"


class Giftsanity(DefaultOnToggle):
    """NPCs who hand over an item are Archipelago checks: the gift NPCs, the
    two endgame Shannons and the wingsmiths. Each gives its check once, the
    first time you talk to them, whatever is in your bag - and no item."""
    display_name = "Giftsanity"


class WingsmithWings(Toggle):
    """The six wingsmiths still hand over their wings, on top of their
    Archipelago check. Only matters with giftsanity on; each gives its wings
    once, the first time you talk to them."""
    display_name = "Wingsmiths Give Wings"


class Enemysanity(Toggle):
    """Defeating a kind of regular monster is an Archipelago check. One check
    per kind, sent the first time you beat one of them and never again -
    there are 67 kinds, so 67 checks, and a lot of hunting."""
    display_name = "Enemysanity"


class ShuffleOrbs(DefaultOnToggle):
    """Shuffle the items that open the way on: the Earth Orb, Wind Jade,
    Water Jewel and Fire Ruby, the Eletale's Book and the Dark Gaol Key.

    Turn this off to leave all six where the game puts them - the four gems
    on their bosses, the book and the key with the two Shannons - so the run
    follows the usual route and only everything else moves."""
    display_name = "Shuffle Orbs"


class Spiritsanity(DefaultOnToggle):
    """Spirits are Archipelago checks. With this on a spirit no longer raises
    an element itself - it sends a check, and the element-choice screen comes
    back as the "Level Up" item, from wherever Archipelago put it."""
    display_name = "Spiritsanity"


class ExtraLevelUps(Range):
    """Extra "Level Up" items in the pool, on top of the one per spirit.

    These have no spirit behind them - nothing is added to the game world.
    They simply arrive like any other item and bring up the element-choice
    screen, so a run can hand out more element levels than Quest 64 has
    spirits for. They take the place of filler, so the pool does not grow."""
    display_name = "Extra Level Ups"
    range_start = 0
    range_end = 99
    default = 0


class MammonPortal(Choice):
    """What it takes to open the way into Mammon's World.

    The last door of the final staircase, in Baragoon Moor, wants the
    Eletale's Book in the vanilla game. Any other choice replaces the Book:
    the door opens once the condition is met, the Book is left out of the
    pool, and the Shannon who would have handed it over is a gift check of
    her own (even with giftsanity off).

    vanilla       the Book, as the game has it

    all_bosses    every boss before Mammon - Solvaring, Zelse, Nepty, Shilf,
                  Fargo, Guilty and Beigis - has to be beaten. They are
                  checks whatever else is switched on, so this costs nothing
                  extra to track.
    all_monsters  one of each of the 67 kinds of regular monster has to be
                  beaten. This needs enemysanity, because the count is kept
                  by the server: a monster leaves nothing behind in the save
                  to read back after a reload. Turning this on turns
                  enemysanity on with it.
    both          all of the above."""
    display_name = "Mammon's World Portal"
    option_vanilla = 0
    option_all_bosses = 1
    option_all_monsters = 2
    option_both = 3
    default = 0


class BossSouls(Choice):
    """Bosses are not in the game until their Soul turns up.

    A boss whose Soul has not arrived is not loaded and not placed: his
    arena is empty and you walk straight through it. It is the same state
    the game itself uses for a boss you have already beaten, so nothing is
    left half-there to bump into. The Soul is an Archipelago item, not
    something the game knows about, so it can be anywhere in the multiworld.

    Whether a boss is there is settled as his arena loads, so a Soul that
    arrives while you are standing in the arena takes effect when you next
    walk in.

    With Souls on, the doors, boats and teleporters the Earth Orb, Wind
    Jade, Water Jewel and Fire Ruby open are open from the start, since the
    boss who drops one may not be there yet. The Dark Gaol Key still locks
    what it locks, and so does the Eletale's Book unless Mammon's World
    Portal replaces it.

    bosses        the seven before Mammon get Souls
    with_mammon   Mammon as well, so the last fight waits on his Soul too"""
    display_name = "Boss Souls"
    option_off = 0
    option_bosses = 1
    option_with_mammon = 2
    default = 0


SEED_CLASSES_HERE
class Goal(Choice):
    """What finishes the run."""
    display_name = "Goal"
    option_mammon = 0
    default = 0


@dataclass
class Q64Options(PerGameCommonOptions):
    goal: Goal
    mammon_portal: MammonPortal
    boss_souls: BossSouls
    shuffle_orbs: ShuffleOrbs
    chestsanity: Chestsanity
    giftsanity: Giftsanity
    wingsmith_wings: WingsmithWings
    enemysanity: Enemysanity
    spiritsanity: Spiritsanity
    extra_level_ups: ExtraLevelUps
SEED_FIELDS_HERE
PY
$options_py =~ s/SEED_CLASSES_HERE\n/$seed_classes/;
$options_py =~ s/SEED_FIELDS_HERE\n/$seed_fields/;

# Regions.py and Rules.py are written from the logic workbook, in Rule
# Builder form (worlds/apquest/rules.py is the pattern). To change the logic,
# change DOCS/archipelago_logic.xlsx and run this again.
my $logic_stamp = "# Generated by tools/archipelago_world.pl from DOCS/archipelago_logic.xlsx.\n"
                . "# Change the logic in that workbook, not here.\n";

my $regions_py = $logic_stamp . <<'PY'
from typing import List, Tuple


# Every region, Menu first: Archipelago starts there.
regions: List[str] = [
PY
    . join('', map { '    ' . py_str($_) . ",\n" } @{ $logic->{regions} })
    . <<'PY'
]

# (from, to). The entrance between them is named "<from> to <to>", which is
# the name world.get_entrance() and the rules use.
connections: List[Tuple[str, str]] = [
PY
    . join('', map { '    (' . py_str($_->{from}) . ', ' . py_str($_->{to}) . "),\n" } @{ $logic->{entrances} })
    . "]\n";

my $rules_py;
{
    my %calls = %{ $logic->{uses}{calls} // {} };
    my %rb = map { $_ => 1 } grep { $_ ne 'OptionFilter' } keys %calls;
    $rb{Rule} = 1;
    my @opts = sort keys %{ $logic->{uses}{options} // {} };
    my @entrance_rules = grep { $_->{rule} ne '' } @{ $logic->{entrances} };
    my @location_rules = map { [ $_->{name}, $logic->{locations}{ $_->{name} }{rule} ] }
                         grep { $logic->{locations}{ $_->{name} }{rule} ne '' } @loc;

    $rules_py = $logic_stamp . "from __future__ import annotations\n\nfrom typing import TYPE_CHECKING, Dict\n\n";
    $rules_py .= "from rule_builder.options import OptionFilter\n" if $calls{OptionFilter};
    $rules_py .= 'from rule_builder.rules import ' . join(', ', sort keys %rb) . "\n";
    $rules_py .= "\nfrom .Options import " . join(', ', @opts) . "\n" if @opts;
    $rules_py .= "\nif TYPE_CHECKING:\n    from . import Q64World\n";
    if (@{ $logic->{named} }) {
        $rules_py .= "\n# Rules used in more than one place (the Named Rules sheet).\n";
        $rules_py .= "$_->[0]: Rule = $_->[1]\n" for @{ $logic->{named} };
    }
    $rules_py .= "\n# The Entrances sheet. An entrance with no rule is always open.\n"
               . "entrance_rules: Dict[str, Rule] = {\n"
               . join('', map { '    ' . py_str($_->{name}) . ": $_->{rule},\n" } @entrance_rules) . "}\n";
    $rules_py .= "\n# The Locations sheet, where reaching the region is not enough.\n"
               . "location_rules: Dict[str, Rule] = {\n"
               . join('', map { '    ' . py_str($_->[0]) . ": $_->[1],\n" } @location_rules) . "}\n";
    $rules_py .= "\n# The Guide sheet's completion rule.\ncompletion_rule: Rule = $logic->{completion}\n";
    $rules_py .= <<'PY';


def set_all_rules(world: Q64World) -> None:
    multiworld, player = world.multiworld, world.player
    for name, rule in entrance_rules.items():
        world.set_rule(multiworld.get_entrance(name, player), rule)
    # A location the options switched off does not exist, and has no rule
    # to set.
    present = {location.name for location in multiworld.get_locations(player)}
    for name, rule in location_rules.items():
        if name in present:
            world.set_rule(multiworld.get_location(name, player), rule)
    world.set_completion_rule(completion_rule)
PY
}

my $init_py = <<'PY';
from typing import Dict, List

from BaseClasses import ItemClassification, Region, Tutorial
from Options import OptionError
from worlds.AutoWorld import WebWorld, World

from .Items import Q64Item, item_data_table, item_table, code_to_item_table, filler_items
from .Locations import (Q64Location, location_data_table, location_table,
                        code_to_location_table, vanilla_locations)
from .Options import Q64Options
from .Regions import regions, connections
from .Rules import set_all_rules


class Q64WebWorld(WebWorld):
    theme = "grass"

    setup_en = Tutorial(
        tutorial_name="Start Guide",
        description="A guide to playing Quest 64 Recompiled in Archipelago.",
        language="English",
        file_name="guide_en.md",
        link="guide/en",
        authors=["Fuzzyness"],
    )

    tutorials = [setup_en]


class Q64World(World):
    """Quest 64, as the recompiled port plays it."""

    game = "Quest 64 Recompiled"
    web = Q64WebWorld()
    options_dataclass = Q64Options
    options: Q64Options
    location_name_to_id = location_table
    item_name_to_id = item_table

    def generate_early(self) -> None:
        # Asking for every kind of monster to be beaten means counting them,
        # and the count is the server's: a monster leaves nothing behind in
        # the save for the game to read back after a reload, the way a chest
        # or a spirit does. So the monster checks have to exist.
        if self.options.mammon_portal.value & 2:
            self.options.enemysanity.value = 1

    def create_item(self, name: str) -> Q64Item:
        data = item_data_table[name]
        return Q64Item(name, data.type, data.code, self.player)

    def get_filler_item_name(self) -> str:
        return self.random.choice(filler_items)

    def create_regions(self) -> None:
        for region_name in regions:
            self.multiworld.regions.append(Region(region_name, self.player, self.multiworld))

        # Named "<from> to <to>", the name Rules.py sets each rule by.
        for source, target in connections:
            self.multiworld.get_region(source, self.player).connect(
                self.multiworld.get_region(target, self.player), f"{source} to {target}")

        for location_name, location_data in location_data_table.items():
            if not location_data.can_create(self.options):
                continue
            region = self.multiworld.get_region(location_data.region, self.player)
            region.add_locations({location_name: location_data.address}, Q64Location)

    def locked_items(self) -> Dict[str, str]:
        """Item name -> location name for anything held out of the pool.

        With shuffle_orbs off the six gate items stay where the game puts
        them. If the group that location belongs to is switched off it is not
        a location at all, so the item goes back in the pool instead.
        """
        if self.options.shuffle_orbs:
            return {}
        return {
            item: location
            for item, location in vanilla_locations.items()
            if location_data_table[location].can_create(self.options)
            and item_data_table[item].can_create(self.options)
        }

    def pre_fill(self) -> None:
        for item_name, location_name in self.locked_items().items():
            location = self.multiworld.get_location(location_name, self.player)
            location.place_locked_item(self.create_item(item_name))

    def create_items(self) -> None:
        # The pool has to be exactly as big as the number of locations in
        # play, and which locations those are depends on the sanity options.
        # So: place what must exist, then pad with filler.
        locked = self.locked_items()
        open_locations = sum(
            1 for data in location_data_table.values() if data.can_create(self.options)
        ) - len(locked)

        pool: List[Q64Item] = []
        level_ups = 0
        for name, data in item_data_table.items():
            if not data.code or not data.can_create(self.options):
                continue
            if name in locked:
                continue
            if name == "Level Up":
                level_ups = data.num_exist
                continue
            for _ in range(data.num_exist):
                pool.append(self.create_item(name))

        # One Level Up per spirit when spirits are checks, plus however many
        # extra the yaml asked for. The extras have nothing behind them in
        # the game world - they just fire the element-choice screen when they
        # arrive - so they are capped only by the room left in the pool.
        wanted = (level_ups if self.options.spiritsanity else 0)
        wanted += self.options.extra_level_ups.value
        for _ in range(min(wanted, max(0, open_locations - len(pool)))):
            pool.append(self.create_item("Level Up"))

        # Too many items for the locations in play: shed the ones no rule
        # waits on - filler, then useful (the wings), then Level Ups, which
        # are progression in name only. The gate items and the Souls are
        # what the seed is beaten with, so they are never dropped; if they
        # alone do not fit, the options cannot make a winnable seed.
        def expendable(item: Q64Item) -> int:
            if item.classification == ItemClassification.filler:
                return 0
            if item.classification == ItemClassification.useful:
                return 1
            if item.name == "Level Up":
                return 2
            return 3

        excess = len(pool) - open_locations
        if excess > 0:
            order = sorted(range(len(pool)), key=lambda i: expendable(pool[i]))
            drop = set(i for i in order[:excess] if expendable(pool[i]) < 3)
            if len(drop) < excess:
                needed = sum(1 for item in pool if expendable(item) == 3)
                raise OptionError(
                    f"Quest 64: player {self.player_name} needs {needed} locations for the "
                    f"items the seed is won with, but the options leave only {open_locations}. "
                    f"Switch on more of the *sanity options.")
            pool = [item for i, item in enumerate(pool) if i not in drop]
        while len(pool) < open_locations:
            pool.append(self.create_item(self.get_filler_item_name()))

        self.multiworld.itempool += pool

    def set_rules(self) -> None:
        set_all_rules(self)

    def fill_slot_data(self) -> Dict[str, object]:
        # What the game needs once it connects: which groups are checks, and
        # the id bases so it can turn a location id back into a check.
        return {
            "goal": self.options.goal.value,
            "mammon_portal": self.options.mammon_portal.value,
            "boss_souls": self.options.boss_souls.value,
            "chestsanity": bool(self.options.chestsanity),
            "giftsanity": bool(self.options.giftsanity),
            "wingsmith_wings": bool(self.options.wingsmith_wings),
            "enemysanity": bool(self.options.enemysanity),
            "spiritsanity": bool(self.options.spiritsanity),
            "shuffle_orbs": bool(self.options.shuffle_orbs),
            # Every location id this slot has. The game's client (APCpp) is
            # not given the server's missing_locations, and it needs to know
            # which chests, spirits and givers are checks in this seed.
            "locations": sorted(loc.address for loc in self.multiworld.get_locations(self.player)
                                if loc.address is not None),
            # The game's own settings from the yaml, and the seed its
            # randomizer rolls them with, so every session of this slot
            # plays the same shuffle.
            "rando_seed": self.random.getrandbits(31),
            "settings": {
SEED_SLOT_HERE            },
        }
PY
$init_py =~ s/SEED_SLOT_HERE/$seed_slot/;

# ---------------------------------------------------------------- yaml
my $yaml = <<"YAML";
# Quest 64 Recompiled - Archipelago player template
# Generated by tools/archipelago_world.pl for Archipelago 0.6.7.
#
# Drop quest64.apworld in Archipelago/custom_worlds and this file in
# Archipelago/Players, then run ArchipelagoGenerate.

name: Player{number}
description: Default Quest 64 Recompiled Template
game: $game
requires:
  version: 0.6.7

$game:
  # Which groups of locations are Archipelago checks.
  #
  # chestsanity   the @{[ $by_group{chest} // 0 ]} treasure chests
  # giftsanity    the @{[ $by_group{giver} // 0 ]} NPCs who hand something over
  # enemysanity   defeating each of the @{[ $by_group{enemy} // 0 ]} kinds of regular monster. One
  #               check a kind, sent the first time you beat one of them
  # spiritsanity  the @{[ $by_group{spirit} // 0 ]} spirits. With this on a spirit sends a check
  #               instead of raising an element, and the element-choice
  #               screen comes back as the "Level Up" item.
  #
  # The @{[ $by_group{boss} // 0 ]} bosses are always checks.
  chestsanity:
    'true': 1
    'false': 0
  giftsanity:
    'true': 1
    'false': 0

  # With giftsanity on, the six wingsmiths still hand over their wings as
  # well as sending their check (once each).
  wingsmith_wings:
    'false': 1
    'true': 0
  enemysanity:
    'true': 0
    'false': 1
  spiritsanity:
    'true': 1
    'false': 0

  # Leave the way-openers where the game puts them: the Earth Orb, Wind Jade,
  # Water Jewel and Fire Ruby on their bosses, and the Eletale's Book and
  # Dark Gaol Key with the two Shannons. Everything else still moves.
  shuffle_orbs:
    'true': 1
    'false': 0

  # Extra "Level Up" items on top of the one per spirit, 0 to 99. Nothing is
  # added to the game world for these - they arrive like any other item and
  # bring up the element-choice screen - so a run can hand out more element
  # levels than the game has spirits. They replace filler, not add to the
  # pool.
  extra_level_ups: 0

  # What it takes to open the way into Mammon's World. In the vanilla game
  # the last door of the final staircase wants the Eletale's Book; any other
  # choice replaces the Book - the door opens once the condition is met, the
  # Book leaves the pool and the Shannon who hands it over becomes a gift
  # check of her own.
  #
  # vanilla       the Book, the game's own gate
  # all_bosses    every boss before Mammon: Solvaring, Zelse, Nepty,
  #               Shilf, Fargo, Guilty and Beigis
  # all_monsters  one of each of the @{[ $by_group{enemy} // 0 ]} kinds of regular monster.
  #               Turns enemysanity on, because the server is what keeps the
  #               count - a monster leaves nothing in the save to read back
  # both          all of it
  mammon_portal:
    vanilla: 1
    all_bosses: 0
    all_monsters: 0
    both: 0

  # Bosses are not in the game until their Soul turns up: an empty arena you
  # walk straight through, the same state the game uses for a boss already
  # beaten. The Soul is an Archipelago item, not one the game knows about, so
  # it can be anywhere in the multiworld. Whether a boss is there is settled
  # as his arena loads, so a Soul that arrives while you are standing in one
  # takes effect the next time you walk in. With Souls on, everything the
  # four gems lock (doors, boats, teleporters) is open from the start;
  # the Dark Gaol Key still locks the endgame, and so does the Eletale's
  # Book unless mammon_portal replaces it.
  #
  # off           every boss is where the game puts him
  # bosses        the seven before Mammon need Souls
  # with_mammon   Mammon needs one as well, so the last fight waits on it
  boss_souls:
    off: 1
    bosses: 0
    with_mammon: 0

  goal:
    mammon: 1
$seed_yaml  progression_balancing: 50
  accessibility: full
YAML

# ---------------------------------------------------------------- write
make_path("$out/quest64/docs");
my %files = (
    "$out/quest64/Locations.py" => $locations_py,
    "$out/quest64/Items.py"     => $items_py,
    "$out/quest64/Options.py"   => $options_py,
    "$out/quest64/Regions.py"   => $regions_py,
    "$out/quest64/Rules.py"     => $rules_py,
    "$out/quest64/__init__.py"  => $init_py,
    # The apworld manifest. Rule Builder (rule_builder) is what sets the
    # floor at 0.6.7.
    "$out/quest64/archipelago.json" => qq({"game": "$game", "minimum_ap_version": "0.6.7", "world_version": "1.3.0", "authors": ["Fuzzyness"], "version": 7, "compatible_version": 7}\n),
    "$out/quest64/docs/en_quest64.md" => "# Quest 64 Recompiled\n\nEvery chest, gift, boss and spirit can hold an item from any world in the\nmultiworld. Turn the Archipelago Connector on in the port's menu and give it\nthe server address and your slot name.\n",
    "$out/quest64/docs/guide_en.md"   => "# Quest 64 Recompiled Setup Guide\n\n1. Put `quest64.apworld` in `Archipelago/custom_worlds`.\n2. Put your filled-in `Quest64Recompiled.yaml` in `Archipelago/Players`.\n3. Generate and host as usual.\n4. In Quest 64 Recompiled, open the config menu, turn on the Archipelago\n   Connector and enter the server address, your slot name and the password\n   if the room has one.\n",
    "$out/Quest64Recompiled.yaml" => $yaml,
);
for my $path (sort keys %files) {
    make_path(dirname($path));
    open my $f, '>', $path or die "$path: $!";
    print $f $files{$path};
    close $f;
}

my $apworld = "$out/quest64.apworld";
unlink $apworld;
my @members = grep { m{/quest64/} } sort keys %files;
zip [ @members ] => $apworld,
    FilterName => sub { s{^\Q$out/\E}{} },
    Zip64 => 0
    or die "zip failed: $ZipError";

# The guaranteed items must fit even when only the smallest group of
# locations is switched on, and there has to be something to pad with.
{
    my $must = 0;
    $must += $_->{count} for grep { $_->{name} ne 'Level Up' } @item_rows;
    my $fewest = $by_group{boss} + $by_group{spirit};   # spiritsanity alone
    die "guaranteed items ($must) do not fit in $fewest locations
" if $must > $fewest;
    die "no filler items
" unless grep { $_->{type} eq 'filler' } @item_rows;
    # Every item a rule waits on must actually be in the pool, or what it
    # guards is unreachable and the seed cannot be finished.
    my @needed = sort keys %{ $logic->{uses}{items} // {} };
    my @unplaceable = grep { my $n = $_; !grep { $_->{name} eq $n && $_->{count} > 0 } @item_rows } @needed;
    die "rules need items with none in the pool: @unplaceable\n" if @unplaceable;
}

printf "wrote %s\n", $apworld;
printf "  %d locations: %d chests, %d givers, %d enemies, %d bosses, %d spirits\n",
    scalar @loc, $by_group{chest} // 0, $by_group{giver} // 0, $by_group{enemy} // 0, $by_group{boss} // 0, $by_group{spirit} // 0;
printf "  %d item kinds, %d items in the pool\n",
    scalar(grep { $_->{count} > 0 } @item_rows),
    eval { my $t = 0; $t += $_->{count} for @item_rows; $t };
printf "wrote %s/Quest64Recompiled.yaml\n", $out;
