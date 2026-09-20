#!/usr/bin/perl
# Regenerates src/game/hardmode/recompiled/ from the Hard Mode patch. Run
# from the repository root after updating tools/hardmode/Quest64.HM.*.rup
# (and payload_funcs.txt, if the payload's layout changed):
#
#   perl tools/hardmode/recompile_payload.pl
#
# Steps: apply the .rup to quest64.us.z64 to get quest64.hm.z64, generate the
# symbols file and N64Recomp config, run N64Recomp, then keep only the two
# files the build uses (payload.c and funcs.h).
use strict;
use warnings;
use FindBin;
use File::Copy qw(move);

my $root = "$FindBin::Bin/../..";
chdir $root or die "chdir $root: $!";

my @rups = sort glob("tools/hardmode/Quest64.HM.*.rup");
die "no tools/hardmode/Quest64.HM.*.rup found\n" unless @rups;
my $rup = $rups[-1];
print "patch: $rup\n";

system("perl", "tools/hardmode/apply_rup.pl", $rup, "quest64.us.z64", "quest64.hm.z64") == 0 or die "apply_rup failed\n";
system("perl", "tools/hardmode/gen_syms.pl") == 0 or die "gen_syms failed\n";

my $out = "src/game/hardmode/recompiled";
unlink glob("$out/*");
system("./N64Recomp.exe", "hardmode.toml") == 0 or die "N64Recomp failed\n";

my @c = glob("$out/funcs_*.c");
die "expected exactly one funcs_N.c, got @c\n" unless @c == 1;
move($c[0], "$out/payload.c") or die "move: $!";
unlink "$out/lookup.cpp", "$out/recomp_overlays.inl";
print "wrote $out/payload.c and $out/funcs.h\n";
