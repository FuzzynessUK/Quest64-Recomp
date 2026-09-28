package Quest64Logic;
# Reads the Archipelago logic from DOCS/archipelago_logic.xlsx and checks it.
#
# The workbook (made by tools/archipelago_logic_sheet.pl, then edited by hand)
# is the one place the world's logic is decided: its Regions, Entrances,
# Locations and Named Rules sheets, and the completion rule on the Guide
# sheet. tools/archipelago_world.pl writes Regions.py and Rules.py from what
# this returns, with Archipelago's Rule Builder.
#
# A Rule cell becomes Python source, so it is parsed here rather than pasted:
# only Rule Builder calls, option classes, named rules, string and number
# literals, & | ( ) , and keyword arguments are allowed, and every item,
# region, location, option and named rule it mentions has to exist. A typo
# stops the generator instead of making every seed unwinnable.
#
# Reads the file either as archipelago_logic_sheet.pl writes it (inline
# strings, formulas with no cached value) or as Excel saves it (shared
# strings, cached values).
use strict;
use warnings;
use IO::Uncompress::Unzip qw(unzip $UnzipError);

# Rule Builder names a Rule cell may call, from rule_builder.rules/.options.
our %RULE_CALLS = map { $_ => 1 } qw(
    Has HasAll HasAny HasAllCounts HasAnyCount HasFromList HasFromListUnique HasGroup HasGroupUnique
    CanReachRegion CanReachLocation CanReachEntrance OptionFilter True_ False_
);
# Calls whose string arguments are item names, region names, location names.
my %ITEM_CALLS = map { $_ => 1 } qw(Has HasAll HasAny HasFromList HasFromListUnique);
my %KEYWORDS = map { $_ => 1 } qw(count operator);
my %OPERATORS = map { $_ => 1 } qw(eq ne gt ge lt le contains);

sub member {
    my ($zip, $name) = @_;
    my $out;
    unzip($zip => \$out, Name => $name) or return undef;
    return $out;
}

sub unxml {
    my $s = shift;
    $s =~ s/&lt;/</g; $s =~ s/&gt;/>/g; $s =~ s/&quot;/"/g; $s =~ s/&apos;/'/g;
    $s =~ s/&#(\d+);/chr($1)/ge; $s =~ s/&#x([0-9A-Fa-f]+);/chr(hex $1)/ge;
    $s =~ s/&amp;/&/g;
    return $s;
}

sub text_of {   # all <t> runs inside a string item
    my $x = shift;
    my $t = '';
    $t .= unxml($1) while $x =~ m{<t(?:\s[^>]*)?>(.*?)</t>}gs;
    return $t;
}

sub col_number { my $c = shift; my $n = 0; $n = $n * 26 + (ord($_) - 64) for split //, $c; return $n }

# { sheet name => [ [row cells...], ... ] }, rows and columns 1-based.
sub read_workbook {
    my ($path) = @_;
    my $wb = member($path, 'xl/workbook.xml') or die "$path: not a workbook ($UnzipError)\n";
    my $rels = member($path, 'xl/_rels/workbook.xml.rels') // '';
    my %target;
    $target{$1} = $2 while $rels =~ /<Relationship\b[^>]*\bId="([^"]+)"[^>]*\bTarget="([^"]+)"/g;
    while ($rels =~ /<Relationship\b[^>]*\bTarget="([^"]+)"[^>]*\bId="([^"]+)"/g) { $target{$2} //= $1 }
    my @shared;
    if (my $ss = member($path, 'xl/sharedStrings.xml')) {
        push @shared, text_of($1) while $ss =~ m{<si>(.*?)</si>}gs;
    }
    my %sheets;
    while ($wb =~ /<sheet\b([^>]*)\/?>/g) {
        my $attrs = $1;
        my ($name) = $attrs =~ /\bname="([^"]*)"/;
        my ($rid)  = $attrs =~ /\br:id="([^"]*)"/;
        next unless defined $name && defined $rid && $target{$rid};
        (my $file = $target{$rid}) =~ s{^/?(xl/)?}{xl/};
        my $xml = member($path, $file) // next;
        my @rows;
        while ($xml =~ m{<c\b([^>]*?)(?:/>|>(.*?)</c>)}gs) {
            my ($attrs, $body) = ($1, $2 // '');
            my ($ref) = $attrs =~ /\br="([A-Z]+\d+)"/ or next;
            my ($col, $row) = $ref =~ /([A-Z]+)(\d+)/;
            my ($type) = $attrs =~ /\bt="([^"]*)"/;
            $type //= '';
            my $value;
            if ($type eq 'inlineStr') { $value = text_of($body) }
            elsif ($type eq 's') { my ($i) = $body =~ m{<v>(\d+)</v>}; $value = defined $i ? $shared[$i] : undef }
            else { my ($v) = $body =~ m{<v>(.*?)</v>}s; $value = defined $v ? unxml($v) : undef }
            $rows[$row][ col_number($col) ] = $value if defined $value;
        }
        $sheets{unxml($name)} = \@rows;
    }
    return \%sheets;
}

sub cell { my ($rows, $r, $c) = @_; my $v = $rows->[$r] ? $rows->[$r][$c] : undef; $v //= ''; $v =~ s/^\s+|\s+$//g; return $v }

# ---------------------------------------------------------------- rules
# Splits a Rule cell into tokens: [kind, text].
sub tokens {
    my ($src, $where) = @_;
    my @t;
    pos($src) = 0;
    while (pos($src) < length $src) {
        if    ($src =~ /\G\s+/gc) { }
        elsif ($src =~ /\G"((?:[^"\\]|\\.)*)"/gc) { push @t, [ 'str', $1 ] }
        elsif ($src =~ /\G'((?:[^'\\]|\\.)*)'/gc) { push @t, [ 'str', $1 ] }
        elsif ($src =~ /\G(\d+)/gc)               { push @t, [ 'num', $1 ] }
        elsif ($src =~ /\G([A-Za-z_]\w*)/gc)      { push @t, [ 'id', $1 ] }
        elsif ($src =~ /\G([&|(),=])/gc)          { push @t, [ 'op', $1 ] }
        else { die "$where: cannot read the rule at \"" . substr($src, pos($src), 12) . "\" in: $src\n" }
    }
    return @t;
}

# Checks one rule and notes what it uses. Returns the rule as Python source.
sub check_rule {
    my ($src, $where, $ctx, $uses) = @_;
    my @t = tokens($src, $where);
    my @stack;          # enclosing calls
    my $depth = 0;
    for my $i (0 .. $#t) {
        my ($kind, $text) = @{ $t[$i] };
        my $next = $t[ $i + 1 ] // [ '', '' ];
        if ($kind eq 'id') {
            if ($next->[1] eq '(' && $RULE_CALLS{$text}) {
                $uses->{calls}{$text}++;
                push @stack, [ $text, $depth ];
            }
            elsif ($next->[1] eq '=' && $KEYWORDS{$text}) { }
            elsif ($ctx->{options}{$text}) {
                die "$where: $text is an option class, only OptionFilter takes one: $src\n"
                    unless @stack && $stack[-1][0] eq 'OptionFilter';
                $uses->{options}{$text}++;
            }
            elsif ($ctx->{named}{$text}) { $uses->{named}{$text}++ }
            elsif ($text eq 'True' || $text eq 'False') { }
            else { die "$where: \"$text\" is not a Rule Builder call, an option class or a named rule: $src\n" }
        }
        elsif ($kind eq 'op' && $text eq '(') { $depth++ }
        elsif ($kind eq 'op' && $text eq ')') {
            $depth--;
            die "$where: unbalanced brackets: $src\n" if $depth < 0;
            pop @stack if @stack && $stack[-1][1] == $depth;
        }
        elsif ($kind eq 'str') {
            my $call = @stack ? $stack[-1][0] : '';
            my $prev = $i > 0 ? $t[ $i - 1 ][1] : '';
            if ($prev eq '=' && $i > 1 && $t[ $i - 2 ][1] eq 'operator') {
                die "$where: operator must be one of @{[ sort keys %OPERATORS ]}: $src\n" unless $OPERATORS{$text};
            }
            elsif ($ITEM_CALLS{$call}) {
                die "$where: no item called \"$text\": $src\n" unless $ctx->{items}{$text};
                $uses->{items}{$text}++;
            }
            elsif ($call eq 'CanReachRegion') {
                die "$where: no region called \"$text\": $src\n" unless $ctx->{regions}{$text};
            }
            elsif ($call eq 'CanReachLocation') {
                die "$where: no location called \"$text\": $src\n" unless $ctx->{locations}{$text};
            }
        }
    }
    die "$where: unbalanced brackets: $src\n" if $depth != 0;
    return $src;
}

# Reads and checks the whole workbook. $known has the names the apworld
# really has: items => { name => classification }, locations => { name => 1 },
# options => { class => 1 }.
sub read_logic {
    my ($path, $known) = @_;
    my $book = read_workbook($path);
    for my $need ('Guide', 'Regions', 'Entrances', 'Locations', 'Named Rules') {
        die "$path: no \"$need\" sheet\n" unless $book->{$need};
    }
    my %logic = (regions => [], entrances => [], locations => {}, named => [], completion => '');

    # Regions: column A from row 4.
    my %region;
    my $rows = $book->{Regions};
    for my $r (4 .. $#$rows) {
        my $name = cell($rows, $r, 1);
        next if $name eq '';
        die "Regions row $r: \"$name\" appears twice\n" if $region{$name}++;
        push @{ $logic{regions} }, $name;
    }
    die "Regions: there must be a region called Menu, where Archipelago starts\n" unless $region{Menu};

    # Named Rules: column A name, B rule.
    my %named;
    $rows = $book->{'Named Rules'};
    for my $r (4 .. $#$rows) {
        my $name = cell($rows, $r, 1);
        next if $name eq '';
        die "Named Rules row $r: \"$name\" is not a Python name\n" unless $name =~ /^[A-Z_][A-Z0-9_]*$/;
        die "Named Rules row $r: \"$name\" appears twice\n" if $named{$name};
        $named{$name} = { row => $r, rule => cell($rows, $r, 2) };
        die "Named Rules row $r: $name has no rule\n" if $named{$name}{rule} eq '';
    }

    my $ctx = { items => $known->{items}, locations => $known->{locations}, options => $known->{options},
                regions => \%region, named => \%named };
    my %uses;

    # Named rules, each after the ones it uses.
    my %deps;
    for my $name (sort keys %named) {
        my %u;
        $named{$name}{rule} = check_rule($named{$name}{rule}, "Named Rules $name", $ctx, \%u);
        $deps{$name} = [ sort keys %{ $u{named} // {} } ];
        merge(\%uses, \%u);
    }
    my (%done, %busy);
    my $visit;
    $visit = sub {
        my $n = shift;
        return if $done{$n};
        die "Named Rules: $n uses itself, through @{[ join ' -> ', sort keys %busy ]}\n" if $busy{$n};
        $busy{$n} = 1;
        $visit->($_) for @{ $deps{$n} };
        delete $busy{$n};
        $done{$n} = 1;
        push @{ $logic{named} }, [ $n, $named{$n}{rule} ];
    };
    $visit->($_) for sort { $named{$a}{row} <=> $named{$b}{row} } keys %named;

    # Entrances: B from, C to, D rule.
    my %seen_entrance;
    $rows = $book->{Entrances};
    for my $r (4 .. $#$rows) {
        my ($from, $to, $rule) = (cell($rows, $r, 2), cell($rows, $r, 3), cell($rows, $r, 4));
        next if $from eq '' && $to eq '' && $rule eq '';
        die "Entrances row $r: needs both a From and a To\n" if $from eq '' || $to eq '';
        die "Entrances row $r: no region called \"$from\"\n" unless $region{$from};
        die "Entrances row $r: no region called \"$to\"\n" unless $region{$to};
        my $name = "$from to $to";
        die "Entrances row $r: \"$name\" appears twice\n" if $seen_entrance{$name}++;
        my %u;
        $rule = check_rule($rule, "Entrances $name", $ctx, \%u) if $rule ne '';
        merge(\%uses, \%u);
        push @{ $logic{entrances} }, { name => $name, from => $from, to => $to, rule => $rule };
    }

    # Every region reachable from Menu (ignoring rules: that is AP's job).
    {
        my %reach = (Menu => 1);
        my @todo = ('Menu');
        while (@todo) {
            my $at = shift @todo;
            for my $e (grep { $_->{from} eq $at } @{ $logic{entrances} }) {
                push @todo, $e->{to} unless $reach{ $e->{to} }++;
            }
        }
        my @cut = grep { !$reach{$_} } @{ $logic{regions} };
        die "Regions with no way in from Menu: @cut\n" if @cut;
    }

    # Locations: A name, F region, G rule. Every location the apworld has,
    # and no other.
    $rows = $book->{Locations};
    for my $r (4 .. $#$rows) {
        my $name = cell($rows, $r, 1);
        next if $name eq '';
        die "Locations row $r: \"$name\" is not a location the apworld has (names are fixed)\n"
            unless $known->{locations}{$name};
        die "Locations row $r: \"$name\" appears twice\n" if $logic{locations}{$name};
        my ($region, $rule) = (cell($rows, $r, 6), cell($rows, $r, 7));
        die "Locations row $r: $name has no region\n" if $region eq '';
        die "Locations row $r: $name is in \"$region\", which is not on the Regions sheet\n" unless $region{$region};
        my %u;
        $rule = check_rule($rule, "Locations $name", $ctx, \%u) if $rule ne '';
        merge(\%uses, \%u);
        $logic{locations}{$name} = { region => $region, rule => $rule };
    }
    my @missing = grep { !$logic{locations}{$_} } sort keys %{ $known->{locations} };
    die "Locations sheet is missing: @missing\n" if @missing;

    # The completion rule: the Guide sheet's "Completion rule" row, column B.
    $rows = $book->{Guide};
    for my $r (1 .. $#$rows) {
        next unless cell($rows, $r, 1) eq 'Completion rule';
        $logic{completion} = cell($rows, $r, 2);
    }
    die "Guide: no Completion rule\n" if $logic{completion} eq '';
    my %u;
    $logic{completion} = check_rule($logic{completion}, 'Completion rule', $ctx, \%u);
    merge(\%uses, \%u);

    # A rule that waits on an item AP treats as filler is never satisfied
    # on purpose: the fill does not place filler with logic in mind.
    my @soft = grep { ($known->{items}{$_} // '') ne 'progression' } sort keys %{ $uses{items} // {} };
    die "rules need items that are not progression: @soft\n" if @soft;

    $logic{uses} = \%uses;
    return \%logic;
}

sub merge {
    my ($into, $from) = @_;
    for my $k (keys %$from) { $into->{$k}{$_} += $from->{$k}{$_} for keys %{ $from->{$k} } }
}

1;
