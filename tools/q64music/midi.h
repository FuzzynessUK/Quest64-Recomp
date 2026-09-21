// Standard MIDI (type 0 or 1) into a cseq::Song: the same conversion as
// tools/mid2cseq.pl, so one tool covers both inputs.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "cseq_writer.h"

namespace midi {
    struct Options {
        std::map<int, int> channel_program;   // MIDI channel (0-15) -> Quest 64 program
        bool one_shot = false;                 // otherwise the whole file loops
        int transpose = 0;
    };

    void convert(const std::vector<uint8_t>& data, const Options& options, cseq::Song& song, std::vector<std::string>& warnings);
}
