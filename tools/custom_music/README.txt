Custom Music
============

This folder is a music library. Put .seq files here (any name; the name is
what the menu shows), then in the game's Audio tab set Randomize Music to
Custom and pick a file for each track in the list that appears (Randomise
all fills every track from this folder; Clear all and Rescan folder sit
beside it). Changes take effect when the game is next launched. Layout >
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
32768 bytes (about ten minutes of dense music). The game log
custom_music.txt (next to the settings, in %LOCALAPPDATA%\Quest64Recompiled)
says what was loaded or why a file was skipped.

The two Pokemon gym leader files shipped here are examples: "GS" from a
MIDI (sequenced by Joao "Johnnyz" Buaes), "RBY (ootrs)" from a Darunia's
Joy pack.
