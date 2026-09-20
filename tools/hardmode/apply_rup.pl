#!/usr/bin/perl
# Applies a Ninja 2 (.rup) patch to a ROM. Used to produce quest64.hm.z64,
# the Hard Mode ROM that hardmode.toml recompiles the hack's payload from.
#
#   perl tools/hardmode/apply_rup.pl tools/hardmode/Quest64.HM.0.9.8.3A.rup quest64.us.z64 quest64.hm.z64
#
# Format (romhacking.net document 288): 0x800-byte text header, then commands:
# 0x01 opens a file (name, type, source/target size, MD5s, and an "A"ppend
# or "M"inify overflow block when the sizes differ), 0x02 is an XOR record
# (offset, length, bytes to XOR into the source), 0x00 ends. Lengths and
# offsets are little-endian with a leading byte count.
use strict;
use warnings;
use Digest::MD5 qw(md5_hex);

my ($patch_path, $rom_path, $out_path) = @ARGV;
die "usage: apply_rup.pl patch.rup in.z64 out.z64\n" unless defined $out_path;

my ($patch, $rom);
{
    local $/;
    open my $fh, '<:raw', $patch_path or die "$patch_path: $!";
    $patch = <$fh>;
    open $fh, '<:raw', $rom_path or die "$rom_path: $!";
    $rom = <$fh>;
}
die "$patch_path is not a NINJA2 patch\n" unless substr($patch, 0, 6) eq 'NINJA2';

my $pos = 0x800;
sub vlv {
    my $n = ord(substr($patch, $pos++, 1));
    my $v = 0;
    for my $i (0 .. $n - 1) {
        $v |= ord(substr($patch, $pos++, 1)) << (8 * $i);
    }
    return $v;
}

my $out = $rom;
my ($source_md5, $target_md5);
while ($pos < length $patch) {
    my $cmd = ord(substr($patch, $pos++, 1));
    if ($cmd == 1) {
        my $name_len = vlv();
        $pos += $name_len;
        $pos += 1;    # ROM type
        my $source_size = vlv();
        my $target_size = vlv();
        $source_md5 = unpack('H32', substr($patch, $pos, 16)); $pos += 16;
        $target_md5 = unpack('H32', substr($patch, $pos, 16)); $pos += 16;
        if ($source_size != $target_size) {
            my $mode = substr($patch, $pos++, 1);
            my $len = vlv();
            my $overflow = substr($patch, $pos, $len);
            $pos += $len;
            if ($mode eq 'A') {
                $out .= $overflow ^ ("\xff" x length $overflow);
            } else {
                $out = substr($out, 0, $target_size);
            }
        }
    } elsif ($cmd == 2) {
        my $offset = vlv();
        my $len = vlv();
        my $xor = substr($patch, $pos, $len);
        $pos += $len;
        my $src = substr($rom, $offset, $len);
        $src .= "\0" x ($len - length $src);
        substr($out, $offset, $len) = $src ^ $xor;
    } elsif ($cmd == 0) {
        last;
    } else {
        die sprintf("unknown RUP command %d at 0x%X\n", $cmd, $pos - 1);
    }
}

die sprintf("source ROM MD5 %s does not match the patch's %s\n", md5_hex($rom), $source_md5)
    if $source_md5 && md5_hex($rom) ne $source_md5;
die sprintf("patched ROM MD5 %s does not match the patch's %s\n", md5_hex($out), $target_md5)
    if $target_md5 && md5_hex($out) ne $target_md5;

open my $fh, '>:raw', $out_path or die "$out_path: $!";
print $fh $out;
close $fh;
printf "wrote %s (%d bytes, md5 %s)\n", $out_path, length $out, md5_hex($out);
