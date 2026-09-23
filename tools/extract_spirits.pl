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
# A third pool of points interpolated between these was tried on 2026-09-22
# and taken out again the same day: the equivalent for chests put one inside a
# fence in Melrode, because a line between two walkable points can cross one.
# Only positions the game itself uses are offered now.
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


# A spot is no use twice, and an entrance that sits on a spirit is the spirit.
my (%seen, @unique);
for my $s (sort { $b->{vanilla} <=> $a->{vanilla} } @spots) {
    my $key = sprintf "%d.%d.%.3f.%.3f", $s->{map}, $s->{submap}, $s->{x}, $s->{z};
    next if $seen{$key}++;
    push @unique, $s;
}
@unique = sort { $a->{map} <=> $b->{map} || $a->{submap} <=> $b->{submap} || $a->{x} <=> $b->{x} || $a->{z} <=> $b->{z} } @unique;

printf STDERR "%d spirits in %d slots, %d entrance points, %d unique spots\n",
    $id, scalar @slots, $entrances, scalar @unique;

print <<"HEADER";
// Generated from the US ROM by tools/extract_spirits.pl. Do not edit by hand.
// See that script for the tables these came out of.
//
// $id spirits in @{[ scalar @slots ]} table slots, and @{[ scalar @unique ]} spots to put one on: every
// vanilla spirit spot and every entrance spawn position (where Brian appears
// when he walks into a submap). Both are positions the game itself uses, so
// a spirit is always somewhere Brian can reach.
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
       $_->{vanilla} ? "Kind::Spirit" : "Kind::Entrance") for @unique;

print <<'FOOTER';
    };
}
FOOTER
