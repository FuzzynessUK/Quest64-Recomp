#include "cseq_writer.h"

#include <algorithm>
#include <cstdio>

namespace {
    // The tempo track. The game's files keep the tempo on their first
    // track; the player takes it from any, and a track of its own keeps the
    // channel tracks simple.
    constexpr int tempo_track = 15;

    void push(cseq::Track& track, int64_t tick, int order, std::vector<uint8_t> bytes) {
        track.events.push_back({ tick, order, std::move(bytes) });
    }

    // Stream bytes the block reader sees: a literal 0xFE is doubled.
    void append_escaped(std::vector<uint8_t>& out, const std::vector<uint8_t>& bytes) {
        for (uint8_t b : bytes) {
            out.push_back(b);
            if (b == 0xFE) {
                out.push_back(0xFE);
            }
        }
    }
}

std::vector<uint8_t> cseq::vlq(uint64_t value) {
    std::vector<uint8_t> out;
    out.push_back(static_cast<uint8_t>(value & 0x7F));
    while ((value >>= 7) != 0) {
        out.insert(out.begin(), static_cast<uint8_t>((value & 0x7F) | 0x80));
    }
    return out;
}

void cseq::add_program(Song& song, int channel, int64_t tick, int program) {
    push(song.tracks[channel], tick, 0, { static_cast<uint8_t>(0xC0 | channel), static_cast<uint8_t>(program & 0x7F) });
}

void cseq::add_control(Song& song, int channel, int64_t tick, int controller, int value) {
    push(song.tracks[channel], tick, 1,
         { static_cast<uint8_t>(0xB0 | channel), static_cast<uint8_t>(controller & 0x7F), static_cast<uint8_t>(std::clamp(value, 0, 127)) });
}

void cseq::add_bend(Song& song, int channel, int64_t tick, int value14) {
    value14 = std::clamp(value14, 0, 16383);
    push(song.tracks[channel], tick, 1,
         { static_cast<uint8_t>(0xE0 | channel), static_cast<uint8_t>(value14 & 0x7F), static_cast<uint8_t>((value14 >> 7) & 0x7F) });
}

void cseq::add_note(Song& song, int channel, int64_t tick, int key, int velocity, int64_t duration) {
    if (duration < 1) {
        duration = 1;
    }
    std::vector<uint8_t> bytes = { static_cast<uint8_t>(0x90 | channel), static_cast<uint8_t>(std::clamp(key, 0, 127)),
                                   static_cast<uint8_t>(std::clamp(velocity, 1, 127)) };
    std::vector<uint8_t> d = vlq(static_cast<uint64_t>(duration));
    bytes.insert(bytes.end(), d.begin(), d.end());
    push(song.tracks[channel], tick, 2, std::move(bytes));
}

void cseq::add_tempo(Song& song, int64_t tick, uint32_t usec) {
    push(song.tracks[tempo_track], tick, 0,
         { 0xFF, 0x51, static_cast<uint8_t>(usec >> 16), static_cast<uint8_t>(usec >> 8), static_cast<uint8_t>(usec) });
}

std::vector<uint8_t> cseq::write(const Song& song) {
    std::vector<std::vector<uint8_t>> tracks(16);
    for (int t = 0; t < 16; t++) {
        std::vector<Event> events = song.tracks[t].events;
        if (t == tempo_track) {
            // The first tempo sits at tick 0 in every case.
            events.insert(events.begin(), Event{ 0, -1, { 0xFF, 0x51, static_cast<uint8_t>(song.tempo_usec >> 16),
                                                           static_cast<uint8_t>(song.tempo_usec >> 8), static_cast<uint8_t>(song.tempo_usec) } });
        }
        if (events.empty()) {
            continue;
        }
        std::stable_sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
            return a.tick != b.tick ? a.tick < b.tick : a.order < b.order;
        });

        std::vector<uint8_t>& out = tracks[t];
        int64_t last = 0;
        size_t loop_resume = 0;   // byte offset just after the loop start marker
        bool loop_written = false;
        auto emit = [&](int64_t tick, const std::vector<uint8_t>& bytes) {
            std::vector<uint8_t> d = vlq(static_cast<uint64_t>(tick - last));
            out.insert(out.end(), d.begin(), d.end());
            append_escaped(out, bytes);
            last = tick;
        };
        auto emit_loop_start = [&]() {
            std::vector<uint8_t> d = vlq(static_cast<uint64_t>(song.loop_start - last));
            out.insert(out.end(), d.begin(), d.end());
            out.insert(out.end(), { 0xFF, 0x2E, 0x00, 0xFF });
            last = song.loop_start;
            loop_resume = out.size();
            loop_written = true;
        };
        for (const Event& e : events) {
            if (e.tick >= song.length) {
                break;
            }
            // Programs and controls that sit exactly on the loop point go
            // before the marker, so they are not repeated; notes after it.
            if (song.loop_start >= 0 && !loop_written && (e.tick > song.loop_start || (e.tick == song.loop_start && e.order >= 2))) {
                emit_loop_start();
            }
            emit(e.tick, e.bytes);
        }
        if (song.loop_start >= 0 && !loop_written) {
            emit_loop_start();
        }
        // Close: the delta to the end, the loop end, end of track.
        std::vector<uint8_t> delta = vlq(static_cast<uint64_t>(song.length - last));
        if (song.loop_start >= 0) {
            while (true) {
                uint32_t back = static_cast<uint32_t>(out.size() + delta.size() + 2 + 6 - loop_resume);
                uint8_t payload[6] = { 0xFF, 0xFF, static_cast<uint8_t>(back >> 24), static_cast<uint8_t>(back >> 16),
                                       static_cast<uint8_t>(back >> 8), static_cast<uint8_t>(back) };
                bool has_fe = std::find(payload, payload + 6, 0xFE) != payload + 6;
                if (!has_fe) {
                    out.insert(out.end(), delta.begin(), delta.end());
                    out.insert(out.end(), { 0xFF, 0x2D });
                    out.insert(out.end(), payload, payload + 6);
                    break;
                }
                delta.insert(delta.begin(), 0x80);   // one byte longer, same value
            }
            out.insert(out.end(), { 0x00, 0xFF, 0x2F });
        }
        else {
            out.insert(out.end(), delta.begin(), delta.end());
            out.insert(out.end(), { 0xFF, 0x2F });
        }
    }

    std::vector<uint8_t> file(17 * 4, 0);
    uint32_t offset = 17 * 4;
    for (int t = 0; t < 16; t++) {
        if (tracks[t].empty()) {
            continue;
        }
        file[t * 4 + 0] = static_cast<uint8_t>(offset >> 24);
        file[t * 4 + 1] = static_cast<uint8_t>(offset >> 16);
        file[t * 4 + 2] = static_cast<uint8_t>(offset >> 8);
        file[t * 4 + 3] = static_cast<uint8_t>(offset);
        offset += static_cast<uint32_t>(tracks[t].size());
    }
    file[64] = 0;
    file[65] = 0;
    file[66] = static_cast<uint8_t>(division >> 8);
    file[67] = static_cast<uint8_t>(division);
    for (int t = 0; t < 16; t++) {
        file.insert(file.end(), tracks[t].begin(), tracks[t].end());
    }
    return file;
}

std::string cseq::summary(const Song& song, const std::vector<uint8_t>& file) {
    size_t notes = 0;
    int used = 0;
    for (int t = 0; t < 15; t++) {
        bool any = false;
        for (const Event& e : song.tracks[t].events) {
            if ((e.bytes[0] & 0xF0) == 0x90) {
                notes++;
                any = true;
            }
        }
        used += any ? 1 : 0;
    }
    double beats = static_cast<double>(song.length) / division;
    double seconds = beats * song.tempo_usec / 1e6;
    char buf[256];
    std::snprintf(buf, sizeof buf, "%zu bytes, %d channel(s), %zu notes, %.1f s at the opening tempo%s%s",
                  file.size(), used, notes, seconds,
                  song.loop_start >= 0 ? ", loops" : ", plays once",
                  file.size() > max_file_size ? " - TOO BIG for the game's 32768-byte buffer" : "");
    return buf;
}
