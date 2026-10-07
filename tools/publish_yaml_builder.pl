#!/usr/bin/perl
# Publishes the YAML builder page (tools/archipelago/yaml-builder.html, made
# by tools/yaml_builder.pl) to GitHub Pages:
#
#   perl tools/publish_yaml_builder.pl
#
# It pushes a commit holding only the page, as index.html (and an empty
# .nojekyll, so GitHub serves it as it is), to the gh-pages branch of the
# fork, which Pages serves at https://fuzzynessuk.github.io/Quest64-Recomp/.
# The commit is made with git's plumbing, so the working tree and the branch
# checked out are never touched. Nothing is pushed when the page has not
# changed.
use strict;
use warnings;
use File::Basename qw(dirname);
use File::Spec;

my $root = File::Spec->rel2abs(dirname(__FILE__) . '/..');
chdir $root or die "$root: $!";
my $remote = 'fork';
my $branch = 'gh-pages';
my $page = 'tools/archipelago/yaml-builder.html';
-f $page or die "$page is missing: run perl tools/yaml_builder.pl first\n";

sub git {
    my $out = qx{git @_};
    die "git @_ failed\n" if $?;
    $out =~ s{\s+\z}{};   # Windows git can end its output with \r\n
    return $out;
}

my $blob = git('hash-object', '-w', $page);
my $empty = git('hash-object', '-w', '--stdin', '<', File::Spec->devnull);
open my $mk, '|-', 'git mktree > .git/yaml_builder_tree' or die "git mktree: $!";
print $mk "100644 blob $empty\t.nojekyll\n100644 blob $blob\tindex.html\n";
close $mk or die "git mktree failed\n";
my $tree = do { open my $f, '<', '.git/yaml_builder_tree' or die; local $/; my $t = <$f>; $t =~ s{\s+\z}{}; $t };
unlink '.git/yaml_builder_tree';

# The branch's current commit, if it has one, is the parent.
my $parent = '';
my $remote_head = qx{git ls-remote $remote refs/heads/$branch};
if ($remote_head =~ /^([0-9a-f]{40})/) {
    $parent = $1;
    git('fetch', '--quiet', $remote, "refs/heads/$branch");
    if (git('rev-parse', "$parent^{tree}") eq $tree) {
        print "the page on $branch is already this one; nothing to publish\n";
        exit 0;
    }
}
my $version = '';
if (open my $j, '<', 'tools/archipelago/quest64/archipelago.json') {
    local $/;
    ($version) = (<$j> =~ /"world_version":\s*"([^"]+)"/);
}
my $message = 'YAML builder' . ($version ? " for apworld $version" : '');
my $commit = git('commit-tree', $tree, ($parent ? ('-p', $parent) : ()), '-m', "\"$message\"");
git('push', $remote, "$commit:refs/heads/$branch");
print "published $commit to $remote $branch ($message)\n";
print "https://fuzzynessuk.github.io/Quest64-Recomp/\n";
