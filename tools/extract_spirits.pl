#!/usr/bin/perl
# Writes src/game/randomizer/spirit_data.cpp: where the game's 98 spirits are,
# and every other spot in the game that is known to be somewhere Brian can
# stand.
#
# The spirit table is 43 entries of 12 bytes at RAM 0x8004C510 (ROM 0x4D110,
# inside the boot segment), read by func_80012220 on every map load:
#
#   u16 map, u16 submap, u16 count, u16 pad, u32 records
#
# and each record is 12 bytes:
#
#   f32 x, f32 z, u8 id, u8 pad[3]
#
# There is no y: func_80012220 hands x and -z to the ground query at
# func_8000EE60 and takes the height it returns, so a spirit only needs an x
# and a z over something solid. The id is the save flag's bit, numbered
# straight through the table (entry 0's first spirit is 0), which is what lets
# this script find each array in the ROM - although it does not have to,
# because the map table gives the addresses exactly.
#
# The records live in each map's own data, so a RAM pointer is turned into a
# ROM offset with the map table at RAM 0x80054F10 (0x44 bytes a map, 36 maps):
# +4 ROM start, +8 ROM end, +0xC RAM destination, +0x18 the submap headers,
# +0x40 the submap count. ROM = pointer - destination + start.
#
# The second pool comes from the same submap headers: +0xC is the entrance
# spawn table (20 bytes each: f32 x, f32 z, f32 facing, then flags) and +0x10
# its count. Those are the positions the game itself stands Brian on when he
# walks into a submap, so they are as good a guarantee of solid ground as the
# spirit spots themselves - 542 of them across 258 submaps, against the 98
# spirit spots, which is what makes a shuffle worth doing at all. Unlike a
# chest, a spirit in a doorway is no trouble: you pick it up walking through.
#
# The third pool is made rather than read, the same way tools/extract_chests.pl
# makes its outdoor spots: a point a third, a half or two thirds of the way
# along the line between two known-walkable points in the same submap. Both
# ends are ground the game itself stands something on, so a short line between
# them is very nearly certain to be walkable too - "very nearly", not
# "certainly", which is why the line is kept short: PAIR_MAX_IN indoors, where
# a long one could cross a wall, and PAIR_MAX_OUT in the open. A spirit asks
# less of a spot than a chest does - no facing, and no clear floor in front to
# stand on - so these can be made indoors as well, and only need to keep clear
# of the NPCs standing about (npcmovement's records carry x at +0x18, z at
# +0x1C) so one never ends up inside somebody.
#
#   perl tools/extract_spirits.pl quest64.us.z64 > src/game/randomizer/spirit_data.cpp
use strict; use warnings;

use constant MAP_TABLE_RAM  => 0x80054F10;
use constant BOOT_ROM       => 0x1000;
use constant BOOT_RAM       => 0x80000400;
use constant MAP_COUNT      => 36;
use constant MAP_STRIDE     => 0x44;
use constant SPIRIT_TABLE   => 0x4D110;
use constant SPIRIT_SLOTS   => 43;

# Made spots: the line between two known-walkable points may be this long,
# and the point taken off it keeps this far from an NPC and from every other
# spot. Indoors is kept short because a long line in a building is much more
# likely to cross a wall than one in open country.
use constant PAIR_MIN     => 20.0;
use constant PAIR_MAX_IN  => 70.0;
use constant PAIR_MAX_OUT => 140.0;
use constant NPC_CLEAR    => 20.0;
use constant SPOT_CLEAR   => 18.0;

# Fields, towns and forests; everything else counts as indoors.
my %outdoor = map { $_ => 1 } (0 .. 12, 31, 32, 33, 34);

my $romfile = shift or die "usage: $0 quest64.us.z64 > spirit_data.cpp\n";
open my $fh, '<:raw', $romfile or die "$romfile: $!\n";
local $/;
my $rom = <$fh>;
close $fh;

sub u32 { unpack 'N', substr($rom, $_[0], 4) }
sub u16 { unpack 'n', substr($rom, $_[0], 2) }
sub f32 { unpack 'f>', substr($rom, $_[0], 4) }

my $map_table = BOOT_ROM + (MAP_TABLE_RAM - BOOT_RAM);
my @maps;
for my $m (0 .. MAP_COUNT - 1) {
    my $e = $map_table + $m * MAP_STRIDE;
    $maps[$m] = {
        rom_start => u32($e + 4),
        rom_end   => u32($e + 8),
        ram       => u32($e + 0xC),
        submaps   => u32($e + 0x18),
        count     => u32($e + 0x40),
    };
}

sub to_rom {
    my ($map, $pointer) = @_;
    return $pointer - $maps[$map]{ram} + $maps[$map]{rom_start};
}

sub map_of {
    my $rom_offset = shift;
    for my $m (0 .. MAP_COUNT - 1) {
        return $m if $rom_offset >= $maps[$m]{rom_start} && $rom_offset < $maps[$m]{rom_end};
    }
    return -1;
}

sub dist { my ($a, $b) = @_; return sqrt(($a->{x} - $b->{x}) ** 2 + ($a->{z} - $b->{z}) ** 2); }
sub clear_of {
    my ($p, $list, $least) = @_;
    for my $q (@$list) { return 0 if dist($p, $q) < $least }
    return 1;
}

# The 43 slots, and the spirit spots they hold.
my (@slots, @spots);
my $id = 0;
for my $i (0 .. SPIRIT_SLOTS - 1) {
    my $e = SPIRIT_TABLE + $i * 12;
    my ($map, $submap, $count, $pointer) = (u16($e), u16($e + 2), u16($e + 4), u32($e + 8));
    my $records = to_rom($map, $pointer);
    for my $k (0 .. $count - 1) {
        my $r = $records + 12 * $k;
        my $got = ord substr($rom, $r + 8, 1);
        die sprintf("spirit id at ROM %06X is %d, expected %d\n", $r, $got, $id + $k) if $got != $id + $k;
        push @spots, { map => $map, submap => $submap, x => f32($r), z => f32($r + 4), vanilla => 1 };
    }
    push @slots, { map => $map, submap => $submap, count => $count };
    $id += $count;
}

# Every entrance spawn position.
my $entrances = 0;
for my $m (0 .. MAP_COUNT - 1) {
    my $headers = to_rom($m, $maps[$m]{submaps});
    for my $s (0 .. $maps[$m]{count} - 1) {
        my $h = $headers + $s * 24;
        my $spawn = u32($h + 0xC);
        my $count = u16($h + 0x10);
        next if !$count || !$spawn;
        my $table = to_rom($m, $spawn);
        for my $k (0 .. $count - 1) {
            push @spots, { map => $m, submap => $s, x => f32($table + 20 * $k), z => f32($table + 20 * $k + 4), vanilla => 0 };
            $entrances++;
        }
    }
}

# The NPCs standing about, which a made spot keeps clear of.
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
    push @{ $npcs{$m} }, { x => f32($r + 0x18), z => f32($r + 0x1C) };
}

# The third pool: points along the lines between the spots read above.
my %known;
push @{ $known{"$_->{map}.$_->{submap}"} }, $_ for @spots;
my @made;
for my $key (sort keys %known) {
    my ($m, $s) = split /\./, $key;
    my @here = @{ $known{$key} };
    my $longest = $outdoor{$m} ? PAIR_MAX_OUT : PAIR_MAX_IN;
    my @new;
    for my $i (0 .. $#here) {
        for my $j ($i + 1 .. $#here) {
            my $d = dist($here[$i], $here[$j]);
            next if $d < PAIR_MIN || $d > $longest;
            for my $t (0.5, 1 / 3, 2 / 3) {
                my $p = { map => $m, submap => $s, vanilla => 0, made => 1,
                          x => $here[$i]{x} + ($here[$j]{x} - $here[$i]{x}) * $t,
                          z => $here[$i]{z} + ($here[$j]{z} - $here[$i]{z}) * $t };
                next unless clear_of($p, $npcs{$m} || [], NPC_CLEAR);
                next unless clear_of($p, \@here, SPOT_CLEAR);
                next unless clear_of($p, \@new, SPOT_CLEAR);
                push @new, $p;
            }
        }
    }
    push @made, @new;
}
push @spots, @made;

# A spot is no use twice, and an entrance that sits on a spirit is the spirit.
my (%seen, @unique);
for my $s (sort { $b->{vanilla} <=> $a->{vanilla} } @spots) {
    my $key = sprintf "%d.%d.%.3f.%.3f", $s->{map}, $s->{submap}, $s->{x}, $s->{z};
    next if $seen{$key}++;
    push @unique, $s;
}
@unique = sort { $a->{map} <=> $b->{map} || $a->{submap} <=> $b->{submap} || $a->{x} <=> $b->{x} || $a->{z} <=> $b->{z} } @unique;

printf STDERR "%d spirits in %d slots, %d entrance points, %d made, %d unique spots\n",
    $id, scalar @slots, $entrances, scalar @made, scalar @unique;

print <<"HEADER";
// Generated from the US ROM by tools/extract_spirits.pl. Do not edit by hand.
// See that script for the tables these came out of.
//
// $id spirits in @{[ scalar @slots ]} table slots, and @{[ scalar @unique ]} spots to put one on: every
// vanilla spirit spot, every entrance spawn position (where Brian appears
// when he walks into a submap), and @{[ scalar @made ]} taken off the lines between
// them - short ones, so the line is very unlikely to leave the floor.
#include "spirit_data.h"

namespace merrow::spirits {
    const std::vector<Slot> vanilla_slots = {
HEADER

printf("        { %2d, %2d, %d },\n", $_->{map}, $_->{submap}, $_->{count}) for @slots;

print <<'MIDDLE';
    };

    const std::vector<Spot> spots = {
MIDDLE

printf("        { %2d, %2d, %14.4ff, %14.4ff, %s },\n",
       $_->{map}, $_->{submap}, $_->{x}, $_->{z},
       $_->{vanilla} ? 'Kind::Spirit' : ($_->{made} ? 'Kind::Made' : 'Kind::Entrance')) for @unique;

print <<'FOOTER';
    };
}
FOOTER
