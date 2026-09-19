#!/usr/bin/perl
# Converts the encounter/pack objects in Merrow's DataStore.cs
# (https://github.com/hangedmandesign/merrow) into
# src/game/randomizer/merrow_mapdata.cpp. These are the structures
# tools/convert_merrow_datastore.pl deliberately skips, because they are nested
# C# object literals rather than plain arrays.
#
#   perl tools/convert_merrow_mapdata.pl DataStore.cs > src/game/randomizer/merrow_mapdata.cpp
#
# Packs and regions are emitted as flat lists with each area holding a range
# into them. Areas that share a pack array in Merrow (Brannoch Castle, Mammon's
# World, Baragoon Moor) get their own copy here, which is what Merrow's
# AreaEncounterData.Copy() does at runtime anyway.
use strict;
use warnings;

my $src = shift or die "usage: $0 DataStore.cs\n";
open my $in, '<', $src or die "$src: $!";
my $text = do { local $/; <$in> };
close $in;

# Pull the body of "<decl> = ... {" up to the matching "};" at the same indent.
sub block_after {
    my ($pattern) = @_;
    return undef unless $text =~ /$pattern/;
    my $start = $+[0];
    my $depth = 1;
    my $i = $start;
    while ($i < length($text) && $depth > 0) {
        my $c = substr($text, $i, 1);
        $depth++ if $c eq '{';
        $depth-- if $c eq '}';
        $i++;
    }
    return substr($text, $start, $i - $start - 1);
}

sub num { my $v = shift; $v =~ s/\s+//g; return $v =~ /^0[xX]/ ? hex($v) : $v + 0; }

# --- monsterTables ---------------------------------------------------------
my $tables_body = block_after(qr/EnemyTable\[\]\s+monsterTables\s*=\s*new\s+Util\.EnemyTable\[\]\s*\{/)
    or die "monsterTables not found\n";
my @tables;
while ($tables_body =~ /EnemyTable\(\s*(0[xX][0-9A-Fa-f]+)\s*,\s*new\s+Util\.EnemyEntry\[\]\s*\{(.*?)\}\s*\)/gs) {
    my ($addr, $entries) = ($1, $2);
    my @names;
    while ($entries =~ /EnemyEntry\(\s*0[xX][0-9A-Fa-f]+\s*,\s*"([^"]*)"\s*\)/g) {
        push @names, $1;
    }
    push @tables, { addr => num($addr), names => \@names };
}
die "no enemy tables parsed\n" unless @tables;

# --- allAreaMapData --------------------------------------------------------
my $maps_body = block_after(qr/AreaMapData\[\]\s+allAreaMapData\s*=\s*\{/)
    or die "allAreaMapData not found\n";
my @maps;
while ($maps_body =~ /AreaMapData\(\s*([^,]+),\s*"([^"]*)"\s*,\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,\)]+)\)/g) {
    push @maps, {
        rom => num($1), name => $2, unk0 => num($3), ptr => num($4),
        doors => num($5), unk8 => num($6), unk10 => num($7),
        table => num($8), unk14 => num($9),
    };
}
die "expected 27 area maps, got " . scalar(@maps) . "\n" unless @maps == 27;

# --- packs / regions -------------------------------------------------------
my %packs;
# Collect the names in one pass first: block_after() runs its own match against
# $text, which resets the position of an enclosing //g loop and would silently
# skip entries.
my @pack_names = ($text =~ /MonsterPackDefinition\[\]\s+(\w+)\s*=\s*new\s+MonsterPackDefinition\[\]\s*\{/g);
for my $name (@pack_names) {
    my $body = block_after(qr/MonsterPackDefinition\[\]\s+\Q$name\E\s*=\s*new\s+MonsterPackDefinition\[\]\s*\{/);
    my @defs;
    while ($body =~ /MonsterPackDefinition\(\s*(0[xX][0-9A-Fa-f]+)\s*,\s*new\s+EnemyPackMember\[\]\s*\{(.*?)\}\s*\)/gs) {
        my ($addr, $members) = ($1, $2);
        my @m;
        while ($members =~ /EnemyPackMember\(\s*([^,]+),\s*([^,]+),\s*([^,\)]+)\)/g) {
            push @m, [ num($1), num($2), num($3) ];
        }
        push @defs, { addr => num($addr), members => \@m };
    }
    $packs{$name} = \@defs;
}

my %regions;
my @region_names = ($text =~ /EncounterRegion\[\]\s+(\w+)\s*=\s*new\s+EncounterRegion\[\]\s*\{/g);
for my $name (@region_names) {
    my $body = block_after(qr/EncounterRegion\[\]\s+\Q$name\E\s*=\s*new\s+EncounterRegion\[\]\s*\{/);
    my @defs;
    while ($body =~ /EncounterRegion\(([^\)]*)\)/g) {
        my @v = map { num($_) } split /\s*,\s*/, $1;
        die "region in $name has " . scalar(@v) . " fields\n" unless @v >= 13;
        # address, xStart, xEnd, zStart, zEnd, presetCount, then 7 preset indices
        push @defs, { addr => $v[0], count => $v[5], presets => [ @v[6 .. 12] ] };
    }
    $regions{$name} = \@defs;
}

# --- area list, in GetMapData order ----------------------------------------
my %decl;
while ($text =~ /AreaEncounterData\s+(\w+)\s*=\s*new\s+Util\.AreaEncounterData\("([^"]*)"\s*,\s*allAreaMapData\[(\d+)\]\s*,\s*(\w+)\s*,\s*(\w+)\s*,/g) {
    $decl{$1} = { name => $2, map => $3 + 0, packs => $4, regions => $5 };
}
my $order_body = block_after(qr/return new FullGameMapData\(new AreaEncounterData\[\]\s*\{/)
    or die "GetMapData not found\n";
my @order;
while ($order_body =~ /(\w+)\.Copy\(\)/g) { push @order, $1; }
die "expected 27 areas in GetMapData, got " . scalar(@order) . "\n" unless @order == 27;

# --- emit ------------------------------------------------------------------
print "// Generated from Merrow's DataStore.cs by tools/convert_merrow_mapdata.pl.\n";
print "// Merrow is copyright (c) 2021 Jonah Davidson (Hangedman), MIT licence.\n";
print "// Do not edit by hand.\n";
print "#include \"merrow_mapdata.h\"\n\nnamespace merrow::mapdata {\n";

print "    const std::vector<EnemyTable> monster_tables = {\n";
for my $t (@tables) {
    printf "        { 0x%06X, { %s } },\n", $t->{addr}, join(", ", map { "\"$_\"" } @{ $t->{names} });
}
print "    };\n\n";

my (@flat_packs, @flat_regions, @areas);
for my $key (@order) {
    my $d = $decl{$key} or die "no declaration for $key\n";
    my $p = $packs{ $d->{packs} } or die "no packs named $d->{packs}\n";
    my $r = $regions{ $d->{regions} } or die "no regions named $d->{regions}\n";
    my $pack_start = scalar @flat_packs;
    push @flat_packs, @$p;
    my $region_start = scalar @flat_regions;
    push @flat_regions, @$r;
    push @areas, {
        name => $d->{name}, map => $maps[ $d->{map} ],
        ps => $pack_start, pc => scalar @$p,
        rs => $region_start, rc => scalar @$r,
    };
}

print "    const std::vector<MonsterPack> packs = {\n";
for my $p (@flat_packs) {
    my $members = join ", ", map { sprintf "{ %d, %d, %d }", @$_ } @{ $p->{members} };
    printf "        { 0x%06X, { %s } },\n", $p->{addr}, $members;
}
print "    };\n\n";

print "    const std::vector<Region> regions = {\n";
for my $r (@flat_regions) {
    printf "        { 0x%06X, %d, { %s } },\n", $r->{addr}, $r->{count}, join(", ", @{ $r->{presets} });
}
print "    };\n\n";

print "    const std::vector<Area> areas = {\n";
for my $a (@areas) {
    my $m = $a->{map};
    printf "        { \"%s\", { 0x%06X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%04X, 0x%04X, 0x%08X }, %d, %d, %d, %d },\n",
        $a->{name}, $m->{rom}, $m->{unk0}, $m->{ptr}, $m->{doors}, $m->{unk8},
        $m->{unk10}, $m->{table}, $m->{unk14}, $a->{ps}, $a->{pc}, $a->{rs}, $a->{rc};
}
print "    };\n}\n";

printf STDERR "tables=%d packs=%d regions=%d areas=%d\n",
    scalar @tables, scalar @flat_packs, scalar @flat_regions, scalar @areas;
