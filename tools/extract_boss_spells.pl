#!/usr/bin/perl
# Writes src/game/randomizer/boss_spells.cpp: the seventeen spells the bosses
# cast, read straight out of the US ROM.
#
# The tables come from Merrow PR #6 (vbhayden, "Data Refactor + Boss Spells",
# github.com/hangedmandesign/merrow/pull/6), which is where the idea of giving
# the bosses' spells to Brian comes from. Its own copies of the data were used
# only to find the addresses: the spell records in the PR match the ROM
# exactly, but thirteen of its seventy-seven animation records do not (byte 21
# of Zelse's Wind Zipper, one of the nine spells we use, is 0A in the ROM and
# A0 there), so every byte below is taken from the ROM instead.
#
#   perl tools/extract_boss_spells.pl quest64.us.z64 > src/game/randomizer/boss_spells.cpp
use strict; use warnings;

use constant SPELL_START  => 0xD4BA60;  # first player spell record
use constant SPELL_SIZE   => 0x44;
use constant ANIM_START   => 0xD4D338;  # first player spell animation record
use constant ANIM_SIZE    => 0x48;
use constant FIRST_BOSS   => 60;        # spell index of Solvaring's first

# name, the ROM's own label, and whether Brian can cast it anywhere. The
# unsafe ones are the PR's: they are the spells whose effects only make sense
# from a boss (BossSpellsCastableAnywhere there leaves them commented out).
my @boss = (
    [ "Solvaring's Fire Blast",   'Sol_Panch', 0 ],
    [ "Solvaring's Earth Spikes", 'Sol_Toge',  0 ],
    [ "Zelse's Wind Razor",       'Zel_Cat',   1 ],
    [ "Zelse's Wind Zipper",      'Zel_Beam',  1 ],
    [ "Nepty's Bubble Shot",      'Npt_Ball',  1 ],
    [ "Nepty's Bubble Shield",    'Npt_Turn',  0 ],
    [ "Fargo's Lava Ball",        'Fag_Ball',  1 ],
    [ "Fargo's Explosion",        'Fag_Bom',   1 ],
    [ "Shilf's Laser",            'Sil_Laser', 0 ],
    [ "Shilf's Dove Razor",       'Sil_Cat',   1 ],
    [ "Guilty's Ground Pound",    'Gil_Punch', 0 ],
    [ "Guilty's Slash",           'Gil_Crow',  0 ],
    [ "Beigis' Spirit Sword",     'Ges_Cat',   1 ],
    [ "Beigis' Green Laser",      'Ges_Beam',  0 ],
    [ "Mammon's Solar Blast",     'On_Light',  0 ],
    [ "Mammon's Flame Waves",     'On_Wave',   1 ],
    [ "Mammon's Fire Arrows",     'On_Needle', 1 ],
);

my $rom = shift or die "usage: $0 quest64.us.z64 > boss_spells.cpp\n";
open my $in, '<:raw', $rom or die "$rom: $!\n";

sub at {
    my ($offset, $length) = @_;
    seek $in, $offset, 0 or die $!;
    read($in, my $buf, $length) == $length or die "short read at $offset\n";
    return uc unpack 'H*', $buf;
}

print <<'HEADER';
// Generated from the US ROM by tools/extract_boss_spells.pl. Do not edit by
// hand. See that script for where the addresses came from.
#include "boss_spells.h"

namespace merrow::data {
    // Merrow's own categories cover status and offence (statusspells and
    // offenseSpells in merrow_data.cpp); these two are the split of the
    // status spells into helpful and harmful that PR #6 adds.
    const std::vector<int> buffSpells = { 17, 22, 24, 44, 48, 54, 57 };
    const std::vector<int> debuffSpells = { 6, 19, 25, 29, 43, 47, 49, 52, 56, 58 };

    const std::vector<BossSpell> bossSpells = {
HEADER

for my $i (0 .. $#boss) {
    my ($name, $label, $castable) = @{ $boss[$i] };
    my $index = FIRST_BOSS + $i;
    my $data_addr = SPELL_START + $index * SPELL_SIZE;
    my $anim_addr = ANIM_START + $index * ANIM_SIZE;
    printf "        { \"%s\", \"%s\", 0x%06X, 0x%06X,\n", $name, $label, $data_addr, $anim_addr;
    printf "          \"%s\",\n", at($data_addr, SPELL_SIZE);
    printf "          \"%s\", %s },\n", at($anim_addr, ANIM_SIZE), $castable ? 'true' : 'false';
}

print <<'FOOTER';
    };
}
FOOTER
