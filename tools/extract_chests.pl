#!/usr/bin/perl
# Writes src/game/randomizer/chest_data.cpp: where the game's 88 chests are,
# and every place one could be put instead.
#
# Chests are placed by func_80011B70 at map load, through two tables:
#
#   outer, RAM 0x8004C470 (ROM 0x4D070), 19 entries of 8 bytes:
#       u32 map, u32 per-submap array
#   per-submap, in that map's own data, indexed by submap, 8 bytes each:
#       u16 count, u16 pad, u32 records
#   record, 36 bytes:
#       f32 x, f32 z, f32 facing,
#       f32 open_x, f32 open_z, f32 open_facing,
#       f32 width, f32 depth, u8 id, u8 item, u16 pad
#
# Like a spirit, a chest has no y: the routine hands x and -z to the ground
# query at func_8000EE60 and takes the height back, so a chest only needs an
# x and a z over something solid. `id` is the save flag's bit and `item` is
# what is inside - the byte Merrow's chest shuffle writes, at +33.
#
# A chest faces (sin f, cos f), and open_x/open_z is where Brian stands to
# open it: six units along that direction, facing back (f +/- pi). That pair
# is what makes a placement safe - the chest is not against a wall and there
# is floor to stand on - so a spot is only offered here if its facing and
# opening position can be justified:
#
#   vanilla    the 88 chest spots, with the game's own facing. Proven.
#   spirit     a vanilla spirit spot. Open ground, but no facing of its own,
#              so the chest is aimed at the nearest other known-walkable
#              point in the same submap and Brian stands on that line.
#              Dropped if it is within DOOR_CLEAR of an entrance, so a chest
#              never lands in a doorway.
#
# A third kind, "made" - a point interpolated along the line between two
# known-walkable points outdoors - was tried on 2026-09-22 and taken out again
# the same day. It put a chest inside a fence in Melrode: a line between two
# walkable points can cross one, and nothing offline can see that. Only
# positions the game itself uses are offered now, which leaves 68 spots
# outdoors against 88 chests, so "outdoors only" fills what it can and the
# rest of the chests stay where they are - which is why a chest carries its
# own position below.
#
# The map table at RAM 0x80054F10 turns a map-data pointer into a ROM offset:
# +4 ROM start, +8 ROM end, +0xC RAM destination, +0x18 submap headers,
# +0x40 submap count. Entrance spawn positions come from the submap headers
# (+0xC a table of 20-byte entries, f32 x, f32 z, f32 facing; +0x10 count),
# the same ones tools/extract_spirits.pl uses.
#
#   perl tools/extract_chests.pl quest64.us.z64 > src/game/randomizer/chest_data.cpp
use strict; use warnings;

use constant BOOT_ROM      => 0x1000;
use constant BOOT_RAM      => 0x80000400;
use constant MAP_TABLE_RAM => 0x80054F10;
use constant MAP_COUNT     => 36;
use constant MAP_STRIDE    => 0x44;
use constant CHEST_TABLE   => 0x8004C470;
use constant CHEST_MAPS    => 19;
use constant SPIRIT_TABLE  => 0x4D110;
use constant SPIRIT_SLOTS  => 43;
use constant RECORD        => 36;

# How far in front of a chest Brian stands, taken from the vanilla records.
use constant REACH      => 6.0;
# A spot this close to an entrance spawn position would block the doorway.
use constant DOOR_CLEAR => 20.0;
# A made spot keeps this far from an NPC standing about, and from any other
# spot, so nothing shares its ground.
use constant NPC_CLEAR  => 25.0;
use constant SPOT_CLEAR => 22.0;
# The longest line between two known-walkable points a made spot may sit on.
use constant PAIR_MIN   => 25.0;
use constant PAIR_MAX   => 140.0;

# Fields, towns and forests. Everything else - the building sets, castles,
# caves and dungeons - counts as indoors.
my %outdoor = map { $_ => 1 } (0 .. 12, 31, 32, 33, 34);

my $romfile = shift or die "usage: $0 quest64.us.z64 > chest_data.cpp\n";
open my $fh, '<:raw', $romfile or die "$romfile: $!\n";
local $/;
my $rom = <$fh>;
close $fh;

sub u32 { unpack 'N', substr($rom, $_[0], 4) }
sub u16 { unpack 'n', substr($rom, $_[0], 2) }
sub f32 { unpack 'f>', substr($rom, $_[0], 4) }
sub boot { return BOOT_ROM + ($_[0] - BOOT_RAM) }

my $map_table = boot(MAP_TABLE_RAM);
my @maps;
for my $m (0 .. MAP_COUNT - 1) {
    my $e = $map_table + $m * MAP_STRIDE;
    $maps[$m] = { rom_start => u32($e + 4), rom_end => u32($e + 8), ram => u32($e + 0xC),
                  submaps => u32($e + 0x18), count => u32($e + 0x40) };
}
sub to_rom { my ($m, $p) = @_; return $p - $maps[$m]{ram} + $maps[$m]{rom_start}; }
sub map_of { my $r = shift; for my $m (0 .. MAP_COUNT - 1) { return $m if $r >= $maps[$m]{rom_start} && $r < $maps[$m]{rom_end} } return -1; }

my $max_submaps = 0;
for my $m (0 .. MAP_COUNT - 1) { $max_submaps = $maps[$m]{count} if $maps[$m]{count} > $max_submaps }

# ---- the 88 chests
my (@chests, %chest_at);
my $outer = boot(CHEST_TABLE);
for my $i (0 .. CHEST_MAPS - 1) {
    my ($map, $per) = (u32($outer + $i * 8), u32($outer + $i * 8 + 4));
    my $p = to_rom($map, $per);
    for my $s (0 .. $maps[$map]{count} - 1) {
        my $count = u16($p + $s * 8);
        next unless $count;
        my $r = to_rom($map, u32($p + $s * 8 + 4));
        for my $k (0 .. $count - 1) {
            my $o = $r + $k * RECORD;
            my %c = (map => $map, submap => $s, rom => $o,
                     x => f32($o), z => f32($o + 4), facing => f32($o + 8),
                     ox => f32($o + 0xC), oz => f32($o + 0x10), ofacing => f32($o + 0x14),
                     w => f32($o + 0x18), d => f32($o + 0x1C),
                     id => ord(substr($rom, $o + 0x20, 1)), item => ord(substr($rom, $o + 0x21, 1)));
            push @chests, \%c;
            $chest_at{ $c{id} } = \%c;
        }
    }
}
@chests = sort { $a->{id} <=> $b->{id} } @chests;
die sprintf("found %d chests, expected 88\n", scalar @chests) unless @chests == 88;
for my $i (0 .. $#chests) {
    die "chest ids are not 0..87\n" if $chests[$i]{id} != $i;
}

# ---- known-walkable points: spirit spots and entrance spawn positions
my (%walkable, %doors);
my $id = 0;
for my $i (0 .. SPIRIT_SLOTS - 1) {
    my $e = SPIRIT_TABLE + $i * 12;
    my ($map, $submap, $count, $ptr) = (u16($e), u16($e + 2), u16($e + 4), u32($e + 8));
    my $r = to_rom($map, $ptr);
    for my $k (0 .. $count - 1) {
        push @{ $walkable{"$map.$submap"} }, [ f32($r + 12 * $k), f32($r + 12 * $k + 4), 'spirit' ];
    }
    $id += $count;
}
for my $m (0 .. MAP_COUNT - 1) {
    my $headers = to_rom($m, $maps[$m]{submaps});
    for my $s (0 .. $maps[$m]{count} - 1) {
        my $h = $headers + $s * 24;
        my ($spawn, $count) = (u32($h + 0xC), u16($h + 0x10));
        next if !$count || !$spawn;
        my $t = to_rom($m, $spawn);
        for my $k (0 .. $count - 1) {
            my @p = (f32($t + 20 * $k), f32($t + 20 * $k + 4));
            push @{ $walkable{"$m.$s"} }, [ @p, 'door' ];
            push @{ $doors{"$m.$s"} }, \@p;
        }
    }
}
for my $c (@chests) {
    push @{ $walkable{"$c->{map}.$c->{submap}"} }, [ $c->{x}, $c->{z}, 'chest' ];
}

# ---- the NPCs standing about, which a made spot keeps clear of
my %npcs;
my $merrow = "src/game/randomizer/merrow_data.cpp";
open my $md, '<', $merrow or die "$merrow: $! (run from the repository root)\n";
my $data = <$md>;
close $md;
my ($block) = $data =~ /npcmovement = \{(.*?)\n\s*\};/s;
die "npcmovement not found in $merrow\n" unless $block;
for my $r ($block =~ /(\d+)/g) {
    my $m = map_of($r);
    next if $m < 0;
    push @{ $npcs{$m} }, [ f32($r + 0x18), f32($r + 0x1C) ];
}

sub dist { my ($a, $b) = @_; return sqrt(($a->[0] - $b->[0])**2 + ($a->[1] - $b->[1])**2); }
sub clear_of {
    my ($p, $list, $least) = @_;
    for my $q (@$list) { return 0 if dist($p, $q) < $least }
    return 1;
}

# Aim a chest at the nearest other known-walkable point, so its open side
# looks down a line the game already walks rather than into a wall.
sub aim {
    my ($map, $submap, $p) = @_;
    my ($best, $at);
    for my $q (@{ $walkable{"$map.$submap"} }) {
        my $d = dist($p, $q);
        next if $d < 0.5;
        if (!defined $best || $d < $best) { $best = $d; $at = $q }
    }
    return () unless defined $at;
    my $facing = atan2($at->[0] - $p->[0], $at->[1] - $p->[1]);
    return ($facing, $p->[0] + sin($facing) * REACH, $p->[1] + cos($facing) * REACH);
}

# ---- the spots
my @spots;
for my $c (@chests) {
    push @spots, { map => $c->{map}, submap => $c->{submap}, x => $c->{x}, z => $c->{z},
                   facing => $c->{facing}, ox => $c->{ox}, oz => $c->{oz}, kind => 'vanilla' };
}
for my $key (sort keys %walkable) {
    my ($m, $s) = split /\./, $key;
    for my $p (@{ $walkable{$key} }) {
        next unless $p->[2] eq 'spirit';
        next unless clear_of($p, $doors{$key} || [], DOOR_CLEAR);
        my ($facing, $ox, $oz) = aim($m, $s, $p);
        next unless defined $facing;
        push @spots, { map => $m, submap => $s, x => $p->[0], z => $p->[1],
                       facing => $facing, ox => $ox, oz => $oz, kind => 'spirit' };
    }
}
my %by_kind;
$by_kind{ $_->{kind} }++ for @spots;
my $outdoor_spots = grep { $outdoor{ $_->{map} } } @spots;
printf STDERR "88 chests; spots: %d vanilla, %d spirit = %d spots (%d outdoors)\n",
    $by_kind{vanilla} // 0, $by_kind{spirit} // 0, scalar @spots, $outdoor_spots;

@spots = sort { $a->{map} <=> $b->{map} || $a->{submap} <=> $b->{submap} || $a->{x} <=> $b->{x} || $a->{z} <=> $b->{z} } @spots;

print <<"HEADER";
// Generated from the US ROM by tools/extract_chests.pl. Do not edit by hand.
// See that script for the tables these came out of and for what makes a spot
// safe to put a chest on.
//
// The game's 88 chests, and @{[ scalar @spots ]} places one can go (@{[ $outdoor_spots ]} of them out of
// doors): @{[ $by_kind{vanilla} // 0 ]} the game's own chest spots, @{[ $by_kind{spirit} // 0 ]} spirit spots clear of any
// doorway - all of them positions the game itself uses.
#include "chest_data.h"

namespace merrow::chests {
    const std::vector<Chest> chests = {
HEADER

printf("        { %2d, %2d, %3d, 0x%06X, %2d, %12.4ff, %12.4ff, %9.5ff, %12.4ff, %12.4ff },\n",
       $_->{map}, $_->{submap}, $_->{id}, $_->{rom}, $_->{item},
       $_->{x}, $_->{z}, $_->{facing}, $_->{ox}, $_->{oz}) for @chests;

print <<'MIDDLE';
    };

    const std::vector<Spot> spots = {
MIDDLE

my %kind_name = (vanilla => 'Kind::Vanilla', spirit => 'Kind::Spirit');
printf("        { %2d, %2d, %12.4ff, %12.4ff, %9.5ff, %12.4ff, %12.4ff, %s, %s },\n",
       $_->{map}, $_->{submap}, $_->{x}, $_->{z}, $_->{facing}, $_->{ox}, $_->{oz},
       $kind_name{ $_->{kind} }, $outdoor{ $_->{map} } ? 'true' : 'false') for @spots;

print <<"FOOTER";
    };

    const std::vector<uint8_t> map_submap_counts = {
FOOTER
print "        ", join(", ", map { $maps[$_]{count} } 0 .. MAP_COUNT - 1), ",\n";
print <<"TAIL";
    };
}
TAIL
printf STDERR "widest map has %d submaps\n", $max_submaps;
