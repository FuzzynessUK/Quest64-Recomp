// Ocarina of Time / Majora's Mask sequence bytecode (the Zelda audio
// engine's "Audioseq" scripts, as .ootrs packs carry it) run through a
// small interpreter that records what it would play, so the result can be
// re-written as a compact sequence for Quest 64.
//
// The engine runs three kinds of script at 48 ticks ("tatums") a beat: the
// sequence script starts channels (up to 16) and sets the tempo; a channel
// script starts note layers (up to 4) and sets the instrument, volume and
// pan; a layer script is the notes themselves. Every script has a program
// counter, a four-deep call/loop stack and a 16-bit `value` register; the
// flow-control opcodes (0xF2-0xFF) are shared by all three. The opcode
// tables below are from the game's decompilation (audio_seqplayer.c); the
// commands that matter to a conversion are acted on, the rest are stepped
// over by their argument size so the script stays in sync.
//
// Looping: the packs loop by jumping back, at the sequence or channel
// level. The interpreter snapshots its whole state every tick; the first
// tick whose state has been seen before is the loop end, and the earlier
// tick with that state the loop start. A script that ends (0xFF at the
// top level of the sequence script) is a one-shot.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "cseq_writer.h"

namespace oot {
    struct Options {
        // OoT channel -> Quest 64 program, when pinned on the command line.
        std::map<int, int> channel_program;
        // (font << 8 | instrument) -> Quest 64 program, from the map file
        // or --inst.
        std::map<int, int> instrument_program;
        // The pack's soundfont (from the .meta), for the instrument map.
        int font = 0;
        // Fanfares play once even if their script would loop.
        bool one_shot = false;
        // Stop after this many ticks if nothing has looped or ended.
        int64_t max_ticks = 48 * 200 * 8;   // eight minutes at 200 bpm
        bool verbose = false;
    };

    struct Report {
        std::vector<std::string> warnings;
        // Per channel: font, instrument and the program chosen, for --list.
        struct ChannelUse {
            int channel;
            int font;
            int instrument;
            int program;
            int notes;
        };
        std::vector<ChannelUse> channels;
        int64_t loop_start = -1;
        int64_t length = 0;
    };

    // Runs the script and fills `song`. Throws std::runtime_error on a
    // script that cannot be followed at all.
    void convert(const std::vector<uint8_t>& data, const Options& options, cseq::Song& song, Report& report);
}
