#!/usr/bin/perl
# The Repel item's two 16x16 icons, from tools/repel/repel_icon.png:
#   - the bag icon, on the item palette (ROM 0xD3BE40), the way the item
#     menu draws every item (pageitem.cpp does the same for the Torn Page);
#   - the icon over Brian's head, on the status-icon palette (ROM 0xD80230,
#     RAM 0x803A2960), which func_8001FEEC has loaded into TMEM by the time
#     repel.cpp draws it.
# Both are CI8 with index 0 transparent. Writes src/game/repel_icon_data.cpp
# and, for a look at the result, tools/repel/preview_*.png at 8x.
#
#   perl tools/repel/repel_icon.pl
use strict;
use warnings;
use Compress::Zlib;
use FindBin;

my $root = "$FindBin::Bin/../..";
my $png  = "$FindBin::Bin/repel_icon.png";
my $rom  = "$root/quest64.us.z64";
my $out  = "$root/src/game/repel_icon_data.cpp";
my $size = 16;

# ---- PNG in: 8-bit RGBA or RGB, not interlaced.
my ($w, $h, @px) = read_png($png);

sub read_png {
    my ($path) = @_;
    open my $f, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $data = <$f>;
    die "$path: not a PNG\n" unless substr($data, 0, 8) eq "\x89PNG\r\n\x1a\n";
    my ($pos, $idat, $w, $h, $depth, $type) = (8, '');
    while ($pos < length $data) {
        my ($len, $kind) = unpack('N a4', substr($data, $pos, 8));
        my $body = substr($data, $pos + 8, $len);
        if ($kind eq 'IHDR') {
            ($w, $h, $depth, $type, undef, undef, my $interlace) = unpack('N N C C C C C', $body);
            die "$path: need 8-bit RGB or RGBA, not interlaced\n"
                unless $depth == 8 && ($type == 6 || $type == 2) && !$interlace;
        }
        $idat .= $body if $kind eq 'IDAT';
        $pos += 12 + $len;
    }
    my $raw = uncompress($idat) // die "$path: bad image data\n";
    my $bpp = $type == 6 ? 4 : 3;
    my $stride = $w * $bpp;
    my @prev = (0) x $stride;
    my @px;
    for my $y (0 .. $h - 1) {
        my $filter = ord substr($raw, $y * ($stride + 1), 1);
        my @line = unpack('C*', substr($raw, $y * ($stride + 1) + 1, $stride));
        for my $i (0 .. $stride - 1) {
            my $a = $i >= $bpp ? $line[$i - $bpp] : 0;
            my $b = $prev[$i];
            my $c = $i >= $bpp ? $prev[$i - $bpp] : 0;
            my $p = $filter == 0 ? 0
                  : $filter == 1 ? $a
                  : $filter == 2 ? $b
                  : $filter == 3 ? int(($a + $b) / 2)
                  : paeth($a, $b, $c);
            $line[$i] = ($line[$i] + $p) & 0xFF;
        }
        @prev = @line;
        for my $x (0 .. $w - 1) {
            my @c = @line[$x * $bpp .. $x * $bpp + $bpp - 1];
            push @c, 255 if $bpp == 3;
            push @px, [@c];
        }
    }
    return ($w, $h, @px);
}

sub paeth {
    my ($a, $b, $c) = @_;
    my $p = $a + $b - $c;
    my ($pa, $pb, $pc) = (abs($p - $a), abs($p - $b), abs($p - $c));
    return $pa <= $pb && $pa <= $pc ? $a : $pb <= $pc ? $b : $c;
}

# ---- Down to 16x16: each target pixel averages the source pixels under it,
# colour weighted by alpha, so the transparent surround does not darken the
# edges.
my @small;
for my $ty (0 .. $size - 1) {
    for my $tx (0 .. $size - 1) {
        my ($x0, $x1) = ($tx * $w / $size, ($tx + 1) * $w / $size);
        my ($y0, $y1) = ($ty * $h / $size, ($ty + 1) * $h / $size);
        my ($r, $g, $b, $a, $n) = (0, 0, 0, 0, 0);
        for my $y (int($y0) .. int($y1 - 1e-9)) {
            for my $x (int($x0) .. int($x1 - 1e-9)) {
                my $p = $px[$y * $w + $x];
                $r += $p->[0] * $p->[3];
                $g += $p->[1] * $p->[3];
                $b += $p->[2] * $p->[3];
                $a += $p->[3];
                $n++;
            }
        }
        push @small, $a == 0 ? [0, 0, 0, 0] : [$r / $a, $g / $a, $b / $a, $a / $n];
    }
}

# ---- Palettes: 256 RGBA5551 entries; index 0 is the transparent one.
open my $romf, '<:raw', $rom or die "$rom: $!\n";
sub palette {
    my ($offset) = @_;
    seek $romf, $offset, 0;
    read $romf, my $bytes, 0x200;
    return [ map { [ (($_ >> 11) & 31) * 255 / 31, (($_ >> 6) & 31) * 255 / 31,
                     (($_ >> 1) & 31) * 255 / 31, $_ & 1 ] } unpack('n*', $bytes) ];
}

sub quantize {
    my ($pal) = @_;
    my @out;
    for my $p (@small) {
        if ($p->[3] < 128) { push @out, 0; next }
        my ($best, $dist) = (0, 1e18);
        for my $i (1 .. 255) {
            my $c = $pal->[$i];
            next unless $c->[3];
            # Weighted the way the eye weights them.
            my $d = 2 * ($c->[0] - $p->[0]) ** 2 + 4 * ($c->[1] - $p->[1]) ** 2 + 3 * ($c->[2] - $p->[2]) ** 2;
            ($best, $dist) = ($i, $d) if $d < $dist;
        }
        push @out, $best;
    }
    return \@out;
}

my $item_pal   = palette(0xD3BE40);
my $status_pal = palette(0xD80230);
my $status = quantize($status_pal);

# ---- The bag icon is drawn here, not taken from the picture: a spray (an
# atomizer: glass bottle, gold cap and nozzle, red squeeze bulb, a puff of
# mist) in the item icons' own style - a black outline, a few shades per
# material and a bright highlight, like the potions and the bell.
my @spray = (
    '................',
    '................',
    '...........KKKK.',
    '.M....KKKK.KrppK',
    'M.KKKKgyyGKKpppK',
    'MMKyggggGGPPpPPK',
    'M.KKKKGGGGKKPPPK',
    '......KwwK..KKK.',
    '.....KKwwKK.....',
    '....KwWwwvvK....',
    '...KwWllvvvVK...',
    '...KWlvvvvvVK...',
    '...KlvvvvvvVK...',
    '...KlvvvvvVVK...',
    '....KvvvvVVK....',
    '.....KKKKKK.....',
);
my %spray_colour = (
    K => [16, 16, 16],                                   # outline
    G => [140, 100, 30], g => [208, 168, 64], y => [248, 232, 140],   # gold
    P => [120, 24, 40], p => [200, 56, 80], r => [248, 150, 160],     # bulb
    w => [200, 224, 240], W => [255, 255, 255],          # glass
    V => [24, 96, 48], v => [56, 168, 88], l => [150, 224, 160],      # liquid
    M => [224, 236, 248],                                # mist
);
die "the spray must be 16x16\n" if @spray != $size || grep { length != $size } @spray;
my @small_saved = @small;
@small = map { my $c = $_; $c eq '.' ? [0, 0, 0, 0] : [ @{ $spray_colour{$c} // die "no colour for $c\n" }, 255 ] }
         map { split //, $_ } @spray;
my $bag = quantize($item_pal);
@small = @small_saved;

# ---- Out.
sub table {
    my ($indices) = @_;
    my $s = '';
    for my $row (0 .. $size - 1) {
        $s .= '    ' . join(', ', map { sprintf '0x%02X', $_ } @{$indices}[$row * $size .. $row * $size + $size - 1]) . ",\n";
    }
    return $s;
}

open my $o, '>', $out or die "$out: $!\n";
print $o <<"CPP";
// GENERATED by tools/repel/repel_icon.pl from tools/repel/repel_icon.png.
// Do not edit; change the picture and rerun the script.
#include "repel.h"

namespace zelda64::repel {
    // The bag icon, CI8 on the item palette (ROM 0xD3BE40): a spray.
    const std::array<uint8_t, 0x100> bag_icon = {
@{[ table($bag) ]}    };

    // The icon over Brian's head, CI8 on the status-icon palette (RAM
    // 0x803A2960).
    const std::array<uint8_t, 0x100> head_icon = {
@{[ table($status) ]}    };
}
CPP
close $o;
print "wrote $out\n";

# ---- Previews at 8x, so the result can be looked at.
for my $which ([ 'bag', $bag, $item_pal ], [ 'head', $status, $status_pal ]) {
    my ($name, $idx, $pal) = @$which;
    my $scale = 8;
    my $raw = '';
    for my $y (0 .. $size * $scale - 1) {
        $raw .= "\0";
        for my $x (0 .. $size * $scale - 1) {
            my $i = $idx->[int($y / $scale) * $size + int($x / $scale)];
            my $c = $pal->[$i];
            $raw .= $i == 0 ? pack('C4', 64, 64, 64, 255) : pack('C4', map { int($_ + 0.5) } @{$c}[0 .. 2], 255);
        }
    }
    my $file = "$FindBin::Bin/preview_$name.png";
    open my $p, '>:raw', $file or die "$file: $!\n";
    my $chunk = sub { my ($k, $d) = @_; pack('N', length $d) . $k . $d . pack('N', crc32($k . $d)) };
    print $p "\x89PNG\r\n\x1a\n",
        $chunk->('IHDR', pack('N N C C C C C', $size * $scale, $size * $scale, 8, 6, 0, 0, 0)),
        $chunk->('IDAT', compress($raw)), $chunk->('IEND', '');
    close $p;
}
