// Writes Quest 64's music format: the libultra "compact sequence" (an
// N64 compressed MIDI, alCSPlayer), as the game's own 44 sequences are laid
// out. Facts verified against those files with a parser written from the
// game's reader (func_800386D0 block reader, func_800387F4 event reader):
//
//   header   u32 trackOffset[16], u32 division (480 for every game file)
//   track    <delta VLQ> <event> ...
//     9n key vel <duration VLQ>   a note; the format has no note-offs
//     Bn cc vv / Cn pp / En lo hi as MIDI (running status allowed, but any
//                                 meta clears it; this writer never relies
//                                 on it)
//     FF 51 t t t                 tempo, three raw bytes of microseconds
//     FF 2E 00 FF                 loop start (the game's files carry 00 FF)
//     FF 2D FF FF <u32 back>      loop end, forever; `back` is measured from
//                                 just after this payload to just after the
//                                 loop start
//     FF 2F                       end of track
//   A 0xFE byte is a back-reference marker (FE hi lo len, measured from the
//   marker byte itself); a literal 0xFE must be written FE FE. The loop-end
//   payload is read raw, so it must hold no 0xFE - the closing delta is
//   padded (a VLQ may carry leading 0x80s) until it does not.
//
// Every game file plays at 480 ticks per quarter note. The game's own
// sequence buffer is 0x8000 bytes, which capped a file at 32 KB; this port
// gives each sequence player a 1 MB buffer of its own instead (audio.cpp's
// quest64_audio_seq_buffer_0/1), so that is the limit now. On real hardware
// or another port, keep a file under 32768 bytes.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace cseq {
    constexpr int division = 480;
    constexpr size_t max_file_size = 1024 * 1024;
    constexpr size_t hardware_file_size = 0x8000;

    struct Event {
        int64_t tick = 0;
        // Sorting key within a tick: program and controls before notes.
        int order = 0;
        std::vector<uint8_t> bytes;   // the event without its delta
    };

    struct Track {
        std::vector<Event> events;
    };

    struct Song {
        Track tracks[16];
        int64_t length = 0;          // ticks; the loop end, or the end of a one-shot
        int64_t loop_start = -1;     // -1: no loop
        uint32_t tempo_usec = 500000;   // the tempo at tick 0
    };

    std::vector<uint8_t> vlq(uint64_t value);

    void add_program(Song& song, int channel, int64_t tick, int program);
    void add_control(Song& song, int channel, int64_t tick, int controller, int value);
    void add_bend(Song& song, int channel, int64_t tick, int value14);   // 0..16383, 8192 centre
    void add_note(Song& song, int channel, int64_t tick, int key, int velocity, int64_t duration);
    void add_tempo(Song& song, int64_t tick, uint32_t usec);

    // Serialises the song. Tracks with no events are left out of the header.
    std::vector<uint8_t> write(const Song& song);

    // The size, note count and tick length as a one-line summary.
    std::string summary(const Song& song, const std::vector<uint8_t>& file);
}
