#!/usr/bin/perl
# Writes tools/archipelago/yaml-builder.html, the Archipelago YAML builder: a
# web page with every option of the player yaml, grouped the way
# archipelago.gg's player-options pages are (dropdowns and sliders, each
# option's explanation on hover), which opens a yaml and saves a filled-in one.
# Published on GitHub Pages (tools/publish_yaml_builder.pl); the game ships a
# shortcut to it.
#
#   perl tools/yaml_builder.pl
#
# Read from what tools/archipelago_world.pl generates - the yaml template and
# the apworld's Options.py - and run by it at the end, so the page always
# offers exactly the options the apworld has. Saving writes the chosen values
# into a copy of the template, so the file keeps the template's comments.
use strict;
use warnings;
use File::Basename qw(dirname);
use JSON::PP;

my $root = dirname(__FILE__) . '/..';
sub slurp { my $p = shift; open my $f, '<:raw', $p or die "$p: $!"; local $/; my $s = <$f>; close $f; $s =~ s/\r//g; return $s }

my $yaml = slurp("$root/tools/archipelago/Quest64Recompiled.yaml");
my $py   = slurp("$root/tools/archipelago/quest64/Options.py");
my ($world_version) = (slurp("$root/tools/archipelago/quest64/archipelago.json") =~ /"world_version":\s*"([^"]+)"/);
$world_version //= '';

# ---------------------------------------------------------------- Options.py
my %class;   # class name -> { title, tip, min, max }
while ($py =~ /^class (\w+)\((\w+)\):\n((?:    .*\n|\n)*)/mg) {
    my ($name, $base, $body) = ($1, $2, $3);
    my %c = (base => $base);
    if ($body =~ /^    """(.*?)"""/ms) {
        my $doc = $1;
        # Dedent the docstring the way Python's inspect.cleandoc does.
        my @l = split /\n/, $doc, -1;
        my $first = shift @l;
        my $indent;
        for (@l) { next unless /\S/; my ($sp) = /^( *)/; $indent = length $sp if !defined $indent || length $sp < $indent }
        $indent //= 0;
        s/^ {0,$indent}// for @l;
        $doc = join("\n", $first, @l);
        $doc =~ s/^\s+|\s+$//g;
        $c{tip} = $doc;
    }
    $c{title} = $1 if $body =~ /^    display_name = "([^"]*)"/m;
    $c{min} = $1 if $body =~ /^    range_start = (-?\d+)/m;
    $c{max} = $1 if $body =~ /^    range_end = (-?\d+)/m;
    $class{$name} = \%c;
}
my %class_of;   # yaml key -> class name
{
    my ($body) = ($py =~ /class Q64Options\(PerGameCommonOptions\):\n((?:    .*\n)*)/) or die "no Q64Options\n";
    while ($body =~ /^    (\w+): (\w+)$/mg) { $class_of{$1} = $2 }
}
# Archipelago's own options, which are not in Options.py.
my %builtin = (
    death_link => { title => 'Death Link', min => 0, max => 0 },
    progression_balancing => {
        title => 'Progression Balancing', min => 0, max => 99,
        tip => "How hard the generator tries to give you your progression items early, rather than leave them " .
               "to arrive late in the run. 0 is off, 50 the default, 99 the most.",
    },
    accessibility => {
        title => 'Accessibility',
        tip => "full: every location can be reached, so every check is possible.\n" .
               "minimal: only what is needed to finish is guaranteed; some locations may be unreachable.",
        choices => [ 'full', 'minimal' ],
    },
);

# ---------------------------------------------------------------- the yaml
my @lines = split /\n/, $yaml, -1;
pop @lines if @lines && $lines[-1] eq '';
my @options;
my $section = '';
my @comment;
my $in_game = 0;   # past the "Quest 64 Recompiled:" line: the header above it has two-space keys too
for (my $i = 0; $i < @lines; $i++) {
    my $l = $lines[$i];
    if ($l =~ /^Quest 64 Recompiled:\s*$/) { $in_game = 1; next }
    next unless $in_game;
    if ($l =~ /^  # ==== (.+?)\s*$/) { $section = $1; @comment = (); next }
    if ($l =~ /^  # ?(.*)$/) { push @comment, $1; next }
    if ($l =~ /^  (\w+):\s*$/) {
        my $key = $1;
        my @choices;
        for (my $j = $i + 1; $j < @lines && $lines[$j] =~ /^    ('?)([\w.-]+)\1:\s*-?\d+\s*$/; $j++) { push @choices, $2 }
        push @options, { key => $key, section => $section, kind => 'choice', choices => \@choices, comment => [ @comment ] };
        @comment = ();
        next;
    }
    if ($l =~ /^  (\w+): (\S+)\s*$/) {
        my ($key, $value) = ($1, $2);
        my $kind = $value =~ /^-?\d+$/ ? 'range' : 'choice';
        push @options, { key => $key, section => $section, kind => $kind, scalar => 1,
                         choices => $kind eq 'choice' ? ($builtin{$key}{choices} // [ $value ]) : [], comment => [ @comment ] };
        @comment = ();
        next;
    }
    @comment = () if $l !~ /^\s*$/ || $l eq '';
}
die "only " . scalar(@options) . " options read from the yaml\n" if @options < 40;

# Groups, in the order the builder shows them. The Archipelago block of the
# yaml is split up; the Randomizer, Enhancements and Cosmetics sections are
# groups of their own. A new option goes in its yaml section unless listed
# here; an unknown section stops the script.
my @group_order = ('Game Options', 'Goal', 'Checks', 'Items and World', 'Traps', 'Randomizer', 'Enhancements', 'Cosmetics');
my %group_of = (
    death_link => 'Game Options', progression_balancing => 'Game Options', accessibility => 'Game Options',
    goal => 'Goal', mammon_portal => 'Goal', orbs_required => 'Goal', pages_required => 'Goal', page_placement => 'Goal',
    chestsanity => 'Checks', giftsanity => 'Checks', enemysanity => 'Checks', ensure_all_enemies => 'Checks',
    spiritsanity => 'Checks', extra_level_ups => 'Checks',
    wings => 'Items and World', wings_in_pool => 'Items and World', boss_items => 'Items and World',
    open_world => 'Items and World', boss_souls => 'Items and World',
);
sub label {
    my $n = shift;
    return 'On' if $n eq 'true';
    return 'Off' if $n eq 'false';
    $n =~ s/_/ /g;
    return ucfirst $n;
}
for my $o (@options) {
    my $k = $o->{key};
    my $g = $group_of{$k} // ($k =~ /trap/ ? 'Traps' : $o->{section});
    my ($gi) = grep { $group_order[$_] eq $g } 0 .. $#group_order;
    die "option $k: no group\n" unless defined $gi;
    $o->{group} = $gi;
    my $c = $class_of{$k} ? $class{ $class_of{$k} } : undef;
    my $b = $builtin{$k} // {};
    $o->{title} = ($c && $c->{title}) // $b->{title} // join(' ', map { ucfirst } split /_/, $k);
    $o->{tip} = ($c && $c->{tip}) // $b->{tip} // join("\n", @{ $o->{comment} });
    if ($o->{kind} eq 'range') {
        $o->{min} = 0 + (($c && defined $c->{min}) ? $c->{min} : $b->{min} // die "option $k: no range\n");
        $o->{max} = 0 + (($c && defined $c->{max}) ? $c->{max} : $b->{max} // die "option $k: no range\n");
    }
    if ($o->{kind} eq 'choice') {
        my @ch = @{ $o->{choices} };
        die "option $k: no choices\n" unless @ch;
        $o->{kind} = 'toggle' if @ch == 2 && grep({ $_ eq 'true' } @ch) && grep({ $_ eq 'false' } @ch);
    }
}

# Within a group the yaml's order, except that these come first, in this
# order (the goal before what the portal asks for).
my @lead = qw(goal mammon_portal orbs_required pages_required page_placement);
my %lead_rank = map { $lead[$_] => $_ } 0 .. $#lead;
{
    my %pos = map { $options[$_]{key} => $_ } 0 .. $#options;
    @options = sort {
        $a->{group} <=> $b->{group}
            || ($lead_rank{ $a->{key} } // 1e6) <=> ($lead_rank{ $b->{key} } // 1e6)
            || $pos{ $a->{key} } <=> $pos{ $b->{key} }
    } @options;
}

# ---------------------------------------------------------------- the page
my $data = {
    worldVersion => $world_version,
    groups => \@group_order,
    options => [ map {
        my $o = $_;
        {
            key => $o->{key}, title => $o->{title}, tip => $o->{tip}, group => $o->{group},
            kind => $o->{kind}, scalar => $o->{scalar} ? JSON::PP::true : JSON::PP::false,
            ($o->{kind} eq 'range' ? (min => $o->{min}, max => $o->{max}) : ()),
            choices => [ map { { name => $_, label => label($_) } } @{ $o->{choices} } ],
        }
    } @options ],
    template => \@lines,
};
my $json = JSON::PP->new->canonical->ascii->encode($data);
$json =~ s{</}{<\\/}g;   # never end the <script> early

my $html = do { local $/; <DATA> };
$html =~ s/__DATA__JSON__/$json/ or die "no data placeholder\n";
$html =~ s/__WORLD_VERSION__/$world_version/g;

my $path = "$root/tools/archipelago/yaml-builder.html";
open my $o, '>:raw', $path or die "$path: $!";
print $o $html;
close $o;
printf "wrote %s: %d options in %d groups, %d template lines\n", $path, scalar(@options), scalar(@group_order), scalar(@lines);

__DATA__
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Quest 64 Recompilation - Archipelago Options</title>
<!-- GENERATED by tools/yaml_builder.pl from the apworld's yaml template and
     Options.py; do not edit. -->
<style>
  :root {
    --bar: #ffdea3;          /* the cream bar, as on archipelago.gg */
    --bar-text: #4a3616;
    --page: #3b7a2a;         /* the green page */
    --header: #ffd85a;
    --label: #fff3d6;
    --tip-bg: #ffffff;
    --tip-text: #1f1f1f;
    --tip-border: #8a8a8a;
  }
  * { box-sizing: border-box; }
  html, body { margin: 0; height: 100%; }
  body {
    background: var(--page);
    font-family: "Segoe UI", system-ui, -apple-system, Roboto, Arial, sans-serif;
    display: flex;
    flex-direction: column;
  }
  header {
    position: sticky;
    top: 0;
    z-index: 2;
    background: var(--bar);
    color: var(--bar-text);
    min-height: 72px;
    padding: 8px 16px 8px 18px;
    display: flex;
    align-items: center;
    gap: 8px;
    flex-wrap: wrap;
  }
  .titles { flex: 1 1 auto; min-width: 200px; }
  .title { font-size: 22px; font-weight: 700; line-height: 30px; }
  .subtitle { font-size: 16px; font-weight: 600; line-height: 24px; margin-left: 1px; }
  .bar-controls { display: flex; align-items: center; gap: 8px; flex-wrap: wrap; font-size: 15px; }
  .bar-controls label { margin-right: 0; }
  #name {
    width: 170px; height: 32px; font: inherit; padding: 0 6px;
    border: 1px solid #7a7a7a; border-radius: 2px; background: #fff; color: #000;
  }
  button {
    height: 32px; font: inherit; padding: 0 14px; cursor: pointer;
    background: #fdfdfd; color: #000; border: 1px solid #d0d0d0; border-radius: 4px;
  }
  button:hover { background: #e0eef9; border-color: #0078d4; }
  button.default { border-color: #0078d4; }
  main { padding: 14px 28px 24px; flex: 1 0 auto; }
  h2 {
    color: var(--header); font-size: 26px; font-weight: 600;
    margin: 0 0 10px; line-height: 44px;
  }
  .group { margin-bottom: 26px; }
  .columns { display: grid; grid-template-columns: 1fr 1fr; column-gap: 40px; }
  .column { min-width: 0; }
  .row { display: flex; align-items: center; height: 38px; }
  .row .label {
    color: var(--label); font-size: 17px; flex: 0 0 min(260px, 45%);
    white-space: nowrap; overflow: hidden; text-overflow: ellipsis; padding-right: 10px; cursor: default;
  }
  .row .control { flex: 1 1 auto; display: flex; align-items: center; min-width: 0; }
  select {
    width: 100%; height: 28px; font: 15px "Segoe UI", system-ui, sans-serif; color: #000;
    background: #fff; border: 1px solid #7a7a7a; border-radius: 2px; padding: 0 4px;
  }
  input[type=range] { flex: 1 1 auto; min-width: 0; accent-color: #0078d4; margin: 0 6px 0 8px; }
  .number { color: var(--label); font-size: 17px; width: 44px; text-align: right; flex: 0 0 44px; }
  #tip {
    position: fixed; z-index: 5; display: none; max-width: 520px;
    background: var(--tip-bg); color: var(--tip-text); border: 1px solid var(--tip-border);
    padding: 6px 8px; font-size: 15px; white-space: pre-wrap; pointer-events: none;
    box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.25);
  }
  footer { color: var(--label); opacity: 0.75; font-size: 13px; padding: 0 28px 18px; }
  footer a { color: var(--header); }
  @media (max-width: 1000px) {
    .columns { grid-template-columns: 1fr; }
    main { padding: 14px 16px 24px; }
  }
</style>
</head>
<body>
<header>
  <div class="titles">
    <div class="title">Quest 64 Recompilation</div>
    <div class="subtitle">Archipelago Options</div>
  </div>
  <div class="bar-controls">
    <label for="name">Player name:</label>
    <input id="name" maxlength="16" placeholder="Your slot name" autocomplete="off" spellcheck="false">
    <button id="open" type="button">Open YAML&hellip;</button>
    <button id="defaults" type="button">Defaults</button>
    <button id="save" type="button" class="default">Save YAML&hellip;</button>
    <input id="file" type="file" accept=".yaml,.yml,text/yaml" hidden>
  </div>
</header>
<main id="page"></main>
<footer>For the Quest 64 Recompiled apworld __WORLD_VERSION__. Put the saved file in Archipelago's Players folder,
with quest64.apworld in custom_worlds, and generate.</footer>
<div id="tip"></div>
<script>
"use strict";
const DATA = __DATA__JSON__;
const options = DATA.options;

// ---------------------------------------------------------------- the yaml

function trim(s) { return s.replace(/^[ \t\r]+|[ \t\r]+$/g, ""); }
function unquote(s) {
  s = trim(s);
  if (s.length >= 2 && (s[0] === "'" || s[0] === '"') && s[s.length - 1] === s[0]) s = s.slice(1, -1);
  return s;
}

function choiceIndex(o, name) {
  let n = unquote(name);
  // A toggle may also be written as yes/no or on/off.
  if (o.kind === "toggle") {
    const lower = n.toLowerCase();
    if (lower === "yes" || lower === "on" || lower === "1") n = "true";
    if (lower === "no" || lower === "off" || lower === "0") n = "false";
    if (lower === "true" || lower === "false") n = lower;
  }
  return o.choices.findIndex(c => c.name === n);
}

// Reads the options out of yaml lines. Only the "Quest 64 Recompiled:"
// section counts; anything not recognised is left at what `out` held.
function readYaml(lines, out) {
  let inGame = false;
  for (let i = 0; i < lines.length; i++) {
    let line = lines[i].replace(/\r$/, "");
    if (line.startsWith("name:")) { out.name = unquote(line.slice(5)); continue; }
    if (line.length && line[0] !== " " && line[0] !== "#") { inGame = trim(line) === "Quest 64 Recompiled:"; continue; }
    if (!inGame || line.length < 3 || line[0] !== " " || line[1] !== " " || line[2] === " " || line[2] === "#") continue;
    const colon = line.indexOf(":");
    if (colon < 0) continue;
    const key = trim(line.slice(2, colon));
    const rest = trim(line.slice(colon + 1));
    const k = options.findIndex(o => o.key === key);
    if (k < 0) continue;
    const o = options[k];
    // The candidates: a scalar, or the block of weighted entries below.
    const entries = [];
    if (rest.length && rest[0] !== "#") {
      entries.push([rest, 1]);
    } else {
      for (let j = i + 1; j < lines.length; j++) {
        const sub = lines[j].replace(/\r$/, "");
        if (!trim(sub).length || trim(sub)[0] === "#") continue;
        if (!sub.startsWith("    ")) break;
        const c = sub.lastIndexOf(":");
        if (c < 0) continue;
        entries.push([trim(sub.slice(0, c)), parseFloat(sub.slice(c + 1)) || 0]);
      }
    }
    // The highest weight wins; ties go to the first.
    let best = -1;
    for (const [name, weight] of entries) {
      if (weight <= best) continue;
      if (o.kind === "range") {
        const n = parseInt(unquote(name), 10);
        if (!isNaN(n)) { out.value[k] = Math.min(o.max, Math.max(o.min, n)); best = weight; }
      } else {
        const idx = choiceIndex(o, name);
        if (idx >= 0) { out.value[k] = idx; best = weight; }
      }
    }
    if (best >= 0) out.found++;
  }
}

function defaults() {
  const v = { name: "", value: options.map(() => 0), found: 0 };
  readYaml(DATA.template, v);
  v.name = "";   // the template's "Player{number}" is not a name
  return v;
}

// The template with the chosen values written in.
function buildYaml(v) {
  const lines = DATA.template.slice();
  let inGame = false;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (line.startsWith("name:")) { lines[i] = "name: " + v.name; continue; }
    if (line.length && line[0] !== " " && line[0] !== "#") { inGame = trim(line) === "Quest 64 Recompiled:"; continue; }
    if (!inGame || line.length < 3 || !line.startsWith("  ") || line[2] === " " || line[2] === "#") continue;
    const key = line.slice(2, line.indexOf(":"));
    const k = options.findIndex(o => o.key === key);
    if (k < 0) continue;
    const o = options[k];
    if (o.scalar) {
      lines[i] = "  " + key + ": " + (o.kind === "range" ? String(v.value[k]) : o.choices[v.value[k]].name);
    } else {
      for (let j = i + 1; j < lines.length && lines[j].startsWith("    "); j++) {
        const c = lines[j].lastIndexOf(":");
        const name = unquote(lines[j].slice(4, c));
        lines[j] = lines[j].slice(0, c + 1) + (name === o.choices[v.value[k]].name ? " 1" : " 0");
      }
    }
  }
  return lines.map(l => l + "\n").join("");
}

// ---------------------------------------------------------------- the page

let values = defaults();
const controls = [];   // per option: { input, number }
const tip = document.getElementById("tip");
let tipTimer = 0;

function showTip(text, x, y) {
  tip.textContent = text;
  tip.style.display = "block";
  const w = tip.offsetWidth, h = tip.offsetHeight;
  let left = x + 14, top = y + 20;
  if (left + w > window.innerWidth - 8) left = Math.max(8, window.innerWidth - w - 8);
  if (top + h > window.innerHeight - 8) top = Math.max(8, y - h - 12);
  tip.style.left = left + "px";
  tip.style.top = top + "px";
}
function hideTip() { clearTimeout(tipTimer); tip.style.display = "none"; }
function addTip(el, text) {
  el.addEventListener("mouseenter", e => {
    clearTimeout(tipTimer);
    const x = e.clientX, y = e.clientY;
    tipTimer = setTimeout(() => showTip(text, x, y), 300);
  });
  el.addEventListener("mouseleave", hideTip);
  el.addEventListener("mousedown", hideTip);
}

function build() {
  const page = document.getElementById("page");
  DATA.groups.forEach((group, g) => {
    const members = options.map((o, k) => k).filter(k => options[k].group === g);
    if (!members.length) return;
    const section = document.createElement("section");
    section.className = "group";
    const h = document.createElement("h2");
    h.textContent = group;
    section.appendChild(h);
    const cols = document.createElement("div");
    cols.className = "columns";
    // As the window does: the first half down the left, the rest the right.
    const perColumn = Math.ceil(members.length / 2);
    const left = document.createElement("div"), right = document.createElement("div");
    left.className = right.className = "column";
    cols.append(left, right);
    members.forEach((k, m) => {
      const o = options[k];
      const row = document.createElement("div");
      row.className = "row";
      const label = document.createElement("div");
      label.className = "label";
      label.textContent = o.title;
      const control = document.createElement("div");
      control.className = "control";
      let input, number = null;
      if (o.kind === "range") {
        input = document.createElement("input");
        input.type = "range";
        input.min = o.min;
        input.max = o.max;
        input.step = 1;
        number = document.createElement("span");
        number.className = "number";
        input.addEventListener("input", () => {
          values.value[k] = parseInt(input.value, 10);
          number.textContent = input.value;
        });
        control.append(input, number);
      } else {
        input = document.createElement("select");
        o.choices.forEach((c, i) => input.add(new Option(c.label, String(i))));
        input.addEventListener("change", () => { values.value[k] = parseInt(input.value, 10); });
        control.append(input);
      }
      input.setAttribute("aria-label", o.title);
      const text = o.title + "\n\n" + o.tip;
      addTip(label, text);
      addTip(input, text);
      row.append(label, control);
      (m < perColumn ? left : right).appendChild(row);
      controls[k] = { input, number };
    });
    section.appendChild(cols);
    page.appendChild(section);
  });
}

function show() {
  document.getElementById("name").value = values.name;
  options.forEach((o, k) => {
    const c = controls[k];
    c.input.value = String(values.value[k]);
    if (c.number) c.number.textContent = String(values.value[k]);
  });
}

function readName() { values.name = trim(document.getElementById("name").value); }

async function save() {
  readName();
  const nameBox = document.getElementById("name");
  if (!values.name) {
    alert("Enter your player (slot) name first. It is how the room knows you, and what you type into the game's Archipelago tab.");
    nameBox.focus();
    return;
  }
  if (/[:#{}\[\]]/.test(values.name)) {
    alert("A player name cannot contain : # { } [ or ].");
    nameBox.focus();
    return;
  }
  const text = buildYaml(values);
  const suggested = values.name + ".yaml";
  const done = "Put it in Archipelago's Players folder (with quest64.apworld in custom_worlds) and generate.";
  // Chrome and Edge: a real "Save as" box, so the player picks the folder.
  if (window.showSaveFilePicker) {
    try {
      const handle = await window.showSaveFilePicker({
        suggestedName: suggested,
        types: [{ description: "YAML files", accept: { "text/yaml": [".yaml"] } }],
      });
      const writable = await handle.createWritable();
      await writable.write(text);
      await writable.close();
      alert("Saved " + handle.name + "\n\n" + done);
      return;
    } catch (e) {
      if (e && e.name === "AbortError") return;   // cancelled
    }
  }
  // Elsewhere a download, which goes where the browser keeps downloads (or
  // asks, if it is set to).
  const url = URL.createObjectURL(new Blob([text], { type: "text/yaml" }));
  const a = document.createElement("a");
  a.href = url;
  a.download = suggested;
  document.body.appendChild(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(url), 10000);
  alert("Saved " + suggested + " to your downloads.\n\n" + done);
}

function openFile(file) {
  const reader = new FileReader();
  reader.onload = () => {
    const v = defaults();
    readYaml(String(reader.result).split("\n"), v);
    if (v.found === 0) {
      alert("That file has no \"Quest 64 Recompiled:\" options in it.");
      return;
    }
    if (v.name === "Player{number}") v.name = "";
    values = v;
    show();
  };
  reader.readAsText(file);
}

build();
show();
document.getElementById("name").addEventListener("input", readName);
document.getElementById("name").addEventListener("keydown", e => { if (e.key === "Enter") save(); });
document.getElementById("save").addEventListener("click", save);
document.getElementById("defaults").addEventListener("click", () => {
  readName();
  const name = values.name;
  values = defaults();
  values.name = name;
  show();
});
const fileInput = document.getElementById("file");
document.getElementById("open").addEventListener("click", () => { fileInput.value = ""; fileInput.click(); });
fileInput.addEventListener("change", () => { if (fileInput.files.length) openFile(fileInput.files[0]); });

// For testing the reader and writer without a person at the page:
// yaml-builder.html#convert=<base64 of a yaml> (or #convert=- for the
// defaults) puts what Save would write into <pre id="converted">.
if (location.hash.startsWith("#convert=")) {
  const arg = location.hash.slice(9);
  const v = defaults();
  if (arg !== "-") readYaml(decodeURIComponent(escape(atob(arg))).split("\n"), v);
  if (!v.name) v.name = "Tester";
  const pre = document.createElement("pre");
  pre.id = "converted";
  pre.textContent = buildYaml(v);
  document.body.appendChild(pre);
}
</script>
</body>
</html>
