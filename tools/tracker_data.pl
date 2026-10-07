#!/usr/bin/perl
# Writes src/game/tracker_data.cpp: every check the tracker shows, grouped
# into areas, and which area each room (map and submap) of the game is in.
#
#   perl tools/tracker_data.pl
#
# The checks come from the logic workbook's Locations sheet (name, id, group),
# the same rows the apworld is generated from, so the tracker and the
# Archipelago locations cannot disagree.
#
# A check's area is the area of the room it is in, not its name. The game
# keeps building interiors in shared "building set" maps named for one town
# that serve several areas: the Dondoran set (map 16) has houses on Holy
# Plains and Dondoran Flats, the Limelin set rooms off West Limelin, the
# Dindom Dries & Brannoch set rooms on Baragoon Moor. Filed by name, a hut on
# Holy Plains was Dondoran. So:
#
#   - an outdoor or dungeon room is its map's area (with map 11 split into
#     Brannoch town, Baragoon Moor and the castle grounds);
#   - an interior room (maps 13-25) is the area of the room whose door leads
#     into it, from the exit records in src/game/map_table.cpp, followed
#     through rooms inside rooms; Mammon's World's doors into the old towns
#     are only used when nothing else leads in;
#   - the room of each spirit and chest is read from the tables the game
#     places them from (RAM 0x8004C510 and 0x8004C470, in the boot segment of
#     quest64.us.z64), each gift NPC's from src/game/minimap.cpp's list.
#
# Bosses are placed by name (their arenas are not rooms the tables list).
use strict;
use warnings;
use File::Basename qw(dirname);

use lib dirname(__FILE__);
use Quest64Logic;

my $root = dirname(__FILE__) . '/..';

# Tracker areas in the order they are listed. The prefixes are only used for
# bosses, by their workbook place; the first match wins.
my @areas = (
    [ 'Melrode',          'Monastery', 'Melrode' ],
    [ 'Holy Plains',      'Holy Plains' ],
    [ 'Connor Forest',    'Connor Forest' ],
    [ 'Dondoran Flats',   'Dondoran Flats' ],
    [ 'Dondoran',         'Dondoran' ],
    [ 'Glencoe Forest',   'Glencoe' ],
    [ 'West Carmaugh',    'West Carmagh' ],
    [ 'Larapool',         'Larapool' ],
    [ 'Cull Hazard',      'Cull Hazard' ],
    [ 'Normoon',          'Normoon' ],
    [ 'Windward Forest',  'Windward' ],
    [ 'West Limelin',     'West Limelin' ],
    [ 'Limelin',          'Limelin' ],
    [ 'Blue Cave',        'Blue Cave', "Nepty's arena" ],
    [ 'Isle of Skye',     'Isle of Skye' ],
    [ 'Baragoon Tunnel',  'Baragoon Tunnel' ],
    [ 'Dindom Dries',     'Dindom' ],
    [ 'Boil Hole',        'Boil Hole' ],
    [ 'Shamwood',         'Shamwood' ],
    [ 'Baragoon Moor',    'Baragoon Moor' ],
    [ 'Brannoch',         ],
    [ 'Brannoch Castle',  'Brannoch Castle' ],
    [ "Mammon's World",   "Mammon's World" ],
    [ 'Hidden Rooms',     ],
    [ 'Monsters' ],   # every monster check, wherever it is met
);
my $monsters_area = $#areas;
my %area_index = map { $areas[$_][0] => $_ } 0 .. $#areas;
sub idx { my $n = shift; return $area_index{$n} // die "no tracker area \"$n\"\n" }

sub area_by_name {
    my $place = shift;
    for my $i (0 .. $#areas) {
        my ($name, @prefixes) = @{ $areas[$i] };
        for my $p (@prefixes) {
            return $i if index($place, $p) == 0;
        }
    }
    return -1;
}

sub c_str { my $s = shift; $s =~ s/\\/\\\\/g; $s =~ s/"/\\"/g; return "\"$s\"" }
sub slurp { my $p = shift; open my $f, '<:raw', $p or die "$p: $!"; local $/; my $s = <$f>; close $f; $s =~ s/\r//g; return $s }

# ---------------------------------------------------------------- rooms
my $map_src = slurp("$root/src/game/map_table.cpp");
my @submaps;   # per map, its submap count
{
    my ($counts) = ($map_src =~ /map_entrance_counts = \{(.*?)\n    \};/s) or die "no map_entrance_counts\n";
    while ($counts =~ /\{ ([\d, ]*?),? \}, \/\/ map (\d+)/g) {
        my @n = split /,\s*/, $1;
        $submaps[$2] = scalar @n;
    }
    die "expected 36 maps, found " . scalar(@submaps) . "\n" unless @submaps == 36;
}
my @exits;
while ($map_src =~ /^\s+\{ (\d+), (\d+), (\d+), (\d+), (\d+) \},$/mg) { push @exits, [ $1, $2, $3, $4 ] }
die "no exits read from map_table.cpp\n" unless @exits > 400;

my %outdoor = (
    0 => 'Melrode', 1 => 'Dondoran', 2 => 'Holy Plains', 3 => 'Dondoran Flats', 4 => 'Larapool',
    5 => 'West Carmaugh', 6 => 'Normoon', 7 => 'West Limelin', 8 => 'Limelin', 9 => 'Dindom Dries',
    10 => 'Shamwood', 11 => 'Baragoon Moor', 12 => 'Isle of Skye', 26 => 'Blue Cave', 27 => 'Cull Hazard',
    28 => 'Baragoon Tunnel', 29 => 'Boil Hole', 30 => 'Brannoch Castle', 31 => 'Connor Forest',
    32 => 'Glencoe Forest', 33 => 'Windward Forest', 34 => "Mammon's World", 35 => 'Blue Cave',
);
sub interior { my $m = shift; return $m >= 13 && $m <= 25 }
sub outdoor_area {
    my ($m, $s) = @_;
    # Map 11: submap 0 is Brannoch town (every house door), 1 and 2 Baragoon
    # Moor (to the castle, to Boil Hole), 3-7 the castle grounds from the
    # roof out to Mammon's World (Shannon is in 4).
    return 'Brannoch' if $m == 11 && $s == 0;
    return 'Brannoch Castle' if $m == 11 && $s >= 3;
    # Map 0's submaps 2-4 only lead into Mammon's World: its Melrode stage.
    return "Mammon's World" if $m == 0 && $s >= 2;
    return $outdoor{$m} // die "map $m: no outdoor area\n";
}

my %room;   # "map/submap" -> area name
for my $m (0 .. 35) {
    next if interior($m);
    $room{"$m/$_"} = outdoor_area($m, $_) for 0 .. $submaps[$m] - 1;
}
{
    my %into;
    for my $e (@exits) {
        my ($sm, $ss, $dm, $ds) = @$e;
        next unless interior($dm) && !($sm == $dm && $ss == $ds);
        push @{ $into{"$dm/$ds"} }, "$sm/$ss";
    }
    # Rooms inside rooms take a few passes to settle.
    for my $pass (1 .. 8) {
        for my $k (sort keys %into) {
            next if defined $room{$k};
            my @from = grep { defined } map { $room{$_} } @{ $into{$k} };
            my @not_mammon = grep { $_ ne "Mammon's World" } @from;
            @from = @not_mammon if @not_mammon;
            $room{$k} = $from[0] if @from;
        }
    }
}
# The hidden rooms and shrines (map 25) keep an area of their own, except the
# ship to the Isle of Skye (25/3 and its cabin 25/4), which no door leads to.
for my $s (0 .. $submaps[25] - 1) {
    $room{"25/$s"} = ($s == 3 || $s == 4) ? 'Isle of Skye' : 'Hidden Rooms';
}
# Anything no door leads into is filed under its building set's own town.
my %set_town = (13 => 'Melrode', 14 => 'Dondoran', 15 => 'Melrode', 16 => 'Dondoran', 17 => 'Larapool',
                18 => 'Larapool', 19 => 'Normoon', 20 => 'Normoon', 21 => 'Limelin', 22 => 'Limelin',
                23 => 'Dindom Dries', 24 => 'Shamwood');
for my $m (13 .. 24) {
    $room{"$m/$_"} //= $set_town{$m} for 0 .. $submaps[$m] - 1;
}
sub room_area {
    my ($m, $s) = @_;
    return idx($room{"$m/$s"} // die "room $m/$s: no area\n");
}

# ---------------------------------------------------------------- rooms of checks
my $rom = do { open my $f, "<:raw", "$root/quest64.us.z64" or die "quest64.us.z64: $!"; local $/; <$f> };
sub boot { my $ram = shift; return $ram - 0x80000450 + 0x1050 }
my @map_rom;   # per map: rom start, end, ram destination
{
    my $mt = boot(0x80054F10);
    for my $m (0 .. 35) { $map_rom[$m] = [ unpack('N3', substr($rom, $mt + $m * 0x44 + 4, 12)) ] }
}
sub map_ptr { my ($m, $p) = @_; return $p - $map_rom[$m][2] + $map_rom[$m][0] }

# Spirits: 43 slots of { u16 map, u16 submap, u16 count, u16 pad, u32 records },
# records { f32 x, f32 z, u8 id, ... } 12 bytes apart.
my %spirit_room;
{
    my $t = boot(0x8004C510);
    for my $i (0 .. 42) {
        my ($m, $s, $count, undef, $p) = unpack('n4N', substr($rom, $t + $i * 12, 12));
        next unless $p;
        my $o = map_ptr($m, $p);
        $spirit_room{ unpack('C', substr($rom, $o + $_ * 12 + 8, 1)) } = [ $m, $s ] for 0 .. $count - 1;
    }
    die "spirit table: " . scalar(keys %spirit_room) . " spirits, expected 98\n" unless keys %spirit_room == 98;
}
# Chests: 19 slots of { u32 map, u32 per-submap array }, each submap
# { u16 count, u16 pad, u32 records }, records 36 bytes. Matched to Merrow's
# chest list (the apworld's chest order) by the record's ROM address.
my %chest_room_at;
{
    my $t = boot(0x8004C470);
    for my $i (0 .. 18) {
        my ($m, $p) = unpack('NN', substr($rom, $t + $i * 8, 8));
        next unless $p;
        my $o = map_ptr($m, $p);
        for my $s (0 .. $submaps[$m] - 1) {
            my ($count, undef, $rp) = unpack('nnN', substr($rom, $o + $s * 8, 8));
            next unless $count && $rp;
            my $ro = map_ptr($m, $rp);
            $chest_room_at{ $ro + $_ * 36 } = [ $m, $s ] for 0 .. $count - 1;
        }
    }
}
my @chest_addr;
{
    my $data = slurp("$root/src/game/randomizer/merrow_data.cpp");
    my ($body) = ($data =~ /chestdata = \{(.*?)\};/s) or die "no chestdata\n";
    $body =~ s{//[^\n]*}{}g;
    my @v = ($body =~ /(\d+)/g);
    push @chest_addr, $v[ $_ * 4 ] for 0 .. @v / 4 - 1;
}
sub chest_room {
    my $i = shift;
    my $a = $chest_addr[$i] // die "chest $i: not in chestdata\n";
    return $chest_room_at{$a} // die sprintf("chest %d (ROM 0x%X): not in the chest table\n", $i, $a);
}
# Gift NPCs, in the apworld's giver order.
my @giver_room;
{
    my $mm = slurp("$root/src/game/minimap.cpp");
    my ($body) = ($mm =~ /GiverSpot givers\[\] = \{(.*?)\n    \};/s) or die "no giver list in minimap.cpp\n";
    while ($body =~ /\{\s*(\d+),\s*(\d+),/g) { push @giver_room, [ $1, $2 ] }
    die "expected 16 gift NPCs, found " . scalar(@giver_room) . "\n" unless @giver_room == 16;
}

# ---------------------------------------------------------------- checks
my %kind_of = ('Chest' => 'Chest', 'Gift NPC' => 'Giver', 'Monster' => 'Monster', 'Boss' => 'Boss', 'Spirit' => 'Spirit');
my %prefix_of = (Chest => 'Chest - ', Giver => 'Giver - ', Monster => 'Defeat - ', Boss => 'Boss - ', Spirit => 'Spirit - ');
my %label_of = (Chest => 'Chest', Giver => 'Gift', Monster => '', Boss => 'Boss', Spirit => 'Spirit');

my $book = Quest64Logic::read_workbook("$root/DOCS/archipelago_logic.xlsx");
my $rows = $book->{Locations} or die "no Locations sheet\n";
my @checks;
for my $r (4 .. $#$rows) {
    my $name = Quest64Logic::cell($rows, $r, 1);
    next if $name eq '';
    my ($id, $group, $place) = (Quest64Logic::cell($rows, $r, 2), Quest64Logic::cell($rows, $r, 3), Quest64Logic::cell($rows, $r, 4));
    my $kind = $kind_of{$group} // die "Locations row $r: unknown group \"$group\"\n";
    my $index = hex($id) & 0xFFF;
    my $area;
    if    ($kind eq 'Monster') { $area = $monsters_area }
    elsif ($kind eq 'Spirit')  { $area = room_area(@{ $spirit_room{$index} // die "spirit $index: no room\n" }) }
    elsif ($kind eq 'Chest')   { $area = room_area(@{ chest_room($index) }) }
    elsif ($kind eq 'Giver')   { $area = room_area(@{ $giver_room[$index] // die "giver $index: no room\n" }) }
    else                       { $area = area_by_name($place) }
    die "Locations row $r: no tracker area for \"$place\"\n" if $area < 0;
    (my $label = $name) =~ s/^\Q$prefix_of{$kind}\E//;
    $label = "$label_of{$kind}: $label" if $label_of{$kind} ne '';
    push @checks, { id => hex($id), kind => $kind, area => $area, label => $label };
}
die "expected 277 checks, found " . scalar(@checks) . "\n" unless @checks == 277;

# ---------------------------------------------------------------- write
my $out = "// GENERATED by tools/tracker_data.pl from DOCS/archipelago_logic.xlsx,\n"
        . "// src/game/map_table.cpp, src/game/minimap.cpp and quest64.us.z64; do not edit.\n"
        . "#include \"tracker.h\"\n\n"
        . "namespace zelda64::tracker {\n\n"
        . "const std::vector<const char*> area_names = {\n"
        . join('', map { "    " . c_str($_->[0]) . ",\n" } @areas)
        . "};\n\n"
        . "const int monsters_area = $monsters_area;\n\n"
        . "const std::vector<CheckInfo> checks = {\n";
for my $c (@checks) {
    $out .= sprintf("    { 0x%08X, Kind::%s, %d, %s },\n", $c->{id}, $c->{kind}, $c->{area}, c_str($c->{label}));
}
$out .= "};\n\n"
      . "const std::vector<std::vector<int>> room_area = {\n";
for my $m (0 .. 35) {
    $out .= "    { " . join(', ', map { room_area($m, $_) } 0 .. $submaps[$m] - 1) . " },   // map $m\n";
}
$out .= "};\n\n}\n";

my $path = "$root/src/game/tracker_data.cpp";
open my $o, '>', $path or die "$path: $!";
print $o $out;
close $o;
printf "wrote %s: %d checks in %d areas\n", $path, scalar(@checks), scalar(@areas);
