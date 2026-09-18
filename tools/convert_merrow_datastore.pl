#!/usr/bin/perl
# Converts Merrow's DataStore.cs (https://github.com/hangedmandesign/merrow)
# into src/game/randomizer/merrow_data.cpp. Only the plain arrays are taken;
# the encounter/pack objects used by enemy-table shuffling are skipped.
#
#   perl tools/convert_merrow_datastore.pl DataStore.cs > src/game/randomizer/merrow_data.cpp
use strict; use warnings;
my $src = shift or die "usage: $0 DataStore.cs\n";
open my $in, '<', $src or die $!;
my @lines = <$in>; close $in;
print "// Generated from Merrow's DataStore.cs by tools/convert_merrow_datastore.pl.\n";
print "// Merrow is copyright (c) 2021 Jonah Davidson (Hangedman), MIT licence.\n";
print "// Do not edit by hand.\n#include \"merrow_data.h\"\n\nnamespace merrow::data {\n";
my $i = 0;
my %count;
while ($i < @lines) {
    my $l = $lines[$i];
    if ($l =~ /^\s*public\s+(?:static\s+)?(string|int|double)(\[\]|\[,\])\s+(\w+)\s*=\s*\{(.*)$/) {
        my ($type, $dims, $name, $rest) = ($1, $2, $3, $4);
        my $ctype = $type eq 'string' ? 'std::string' : $type;
        my $decl = $dims eq '[,]' ? "std::vector<std::vector<$ctype>>" : "std::vector<$ctype>";
        my @body = ("$rest\n");
        # C# allows a single-line "{ ... };" too.
        unless ($rest =~ /\};\s*(\/\/.*)?$/) {
            while (++$i < @lines) {
                push @body, $lines[$i];
                last if $lines[$i] =~ /^\s*\};/;
            }
        }
        my $text = join('', @body);
        # C# reads 08 as decimal; C++ would treat a leading zero as octal.
        $text =~ s/(?<![\w.])0+(\d+)(?![\w.])/$1/g if $type ne "string";
        $text =~ s/\};\s*(\/\/[^\n]*)?\s*$//s;
        # C# float/double suffixes (1.5f, 38d) are not valid C++.
        $text =~ s/(\d)[fd]\b/$1/g if $type eq 'double';
        print "const $decl $name = {$text};\n\n";
        $count{$decl}++;
    }
    elsif ($l =~ /^\s*public\s+(?:static\s+)?string\s+(\w+)\s*=\s*("[^"]*");/) {
        print "const std::string $1 = $2;\n\n";
        $count{'std::string'}++;
    }
    $i++;
}
print "}\n";
print STDERR "$_: $count{$_}\n" for sort keys %count;
