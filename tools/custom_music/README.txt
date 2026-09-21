Custom Music
============

This folder is a music library. Put .seq files here (any name; the name is
what the menu shows), then in the game's Audio tab set Custom Music to:

           each launch;
  Custom   pick a file for each track in the list that appears (with
           Randomise all / Clear all / Rescan folder buttons).

Changes take effect when the game is next launched. Layout > Notifications
> Show song name announces each track as it starts.

A .seq file is a sequence in the game's own compact format (an N64
"compressed MIDI"). tools/mid2cseq.pl in the source tree writes one from a
standard MIDI file:

    perl tools/mid2cseq.pl song.mid "custom_music/My Song.seq" --prog 0=21,1=23,2=0

--prog pins MIDI channels to the game's instrument programs (0-28; 9 is
the drum kit). A file may be at most 32768 bytes. The game log
custom_music.txt (next to the settings, in %LOCALAPPDATA%\Quest64Recompiled)
says what was loaded or why a file was skipped.

"Pokemon GS - Gym Leader Battle.seq" shipped with the game is the gym
leader theme from Pokemon Gold/Silver (MIDI sequenced by Joao "Johnnyz"
Buaes), an experiment.
