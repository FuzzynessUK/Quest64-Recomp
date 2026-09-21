Custom Music
============

With "Custom Music" switched on in the Audio tab, every file in this folder
named track_NN.seq replaces the game's music track NN for that session.
Track numbers (decimal):

   2  Melrode            13  Battle             21  Holy Plains
  15  Melrode Monastery  25  Dondoran           27  Title screen
  30  Death jingle       43  Victory fanfare

The full list is the map music table in the source (DOCS/HANDOFF.md).

A .seq file is a sequence in the game's own compact format (an N64
"compressed MIDI"). tools/mid2cseq.pl in the source tree writes one from a
standard MIDI file:

    perl tools/mid2cseq.pl song.mid custom_music/track_21.seq --prog 0=21,1=23,2=0

--prog pins MIDI channels to the game's instrument programs (0-28; 9 is
the drum kit). A file may be at most 32768 bytes. The game log
custom_music.txt (next to the settings, in %LOCALAPPDATA%\Quest64Recompiled)
says what was loaded or why a file was skipped.

track_15.seq shipped with the game is the Pokemon Gold/Silver gym leader battle
theme (from a MIDI sequenced by Joao "Johnnyz" Buaes), an experiment.
