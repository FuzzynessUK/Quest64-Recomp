#include "midi.h"

#include <algorithm>
#include <stdexcept>

namespace {
    constexpr int kit_program = 9;
    // General MIDI families (8 programs each) to Quest 64's bank, the same
    // rough guesses as mid2cseq.pl; --prog pins a channel exactly.
    constexpr int family_program[16] = {
        5,    // piano
        13,   // chromatic percussion
        27,   // organ
        26,   // guitar
        26,   // bass
        0,    // strings
        0,    // ensemble
        11,   // brass
        6,    // reed
        6,    // pipe
        8,    // synth lead
        0,    // synth pad
        0,    // synth effects
        13,   // ethnic
        9,    // percussive
        9,    // sound effects
    };

    struct Reader {
        const std::vector<uint8_t>& d;
        size_t p = 0;
        explicit Reader(const std::vector<uint8_t>& data) : d(data) {}
        uint8_t u8() {
            if (p >= d.size()) throw std::runtime_error("MIDI file is truncated");
            return d[p++];
        }
        uint16_t u16() { uint16_t hi = u8(); return static_cast<uint16_t>((hi << 8) | u8()); }
        uint32_t u32() { uint32_t hi = u16(); return (hi << 16) | u16(); }
        uint32_t vlq() {
            uint32_t v = 0;
            for (int i = 0; i < 5; i++) {
                uint8_t b = u8();
                v = (v << 7) | (b & 0x7F);
                if (!(b & 0x80)) break;
            }
            return v;
        }
    };

    struct Event {
        int64_t t;
        int ch;
        int type;
        int a;
        int b;
    };
}

void midi::convert(const std::vector<uint8_t>& data, const Options& options, cseq::Song& song, std::vector<std::string>& warnings) {
    Reader r(data);
    if (data.size() < 14 || data[0] != 'M' || data[1] != 'T' || data[2] != 'h' || data[3] != 'd') {
        throw std::runtime_error("not a MIDI file");
    }
    r.p = 4;
    uint32_t hlen = r.u32();
    r.u16();   // format
    uint16_t ntracks = r.u16();
    uint16_t division = r.u16();
    if (division & 0x8000) throw std::runtime_error("SMPTE time division is not supported");
    r.p = 8 + hlen;

    std::vector<Event> events;
    std::vector<std::pair<int64_t, uint32_t>> tempos;
    for (int tr = 0; tr < ntracks; tr++) {
        if (r.p + 8 > data.size() || data[r.p] != 'M' || data[r.p + 1] != 'T' || data[r.p + 2] != 'r' || data[r.p + 3] != 'k') {
            throw std::runtime_error("MIDI track header missing");
        }
        r.p += 4;
        uint32_t len = r.u32();
        size_t end = r.p + len;
        int64_t t = 0;
        int status = 0;
        while (r.p < end) {
            t += r.vlq();
            uint8_t b = data[r.p];
            if (b == 0xFF) {
                r.p++;
                uint8_t type = r.u8();
                uint32_t l = r.vlq();
                if (type == 0x51 && l == 3) {
                    tempos.push_back({ t, (static_cast<uint32_t>(data[r.p]) << 16) | (static_cast<uint32_t>(data[r.p + 1]) << 8) | data[r.p + 2] });
                }
                r.p += l;
                continue;
            }
            if (b == 0xF0 || b == 0xF7) {
                r.p++;
                r.p += r.vlq();
                continue;
            }
            if (b & 0x80) {
                status = b;
                r.p++;
            }
            int type = status & 0xF0;
            int ch = status & 0x0F;
            if (type == 0xC0 || type == 0xD0) {
                events.push_back({ t, ch, type, r.u8(), 0 });
            }
            else {
                int a = r.u8();
                int bb = r.u8();
                if (type == 0x90 && bb == 0) type = 0x80;
                events.push_back({ t, ch, type, a, bb });
            }
        }
        r.p = end;
    }
    std::stable_sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.t < b.t; });
    std::stable_sort(tempos.begin(), tempos.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    double scale = static_cast<double>(cseq::division) / division;
    auto tick = [scale](int64_t t) { return static_cast<int64_t>(t * scale + 0.5); };

    struct Open { int64_t start; int velocity; };
    std::map<int, Open> open;   // ch*128+key
    int64_t song_end = 0;
    int program[16];
    int out_program[16];
    for (int i = 0; i < 16; i++) { program[i] = 0; out_program[i] = -1; }
    auto mapped = [&](int ch) {
        auto pinned = options.channel_program.find(ch);
        if (pinned != options.channel_program.end()) return pinned->second;
        if (ch == 9) return kit_program;
        return family_program[(program[ch] / 8) & 15];
    };
    auto ensure_program = [&](int ch, int64_t at) {
        int p = mapped(ch);
        if (p != out_program[ch]) {
            cseq::add_program(song, ch, at, p);
            out_program[ch] = p;
        }
    };
    for (const Event& e : events) {
        int64_t t = tick(e.t);
        song_end = std::max(song_end, t);
        int k = e.ch * 128 + e.a;
        switch (e.type) {
        case 0x90: {
            auto it = open.find(k);
            if (it != open.end()) {
                cseq::add_note(song, e.ch, it->second.start, e.a + (e.ch == 9 ? 0 : options.transpose), it->second.velocity, t - it->second.start);
                open.erase(it);
            }
            ensure_program(e.ch, t);
            open[k] = { t, e.b };
            break;
        }
        case 0x80: {
            auto it = open.find(k);
            if (it != open.end()) {
                cseq::add_note(song, e.ch, it->second.start, e.a + (e.ch == 9 ? 0 : options.transpose), it->second.velocity, t - it->second.start);
                open.erase(it);
            }
            break;
        }
        case 0xB0:
            if (e.a == 7 || e.a == 10 || e.a == 91 || e.a == 64) {
                cseq::add_control(song, e.ch, t, e.a, e.b);
            }
            break;
        case 0xC0:
            program[e.ch] = e.a;
            ensure_program(e.ch, t);
            break;
        case 0xE0:
            cseq::add_bend(song, e.ch, t, e.a | (e.b << 7));
            break;
        default:
            break;
        }
    }
    for (auto& [k, o] : open) {
        cseq::add_note(song, k / 128, o.start, (k % 128) + ((k / 128) == 9 ? 0 : options.transpose), o.velocity, std::max<int64_t>(1, song_end - o.start));
    }
    song.tempo_usec = 500000;
    bool first = true;
    for (const auto& [t, usec] : tempos) {
        if (first && t == 0) {
            song.tempo_usec = usec;
        }
        else {
            cseq::add_tempo(song, tick(t), usec);
        }
        first = false;
    }
    song.length = std::max<int64_t>(song_end, 1);
    song.loop_start = options.one_shot ? -1 : 0;
    if (events.empty()) {
        warnings.push_back("the MIDI file has no events");
    }
}
