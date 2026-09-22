Custom Music
============

This folder is a music library. Put .seq files here (any name; the name is
what the menu shows), then in the game's Audio tab set Randomize Music to On and
pick a file for each track in the list that appears, or press one of the
Randomise all buttons: Custom only fills every track from the folders,
Game's own shuffles the game's own music between its slots, Mix draws
each track from either. Track choices apply straight away; Preview on a row plays the track now.

A second folder, "fanfares" next to the game, is the pool for the jingles
(victory fanfare, death, the two chimes): their rows pick from it, and Randomise all
draws them from it (they are left alone when it is empty). Put one-shot
pieces there - a looping song on those two would never end. q64music puts
a pack whose .meta says "fanfare" there by itself when converting a folder. Layout >
Notifications > Show song name announces each track as it starts.

Converting music: q64music.exe next to the game
-----------------------------------------------

    q64music "song.ootrs"                converts a Darunia's Joy / OoT Randomizer pack
    q64music "song.mid"                  converts a standard MIDI file
    q64music "song.ootrs" -d custom_music   ... straight into this folder
    q64music "C:\Music\packs"             a whole folder (subfolders too) into
                                         this custom_music folder in one go
    q64music "song.ootrs" --list         shows the channels and instruments only

Quest 64 has only its own 29 instruments (no names; 9 is the drum kit), so
every OoT or MIDI instrument is mapped to one of them: q64music.map beside
the exe holds the defaults, a first guess by instrument type, and you tune
a song by ear with

    --prog 0=21,2=26     pin channel 0 to program 21 and channel 2 to 26
    --inst 0x3:14=8      map soundfont 3, instrument 14, to program 8 (or edit the map)
    --transpose -12      drop everything an octave

Programs that sustain (strings, winds, pads, organ): 0 1 2 3 4 5 6 10 11 15
19 20 21 22 23 28. Programs that decay (piano, harp, plucks, mallets): 7 8
12 13 14 16 17 18 25 27. Bass: 26. Drums: 9. The game's own field theme uses
4 5 0 6 7 9; the battle theme 21 0 18 25 23 19 7 9.

A pack's bundled soundfont (.zbank) cannot be used; a file may be at most
1 MB. (The game's own sequence buffer is 32768 bytes, which used to be the
limit; this port gives each sequence player a 1 MB buffer of its own
instead. Keep a file under 32768 bytes if it has to run on hardware or in
another port.) The game log
custom_music.txt (next to the settings, in %LOCALAPPDATA%\Quest64Recompiled)
says what was loaded or why a file was skipped.

The two Pokemon gym leader files shipped here are examples: "GS" from a
MIDI (sequenced by Joao "Johnnyz" Buaes), "RBY (ootrs)" from a Darunia's
Joy pack.
