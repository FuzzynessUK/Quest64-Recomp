#!/usr/bin/perl
# Converts a standard MIDI file (type 0 or 1) into the compact sequence
# format Quest 64's libultra sequence player (alCSPlayer) reads, for the
# Audio tab's Custom Music option.
#
#   perl tools/mid2cseq.pl in.mid out.seq [--prog 0=21,1=23,2=0]
#
# --prog pins a MIDI channel to a bank program, overriding the family map.
#
# The format, as read off the game's own parser (func_800386D0, the block
# reader, and func_800387F4, the event reader):
#   header: u32 trackOffset[16], u32 division (the game's files use 480)
#   track:  delta (MIDI variable-length) then an event:
#             9n key vel <duration VLQ>     a note; there are no note-offs
#             Bn cc vv / Cn pp / En lo hi   as MIDI (running status allowed,
#                                           but cleared by any meta)
#             FF 51 t t t                   tempo, three raw bytes
#             FF 2E 00 FF                   loop start (the game's files
#                                           always carry 00 FF)
#             FF 2D count remaining o o o o loop end: `remaining` counts
#                                           down in place, 0xFF loops
#                                           forever; the u32 is how far back
#                                           from the end of this payload the
#                                           loop resumes
#             FF 2F                         end of track
#   A 0xFE byte is a back-reference marker (FE hi lo len, measured from
#   the marker byte) and a literal 0xFE must be written FE FE; the
#   loop-end payload is read raw, so it must not contain one. The player
#   acts on controllers 7 (volume), 10 (pan), 91 (reverb) and 64 (sustain);
#   the rest are dropped rather than trusted to be ignored.
#
# Instruments: the game's music bank has 29 programs and no separate
# percussion set. Program 9 is a drum kit on GM key numbers; the others
# are mapped from General MIDI families by --map gm (the default), which
# is a rough guess by ear-less inspection of the bank - edit %gm_family
# to taste or pin channels with --prog. MIDI channel 10 always goes to the
# kit. For reference the battle theme (sequence 13) uses programs 21 (lead),
# 0, 18, 25, 23, 19, 7 and 9 (drums); the field theme 4, 5, 0, 6, 7, 9.
use strict;
use warnings;

my ($in, $out, @rest) = @ARGV;
die "usage: mid2cseq.pl in.mid out.seq [--prog ch=prog,...]\n" unless defined $out;
my %pinned;
while (@rest) {
    my $a = shift @rest;
    if ($a eq '--prog') {
        my $v = shift @rest // '';
        for my $pair (split /,/, $v) {
            my ($ch, $prog) = split /=/, $pair;
            $pinned{$ch} = $prog;
        }
    }
    else { die "unknown option $a\n" }
}

local $/;
open my $f, '<:raw', $in or die "$in: $!";
my $midi = <$f>;
close $f;

# ---------------------------------------------------------------- MIDI in
sub read_vlq {
    my ($d, $p) = @_;
    my $v = 0;
    while (1) {
        my $b = ord(substr($$d, $$p++, 1));
        $v = ($v << 7) | ($b & 0x7F);
        last unless $b & 0x80;
    }
    return $v;
}

substr($midi, 0, 4) eq 'MThd' or die "not a MIDI file";
my ($hlen, $format, $ntracks, $division) = unpack('NnnN', substr($midi, 4, 10));
$division = unpack('n', substr($midi, 12, 2));
die "SMPTE time division not supported" if $division & 0x8000;
my $pos = 8 + $hlen;

my @events;      # { t, ch, type, a, b } absolute MIDI ticks
my @tempos;      # { t, usec }
for my $tr (0 .. $ntracks - 1) {
    substr($midi, $pos, 4) eq 'MTrk' or die "track $tr: no MTrk";
    my $len = unpack('N', substr($midi, $pos + 4, 4));
    my $data = substr($midi, $pos + 8, $len);
    $pos += 8 + $len;
    my ($p, $t, $status) = (0, 0, 0);
    while ($p < length $data) {
        $t += read_vlq(\$data, \$p);
        my $b = ord(substr($data, $p, 1));
        if ($b == 0xFF) {
            my $type = ord(substr($data, $p + 1, 1));
            $p += 2;
            my $l = read_vlq(\$data, \$p);
            my $payload = substr($data, $p, $l);
            $p += $l;
            if ($type == 0x51) {
                my ($a, $c, $d) = unpack('CCC', $payload);
                push @tempos, { t => $t, usec => ($a << 16) | ($c << 8) | $d };
            }
            next;
        }
        if ($b == 0xF0 || $b == 0xF7) {
            $p++;
            my $l = read_vlq(\$data, \$p);
            $p += $l;
            next;
        }
        if ($b & 0x80) { $status = $b; $p++ }
        my $type = $status & 0xF0;
        my $ch = $status & 0x0F;
        if ($type == 0xC0 || $type == 0xD0) {
            my $a = ord(substr($data, $p++, 1));
            push @events, { t => $t, ch => $ch, type => $type, a => $a, b => 0 };
        }
        else {
            my $a = ord(substr($data, $p++, 1));
            my $bb = ord(substr($data, $p++, 1));
            $type = 0x80 if $type == 0x90 && $bb == 0;
            push @events, { t => $t, ch => $ch, type => $type, a => $a, b => $bb };
        }
    }
}
@events = sort { $a->{t} <=> $b->{t} } @events;
@tempos = sort { $a->{t} <=> $b->{t} } @tempos;

# ---------------------------------------------------------------- notes with durations
my $scale = 480 / $division;
my %open;        # "ch:key" => index into @notes
my @notes;       # { t, ch, key, vel, dur }
my @controls;    # { t, ch, type, a, b }
my $song_end = 0;
for my $e (@events) {
    my $t = int($e->{t} * $scale + 0.5);
    $song_end = $t if $t > $song_end;
    my $k = "$e->{ch}:$e->{a}";
    if ($e->{type} == 0x90) {
        if (exists $open{$k}) {
            $notes[$open{$k}]{dur} = $t - $notes[$open{$k}]{t};
        }
        push @notes, { t => $t, ch => $e->{ch}, key => $e->{a}, vel => $e->{b}, dur => 0 };
        $open{$k} = $#notes;
    }
    elsif ($e->{type} == 0x80) {
        if (exists $open{$k}) {
            $notes[$open{$k}]{dur} = $t - $notes[$open{$k}]{t};
            delete $open{$k};
        }
    }
    else {
        push @controls, { %$e, t => $t };
    }
}
for my $i (keys %open) { $notes[$open{$i}]{dur} = $song_end - $notes[$open{$i}]{t} }
$_->{dur} = 1 for grep { $_->{dur} < 1 } @notes;

# ---------------------------------------------------------------- programs
my %gm_family = (
    piano => 5, chromatic => 13, organ => 27, guitar => 26, bass => 26, strings => 0,
    ensemble => 0, brass => 11, reed => 6, pipe => 6, lead => 8, pad => 0, fx => 0,
    ethnic => 13, percussive => 9, effects => 9,
);
my @families = qw(piano chromatic organ guitar bass strings ensemble brass reed pipe lead pad fx ethnic percussive effects);
sub map_program {
    my ($ch, $gm) = @_;
    return $pinned{$ch} if exists $pinned{$ch};
    return 9 if $ch == 9;
    return $gm_family{ $families[ int($gm / 8) ] } // 5;
}

# ---------------------------------------------------------------- tracks out
sub vlq {
    my $v = shift;
    my @b = ($v & 0x7F);
    while ($v >>= 7) { unshift @b, ($v & 0x7F) | 0x80 }
    return pack('C*', @b);
}
# Escape literal 0xFE bytes in stream data the block reader sees.
sub esc { my $s = shift; $s =~ s/\xFE/\xFE\xFE/g; return $s }

my %used_ch;
$used_ch{ $_->{ch} }++ for @notes;
my @channels = sort { $a <=> $b } keys %used_ch;
die "more than 15 channels in use" if @channels > 15;

my @tracks;   # binary track data, index 0..15
my $loop_len = $song_end < 1 ? 1 : $song_end;

# One track per used channel: program and controls at their times, notes.
for my $ch (@channels) {
    my @items;
    my $program;
    for my $c (grep { $_->{ch} == $ch } @controls) {
        if ($c->{type} == 0xC0) { $program = $c->{a} if !defined $program && $c->{t} == 0; push @items, [ $c->{t}, 1, $c ] }
        elsif ($c->{type} == 0xE0) { push @items, [ $c->{t}, 1, $c ] }
        elsif ($c->{type} == 0xB0 && ($c->{a} == 7 || $c->{a} == 10 || $c->{a} == 91 || $c->{a} == 64)) { push @items, [ $c->{t}, 1, $c ] }
    }
    push @items, [ $_->{t}, 2, $_ ] for grep { $_->{ch} == $ch } @notes;
    @items = sort { $a->[0] <=> $b->[0] || $a->[1] <=> $b->[1] } @items;

    my $data = '';
    my $last = 0;
    # Program first, then the loop start so it is not repeated.
    $data .= vlq(0) . pack('CC', 0xC0 | $ch, map_program($ch, $program // 0));
    $data .= vlq(0) . "\xFF\x2E\x00\xFF";
    for my $it (@items) {
        my ($t, $kind, $e) = @$it;
        my $delta = $t - $last;
        $last = $t;
        if ($kind == 2) {
            $data .= vlq($delta) . pack('CCC', 0x90 | $ch, $e->{key}, $e->{vel}) . esc(vlq($e->{dur}));
        }
        elsif ($e->{type} == 0xC0) {
            $data .= vlq($delta) . pack('CC', 0xC0 | $ch, map_program($ch, $e->{a}));
        }
        else {
            $data .= vlq($delta) . pack('CCC', $e->{type} | $ch, $e->{a}, $e->{b});
        }
    }
    $tracks[$ch] = [ $data, $loop_len - $last ];
}
# The conductor track (16th, as the game's own files have): tempo, loop.
{
    my $data = '';
    my $last = 0;
    my $first = @tempos && $tempos[0]{t} == 0 ? shift @tempos : { t => 0, usec => 500000 };
    $data .= vlq(0) . esc("\xFF\x51" . pack('CCC', ($first->{usec} >> 16) & 0xFF, ($first->{usec} >> 8) & 0xFF, $first->{usec} & 0xFF));
    $data .= vlq(0) . "\xFF\x2E\x00\xFF";
    for my $tp (@tempos) {
        my $t = int($tp->{t} * $scale + 0.5);
        next if $t > $loop_len;
        $data .= vlq($t - $last) . esc("\xFF\x51" . pack('CCC', ($tp->{usec} >> 16) & 0xFF, ($tp->{usec} >> 8) & 0xFF, $tp->{usec} & 0xFF));
        $last = $t;
    }
    $tracks[15] = [ $data, $loop_len - $last ];
}

# Close every track: the delta to the loop point, a loop end pointing back
# to just after the loop start (count FF FF: forever), and end of track.
# The loop-end payload is read raw, so if it would hold a 0xFE the closing
# delta is padded by a byte (a VLQ may carry leading 0x80s) to move it on.
for my $i (0 .. 15) {
    next unless defined $tracks[$i];
    my ($data, $tail) = @{ $tracks[$i] };
    my $start = index($data, "\xFF\x2E\x00\xFF") + 4;
    my $delta = vlq($tail);
    while (1) {
        my $back = length($data) + length($delta) + 2 + 6 - $start;   # from after the 6-byte payload
        my $payload = pack('CCN', 0xFF, 0xFF, $back);
        if ($payload !~ /\xFE/) {
            $tracks[$i] = $data . $delta . "\xFF\x2D" . $payload . vlq(0) . "\xFF\x2F";
            last;
        }
        $delta = "\x80" . $delta;
    }
}

# ---------------------------------------------------------------- file
my $header_size = 17 * 4;
my $offset = $header_size;
my @offsets = (0) x 16;
my $body = '';
for my $i (0 .. 15) {
    next unless defined $tracks[$i];
    $offsets[$i] = $offset;
    $body .= $tracks[$i];
    $offset += length $tracks[$i];
}
my $file = pack('N16N', @offsets, 480) . $body;
open my $o, '>:raw', $out or die "$out: $!";
print $o $file;
close $o;
printf "%s: %d bytes, %d tracks (channels %s), %d notes, loop %d ticks at 480/quarter\n",
    $out, length $file, scalar(grep { defined } @tracks), join(',', @channels), scalar @notes, $loop_len;
