package Quest64Checks;
# Every Archipelago check Quest 64 can offer, read out of the Merrow tables
# this recomp ships and, for the spirits, the ROM itself. Shared by
# tools/archipelago_checks.pl (which writes DOCS/archipelago_checks.xlsx) and
# tools/archipelago_world.pl (which writes the .apworld), so a location's
# name is decided in exactly one place and the sheet and the world cannot
# drift apart. Names are the sheet's "Suggested name" column.
#
#   my $checks = Quest64Checks::gather($repo_root);
use strict;
use warnings;

# Helpers, at package level so item_name keeps seeing @items.
our @items;
sub table { my ($src, $name) = @_; my ($body) = ($src =~ /\b\Q$name\E = \{(.*?)\n\s*\};/s); die "no table $name" unless defined $body; return $body }
sub strings { my $b = shift; return ($b =~ /"([^"]*)"/g) }
sub numbers { my $b = shift; $b =~ s{//[^\n]*}{}g; return ($b =~ /(\d+)/g) }
sub item_name { my $id = shift; return 'nothing' if $id == 255; return $items[$id] // "item $id" }
sub nice { my $s = shift; $s =~ s/`/'/g; return join ' ', map { ucfirst lc } split / /, $s }

sub gather {
    my ($root) = @_;
    my ($data, $map, $prog, $rom);
    {
        local $/;
        open my $f, '<', "$root/src/game/randomizer/merrow_data.cpp" or die $!;
        $data = <$f>; close $f;
        open $f, '<', "$root/src/game/randomizer/merrow_mapdata.cpp" or die $!;
        $map = <$f>; close $f;
        open $f, '<', "$root/src/game/randomizer/enemy_progression_data.cpp" or die $!;
        $prog = <$f>; close $f;
        open $f, '<:raw', "$root/quest64.us.z64" or die "quest64.us.z64: $!";
        $rom = <$f>; close $f;
    }
    $_ =~ s/\r//g for ($data, $map, $prog);


    # ---------------------------------------------------------------- items
    my @item_raw = strings(table($data, 'items'));
    @items = ();
    for (my $i = 0; $i + 2 < @item_raw; $i += 3) { push @items, nice($item_raw[$i]) }

    # ---------------------------------------------------------------- chests
    # chestdata: ROM address of the chest record (item byte at +33), vanilla
    # item, Merrow's logical path order, "boss region" (how many bosses the
    # vanilla route has beaten by the time the chest is reachable).
    my @chest_names = strings(table($data, 'chestlocnames'));
    my @chest_vals  = numbers(table($data, 'chestdata'));
    die "chest tables disagree" unless @chest_vals == @chest_names * 4;
    my @stage = ('Start', 'After Solvaring (Earth Orb)', 'After Zelse (Wind Jade)', 'After Nepty (Water Jewel)',
                 'After Shilf', 'After Fargo (Fire Ruby)');
    my @chests;
    for my $i (0 .. $#chest_names) {
        my ($addr, $item, $order, $region) = @chest_vals[ $i * 4 .. $i * 4 + 3 ];
        (my $area = $chest_names[$i]) =~ s/ #\d+$//;
        push @chests, { idx => $i, name => $chest_names[$i], area => $area, item => item_name($item), item_id => $item,
                        order => $order, region => $region, addr => $addr };
    }

    # ---------------------------------------------------------------- givers
    my @gifter_names = strings(table($data, 'gifternames'));
    my @granters = numbers(table($data, 'itemgranters'));
    my @givers;
    for my $i (0 .. $#gifter_names) {
        my ($addr, $item) = @granters[ $i * 2, $i * 2 + 1 ];
        my ($who, $where) = ($gifter_names[$i] =~ /^(.*?) \((.*)\)$/);
        my $kind = $i >= 10 ? 'Wingsmith' : $i >= 8 ? 'Shannon (endgame)' : 'Gift NPC';
        push @givers, { idx => $i, who => $who, where => $where, kind => $kind, item => item_name($item), item_id => $item, addr => $addr };
    }

    # ---------------------------------------------------------------- monsters
    my @mon_raw = strings(table($data, 'monsternames'));
    my @monster_names;
    for (my $i = 0; $i + 1 < @mon_raw; $i += 2) { push @monster_names, nice($mon_raw[$i]) }
    my @stats = numbers(table($data, 'monsterstatvanilla'));
    my @drops = numbers(table($data, 'dropdata'));
    die "monster tables disagree" unless @monster_names == 75 && @stats >= 450 && @drops == 150;

    # Areas and which monsters each one rolls, from the encounter packs.
    my @tables;
    my $mt = table($map, 'monster_tables');
    while ($mt =~ /\{ 0x[0-9A-F]+, \{ (.*?) \} \}/g) { my @e = ($1 =~ /"([^"]*)"/g); push @tables, \@e }
    my @packs;
    my $pk = table($map, 'packs');
    while ($pk =~ /\{ 0x[0-9A-F]+, \{ (.*?) \} \},?\n/g) {
        my @mem; my $s = $1;
        while ($s =~ /\{ (\d+), (\d+), (\d+) \}/g) { push @mem, $1 }
        push @packs, \@mem;
    }
    my @areas;
    my $ar = table($map, 'areas');
    while ($ar =~ /\{ "([^"]+)", \{ (?:0x[0-9A-F]+, ){6}0x([0-9A-F]+), 0x[0-9A-F]+ \}, (\d+), (\d+), (\d+), (\d+) \}/g) {
        push @areas, { name => $1, table => hex($2), ps => $3, pc => $4 };
    }
    die "unexpected map data (@{[scalar @tables]} tables, @{[scalar @areas]} areas)" unless @tables == 6 && @areas == 27;
    my %id_of; $id_of{ uc $monster_names[$_] } = $_ for 0 .. $#monster_names;
    my %areas_of;   # monster id -> { area name => 1 }
    for my $a (@areas) {
        (my $base = $a->{name}) =~ s/ \d+$//;
        my $names = $tables[ $a->{table} ];
        for my $p ($a->{ps} .. $a->{ps} + $a->{pc} - 1) {
            for my $slot (@{ $packs[$p] }) {
                my $id = $id_of{ $names->[$slot] };
                $areas_of{$id}{$base} = 1 if defined $id;
            }
        }
    }
    # Story order of the areas, from the progression data.
    my %tier; my $n = 0;
    my $pa = table($prog, 'areas');
    while ($pa =~ /\{ "([^"]+)", (\d+), (\d+),/g) { $tier{$1} = { order => $n++, tier => $3, map => $2 } }
    my %prog_flags;
    my $pm = table($prog, 'monsters');
    while ($pm =~ /\{ "([^"]+)", (\d+), (true|false), (true|false),/g) { $prog_flags{ nice($1) } = { dangerous => $3, flying => $4 } }

    my @monsters;
    for my $i (0 .. 66) {
        my @s = @stats[ $i * 6 .. $i * 6 + 5 ];
        my @where = sort { ($tier{$a}{order} // 99) <=> ($tier{$b}{order} // 99) } keys %{ $areas_of{$i} || {} };
        my $f = $prog_flags{ $monster_names[$i] } || {};
        push @monsters, { id => $i, name => $monster_names[$i], areas => \@where,
                          first => $where[0] // '', tier => ($where[0] ? $tier{ $where[0] }{tier} : ''),
                          hp => $s[0], atk => $s[1], def => $s[2], agi => $s[3], exp => $s[4],
                          drop => item_name($drops[ $i * 2 + 1 ]), drop_id => $drops[ $i * 2 + 1 ], drop_addr => $drops[ $i * 2 ],
                          flying => ($f->{flying} // '') eq 'true' ? 'Y' : 'N', dangerous => ($f->{dangerous} // '') eq 'true' ? 'Y' : 'N' };
    }

    # ---------------------------------------------------------------- bosses
    # Ids 67-74 in the monster list; the vanilla order, arenas and rewards are
    # the game's (the reward ids match randomizer.cpp's vanilla_boss_items).
    my @boss_info = (
        [ 'Solvaring', 'Connor Forest (Solvaring\'s Lair)', 20, 'Earth Orb', 1 ],
        [ 'Zelse',     'Windward Forest',                    21, 'Wind Jade', 2 ],
        [ 'Nepty',     'Blue Cave',                          22, 'Water Jewel', 3 ],
        [ 'Shilf',     'Baragoon Tunnel',                    255, '', 4 ],
        [ 'Fargo',     'Boil Hole',                          23, 'Fire Ruby', 5 ],
        [ 'Guilty',    'Brannoch Castle',                    255, '', 6 ],
        [ 'Beigis',    'Brannoch Castle roof (shrine)',      255, '', 7 ],
        [ 'Mammon',    'Mammon\'s World',                    255, '(ending)', 8 ],
    );
    my @bosses;
    for my $k (0 .. 7) {
        my $i = 67 + $k;
        my @s = @stats[ $i * 6 .. $i * 6 + 5 ];
        my ($name, $arena, $item, $reward, $order) = @{ $boss_info[$k] };
        die "boss $k is $monster_names[$i], not $name" unless lc $monster_names[$i] eq lc $name;
        push @bosses, { id => $i, name => $name, arena => $arena, order => $order, reward => $reward, reward_id => $item,
                        hp => $s[0], atk => $s[1], def => $s[2], agi => $s[3], exp => $s[4],
                        drop => item_name($drops[ $i * 2 + 1 ]), drop_addr => $drops[ $i * 2 ] };
    }

    # ---------------------------------------------------------------- spirits
    # Not in Merrow. Each map file ends with its spirit list: 12-byte records of
    # { f32 x, f32 z, u16 id << 8, u16 0 } (some maps pad them to 16), the ids
    # unique across the game and consecutive within a map. 98 in all, ids 0-97,
    # which is also the cap on the game's collected-spirit counter. The map
    # table at 0x80054F10 (0x44 bytes a map, ROM range at +4/+8) says which
    # file is which; names are src/game/map_table.cpp's.
    my @map_names = (
        'Melrode', 'Dondoran', 'Holy Plains', 'Dondoran Flats', 'Larapool', 'West Carmagh (Greenoch)', 'Normoon',
        'West Limelin', 'Limelin', 'Dindom Dries', 'Shamwood', 'Brannoch / Baragoon Moor', 'Isle of Skye',
        'Melrode Monastery', 'Dondoran Castle', 'Melrode buildings', 'Dondoran buildings', 'Larapool buildings',
        'Larapool & Greenoch buildings', 'Normoon buildings (small set)', 'Normoon buildings', 'Limelin Castle',
        'Limelin buildings', 'Dindom Dries & Brannoch buildings', 'Shamwood buildings', 'Hidden rooms & shrines',
        'Blue Cave', 'Cull Hazard', 'Baragoon Tunnel (Shilf)', 'Boil Hole (Fargo)', 'Brannoch Castle',
        'Connor Forest (Solvaring)', 'Glencoe Forest', 'Windward Forest (Zelse)', "Mammon's World", "Nepty's arena (Isle of Skye)",
    );
    my %spirit_notes = (
        83 => 'Hidden inside the return ship (Limelin to Dondoran); "Reveal hidden spirits" moves it out',
        97 => 'Hidden inside the Limelin Castle library; "Reveal hidden spirits" moves it out',
    );
    my $spirit_count = 98;
    my $map_table = 0x80054F10 - 0x80000450 + 0x1050;
    my $sane = sub { my $v = shift; return 0 unless $v == $v; return 1 if $v == 0; return abs($v) >= 0.01 && abs($v) < 20000 };
    my (@spirits, @runs);
    for my $m (0 .. 35) {
        my ($start, $end) = unpack('NN', substr($rom, $map_table + $m * 0x44 + 4, 8));
        my @found;
        for (my $o = $end - 0x400; $o + 12 <= $end; $o += 4) {
            my ($x, $z, $id, $pad) = unpack('f>f>nn', substr($rom, $o, 12));
            next unless $pad == 0 && ($id & 0xFF) == 0 && ($id >> 8) < $spirit_count;
            next unless $sane->($x) && $sane->($z);
            push @found, { id => $id >> 8, x => $x, z => $z, addr => $o };
        }
        # Group into runs of consecutive ids (records sit 12-24 bytes apart);
        # other data can look like a record too, so the runs are settled below.
        my @run;
        for my $e (@found) {
            if (@run && $e->{id} == $run[-1]{id} + 1 && $e->{addr} - $run[-1]{addr} <= 24) { push @run, $e }
            else { push @runs, [ $m, @run ] if @run; @run = ($e) }
        }
        push @runs, [ $m, @run ] if @run;
    }
    # Ids are unique across the game, so the longest runs are the real lists
    # and anything that collides with one of them is not a spirit.
    my %claimed;
    for my $r (sort { @$b <=> @$a || $a->[0] <=> $b->[0] } @runs) {
        my ($m, @recs) = @$r;
        next if grep { $claimed{ $_->{id} } } @recs;
        $claimed{ $_->{id} } = 1 for @recs;
        push @spirits, { %$_, map => $m, map_name => $map_names[$m] } for @recs;
    }
    @spirits = sort { $a->{id} <=> $b->{id} } @spirits;
    my %nth;
    $_->{nth} = ++$nth{ $_->{map} } for @spirits;
    {
        my %ids = map { $_->{id} => 1 } @spirits;
        my @missing = grep { !$ids{$_} } 0 .. $spirit_count - 1;
        die "spirit scan: @{[scalar @spirits]} found, missing @missing" if @missing || @spirits != $spirit_count;
    }


    return {
        items   => \@items,   chests  => \@chests,  givers   => \@givers,
        monsters => \@monsters, bosses => \@bosses, spirits  => \@spirits,
        stage   => \@stage,   spirit_notes => \%spirit_notes,
    };
}

# The one place a check's Archipelago name is decided.
sub chest_name  { my $c = shift; return "Chest - $c->{name}" }
sub giver_name  { my $g = shift; return "Giver - $g->{who} ($g->{where})" }
sub enemy_name  { my $m = shift; return "Defeat - $m->{name}" }
sub boss_name   { my $b = shift; return "Boss - $b->{name}" }
sub spirit_name { my $s = shift; return "Spirit - $s->{map_name} #$s->{nth}" }

1;
