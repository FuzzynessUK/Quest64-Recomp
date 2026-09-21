#include "oot_seq.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <unordered_map>

namespace {
    constexpr int tatums_per_beat = 48;
    constexpr int tick_scale = cseq::division / tatums_per_beat;   // 10
    constexpr int midi_offset = 21;   // OoT note 0 is MIDI 21 (A0)

    // Quest 64's bank: 29 programs, no percussion bank; 9 is a drum kit on
    // General MIDI key numbers. Melodic instruments not in the map file
    // cycle through the programs the game's own battle theme uses.
    constexpr int kit_program = 9;
    constexpr int default_programs[] = { 21, 23, 0, 19, 25, 18, 7, 5 };
    constexpr int oot_drum_instrument = 0x7F;
    constexpr int oot_sfx_instrument = 0x7E;

    struct Script {
        uint32_t pc = 0;
        uint32_t stack[4] = {};
        uint8_t loops[4] = {};
        int depth = 0;
        int16_t value = 0;
    };

    struct Layer {
        bool enabled = false;
        Script s;
        int64_t delay = 0;
        int transpose = 0;
        int last_delay = 0;
        int gate = 0x80;
        int velocity = 0;        // small-note velocity (0xC1)
        int instrument = -1;     // -1: the channel's
        int bend = 0;            // semitones, rounded, from 0xCE
        int pan = -1;
        int note_index = -1;     // the note this layer is sounding, in Notes
    };

    struct Channel {
        bool enabled = false;
        Script s;
        int64_t delay = 0;
        bool large_notes = false;
        int font = 0;
        int instrument = 0;
        int volume = 127;
        int pan = 64;
        int reverb = 0;
        int transpose = 0;
        int bend = 0;            // -127..127
        Layer layers[4];
        // What the compact sequence's channel has been told so far.
        int out_program = -1;
        int out_volume = -1;
        int out_pan = -1;
        int out_reverb = -1;
        int out_bend14 = -1;
    };

    struct Sequence {
        Script s;
        int64_t delay = 0;
        bool finished = false;
        int tempo = 120;
        int volume = 127;
        int transpose = 0;
        Channel ch[16];
    };

    struct Note {
        int channel;
        int64_t start;
        int64_t end;
        int key;
        int velocity;
        int program;
    };

    struct Machine {
        const std::vector<uint8_t>& data;
        const oot::Options& options;
        oot::Report& report;
        cseq::Song& song;
        Sequence seq;
        std::vector<Note> notes;
        int64_t tick = 0;
        int tempo_out = -1;
        std::array<int, 16> channel_notes{};
        std::array<int, 16> channel_font{};
        std::array<int, 16> channel_instrument{};
        std::array<int, 16> channel_program{};
        std::map<std::string, int> warned;

        Machine(const std::vector<uint8_t>& d, const oot::Options& o, oot::Report& r, cseq::Song& s)
            : data(d), options(o), report(r), song(s) {
            for (Channel& ch : seq.ch) ch.font = o.font;
        }

        void warn(const std::string& text) {
            if (warned[text]++ == 0) {
                report.warnings.push_back(text);
            }
        }

        // ---- script reading
        uint8_t u8(Script& s) {
            if (s.pc >= data.size()) {
                throw std::runtime_error("script ran off the end of the file");
            }
            return data[s.pc++];
        }
        int8_t s8(Script& s) { return static_cast<int8_t>(u8(s)); }
        uint16_t u16(Script& s) {
            uint16_t hi = u8(s);
            return static_cast<uint16_t>((hi << 8) | u8(s));
        }
        int compressed(Script& s) {
            int v = u8(s);
            if (v & 0x80) {
                v = ((v & 0x7F) << 8) | u8(s);
            }
            return v;
        }

        // The flow control shared by the three script kinds. Returns 1 if
        // the script ended, 0 to keep going.
        int flow(Script& s, uint8_t cmd) {
            switch (cmd) {
            case 0xFF:
                if (s.depth == 0) {
                    return 1;
                }
                s.pc = s.stack[--s.depth];
                return 0;
            case 0xFC: {   // call
                uint16_t addr = u16(s);
                if (s.depth >= 4) {
                    warn("call stack overflow; call ignored");
                    return 0;
                }
                s.stack[s.depth++] = s.pc;
                s.pc = addr;
                return 0;
            }
            case 0xFB:   // jump
                s.pc = u16(s);
                return 0;
            case 0xFA: {   // beqz
                uint16_t addr = u16(s);
                if (s.value == 0) s.pc = addr;
                return 0;
            }
            case 0xF9: {   // bltz
                uint16_t addr = u16(s);
                if (s.value < 0) s.pc = addr;
                return 0;
            }
            case 0xF5: {   // bgez
                uint16_t addr = u16(s);
                if (s.value >= 0) s.pc = addr;
                return 0;
            }
            case 0xF8: {   // loop
                uint8_t count = u8(s);
                if (s.depth >= 4) {
                    warn("loop stack overflow; loop ignored");
                    return 0;
                }
                s.loops[s.depth] = count;
                s.stack[s.depth++] = s.pc;
                return 0;
            }
            case 0xF7:   // loopend
                if (s.depth == 0) {
                    return 0;
                }
                s.loops[s.depth - 1]--;
                if (s.loops[s.depth - 1] != 0) {
                    s.pc = s.stack[s.depth - 1];
                }
                else {
                    s.depth--;
                }
                return 0;
            case 0xF6:   // break
                if (s.depth > 0) s.depth--;
                return 0;
            case 0xF4: {   // rjump
                int8_t rel = s8(s);
                s.pc = static_cast<uint32_t>(static_cast<int32_t>(s.pc) + rel);
                return 0;
            }
            case 0xF3: {   // rbeqz
                int8_t rel = s8(s);
                if (s.value == 0) s.pc = static_cast<uint32_t>(static_cast<int32_t>(s.pc) + rel);
                return 0;
            }
            case 0xF2: {   // rbltz
                int8_t rel = s8(s);
                if (s.value < 0) s.pc = static_cast<uint32_t>(static_cast<int32_t>(s.pc) + rel);
                return 0;
            }
            default:
                return 0;
            }
        }

        // ---- output helpers
        int64_t out_tick() const { return tick * tick_scale; }

        void end_note(Layer& layer) {
            if (layer.note_index >= 0) {
                Note& n = notes[layer.note_index];
                if (n.end > tick) {
                    n.end = tick;
                }
                layer.note_index = -1;
            }
        }

        void disable_layer(Layer& layer) {
            end_note(layer);
            layer.enabled = false;
        }

        void disable_channel(Channel& ch) {
            for (Layer& l : ch.layers) {
                disable_layer(l);
            }
            ch.enabled = false;
        }

        void init_channel(Channel& ch) {
            disable_channel(ch);
            Channel fresh;
            fresh.font = options.font;
            fresh.out_program = ch.out_program;
            fresh.out_volume = ch.out_volume;
            fresh.out_pan = ch.out_pan;
            fresh.out_reverb = ch.out_reverb;
            fresh.out_bend14 = ch.out_bend14;
            ch = fresh;
        }

        void start_channel(int index, uint32_t pc) {
            Channel& ch = seq.ch[index];
            disable_channel(ch);
            ch.enabled = true;
            ch.s = Script{};
            ch.s.pc = pc;
            ch.delay = 0;
        }

        void start_layer(Channel& ch, int index, uint32_t pc) {
            Layer& l = ch.layers[index];
            disable_layer(l);
            l = Layer{};
            l.enabled = true;
            l.s.pc = pc;
        }

        int program_for(int channel, int font, int instrument) {
            auto pinned = options.channel_program.find(channel);
            if (pinned != options.channel_program.end()) {
                return pinned->second;
            }
            if (instrument == oot_drum_instrument) {
                return kit_program;
            }
            auto mapped = options.instrument_program.find((font << 8) | instrument);
            if (mapped != options.instrument_program.end()) {
                return mapped->second;
            }
            return default_programs[instrument % (sizeof default_programs / sizeof default_programs[0])];
        }

        void sync_channel(int index) {
            Channel& ch = seq.ch[index];
            int64_t t = out_tick();
            int volume = std::clamp(ch.volume * seq.volume / 127, 0, 127);
            if (volume != ch.out_volume) {
                cseq::add_control(song, index, t, 7, volume);
                ch.out_volume = volume;
            }
            if (ch.pan != ch.out_pan) {
                cseq::add_control(song, index, t, 10, ch.pan);
                ch.out_pan = ch.pan;
            }
            if (ch.reverb != ch.out_reverb) {
                cseq::add_control(song, index, t, 91, ch.reverb);
                ch.out_reverb = ch.reverb;
            }
            // OoT's bend spans an octave each way; the compact player's
            // wheel two semitones. Whole semitones go into the note number
            // (bend_semitones), the remainder to the wheel.
            double semis = ch.bend * 12.0 / 127.0;
            double frac = semis - std::round(semis);
            int bend14 = std::clamp(static_cast<int>(std::lround(8192 + frac / 2.0 * 8192)), 0, 16383);
            if (bend14 != ch.out_bend14) {
                cseq::add_bend(song, index, t, bend14);
                ch.out_bend14 = bend14;
            }
        }

        int bend_semitones(const Channel& ch) const {
            return static_cast<int>(std::round(ch.bend * 12.0 / 127.0));
        }

        void sync_tempo() {
            if (seq.tempo != tempo_out) {
                int bpm = std::clamp(seq.tempo, 1, 300);
                uint32_t usec = static_cast<uint32_t>(60000000.0 / bpm);
                if (tick == 0) {
                    song.tempo_usec = usec;
                }
                else {
                    cseq::add_tempo(song, out_tick(), usec);
                }
                tempo_out = seq.tempo;
            }
        }

        // ---- the sequence script
        void run_sequence() {
            Script& s = seq.s;
            while (true) {
                uint8_t cmd = u8(s);
                if (cmd >= 0xF2) {
                    if (cmd == 0xFD) {   // delay
                        seq.delay = compressed(s);
                        return;
                    }
                    if (cmd == 0xFE) {   // delay1
                        seq.delay = 1;
                        return;
                    }
                    if (flow(s, cmd)) {
                        seq.finished = true;
                        for (Channel& ch : seq.ch) disable_channel(ch);
                        return;
                    }
                    continue;
                }
                if (cmd >= 0xC0) {
                    switch (cmd) {
                    case 0xF1: u8(s); break;                  // reservenotes
                    case 0xF0: break;                         // unreservenotes
                    case 0xDF: seq.transpose = s8(s); break;
                    case 0xDE: seq.transpose += s8(s); break;
                    case 0xDD: seq.tempo = u8(s); sync_tempo(); break;
                    case 0xDC: seq.tempo += s8(s); sync_tempo(); break;
                    case 0xDB: seq.volume = u8(s); for (int i = 0; i < 16; i++) if (seq.ch[i].enabled) sync_channel(i); break;
                    case 0xDA: u8(s); u16(s); break;          // fade
                    case 0xD9: u8(s); break;                  // volscale
                    case 0xD7: {                              // initchan
                        uint16_t mask = u16(s);
                        for (int i = 0; i < 16; i++) if (mask & (1 << i)) init_channel(seq.ch[i]);
                        break;
                    }
                    case 0xD6: {                              // freechan
                        uint16_t mask = u16(s);
                        for (int i = 0; i < 16; i++) if (mask & (1 << i)) disable_channel(seq.ch[i]);
                        break;
                    }
                    case 0xD5: u8(s); break;                  // mutescale
                    case 0xD4: break;                         // mute
                    case 0xD3: u8(s); break;                  // mutebhv
                    case 0xD2: u16(s); break;                 // ldshortvel
                    case 0xD1: u16(s); break;                 // ldshortgate
                    case 0xD0: u8(s); break;                  // noteallocpolicy
                    case 0xCE: u8(s); break;
                    case 0xCD: u16(s); break;
                    case 0xCC: s.value = u8(s); break;        // ldi
                    case 0xC9: s.value &= u8(s); break;       // bitand
                    case 0xC8: s.value -= u8(s); break;       // sub
                    case 0xC7: u8(s); u16(s); break;          // stseq
                    case 0xC6: break;
                    case 0xC5: u16(s); break;                 // scriptctr
                    case 0xC4: u8(s); u8(s); warn("sequence 0xC4 runseq is not supported"); break;
                    default:
                        warn("unknown sequence command 0x" + hex(cmd));
                        break;
                    }
                    continue;
                }
                int low = cmd & 0x0F;
                switch (cmd & 0xF0) {
                case 0x00: s.value = seq.ch[low].enabled ? 1 : 0; break;   // testchan
                case 0x10: break;
                case 0x50: s.value = static_cast<int16_t>(s.value - 0); break;   // subvar (io): io not modelled
                case 0x60: u8(s); break;
                case 0x70: break;                                          // stio
                case 0x80: s.value = 0; break;                             // ldio
                case 0x90: start_channel(low, u16(s)); break;              // ldchan
                case 0xA0: {                                               // rldchan
                    int16_t rel = static_cast<int16_t>(u16(s));
                    start_channel(low, static_cast<uint32_t>(static_cast<int32_t>(s.pc) + rel));
                    break;
                }
                case 0xB0: u8(s); u16(s); warn("sequence 0xB0 ldseq is not supported"); break;
                default: warn("unknown sequence command 0x" + hex(cmd)); break;
                }
            }
        }

        // ---- a channel script
        void run_channel(int index) {
            Channel& ch = seq.ch[index];
            Script& s = ch.s;
            while (true) {
                uint8_t cmd = u8(s);
                if (cmd >= 0xF2) {
                    if (cmd == 0xFD) {
                        ch.delay = compressed(s);
                        if (ch.delay == 0) continue;
                        return;
                    }
                    if (cmd == 0xFE) {
                        ch.delay = 1;
                        return;
                    }
                    if (flow(s, cmd)) {
                        disable_channel(ch);
                        return;
                    }
                    continue;
                }
                if (cmd >= 0xC0) {
                    switch (cmd) {
                    case 0xF1: u8(s); break;
                    case 0xF0: break;
                    case 0xEF: u8(s); u16(s); break;
                    case 0xEE: u8(s); break;
                    case 0xED: u8(s); break;                  // gain
                    case 0xEC: break;                         // vibrato reset
                    case 0xEB: ch.font = u8(s); ch.instrument = u8(s); break;   // fontinstr
                    case 0xEA: disable_channel(ch); return;   // stop
                    case 0xE9: u8(s); break;                  // notepri
                    case 0xE8: for (int i = 0; i < 8; i++) u8(s); break;   // params
                    case 0xE7: u16(s); break;                 // ldparams
                    case 0xE6: u8(s); break;                  // samplebook
                    case 0xE5: u8(s); break;                  // reverbidx
                    case 0xE4: warn("channel 0xE4 dyncall is not supported"); break;
                    case 0xE3: u8(s); break;
                    case 0xE2: u8(s); break;
                    case 0xE1: u8(s); u8(s); u8(s); break;    // vibfreqgrad
                    case 0xE0: u8(s); u8(s); u8(s); break;    // vibdepthgrad
                    case 0xDF: ch.volume = u8(s); sync_channel(index); break;
                    case 0xDE: u16(s); warn("channel 0xDE freqscale is ignored"); break;
                    case 0xDD: ch.pan = u8(s) & 0x7F; sync_channel(index); break;
                    case 0xDC: u8(s); break;                  // panweight
                    case 0xDB: ch.transpose = s8(s); break;
                    case 0xDA: u16(s); break;                 // envelope
                    case 0xD9: u8(s); break;                  // releaserate
                    case 0xD8: u8(s); break;                  // vibdepth
                    case 0xD7: u8(s); break;                  // vibfreq
                    case 0xD6: u8(s); break;                  // vibdelay
                    case 0xD5: u8(s); break;
                    case 0xD4: ch.reverb = u8(s) & 0x7F; sync_channel(index); break;
                    case 0xD3: ch.bend = s8(s); sync_channel(index); break;
                    case 0xD2: u8(s); break;                  // sustain
                    case 0xD1: u8(s); break;                  // notealloc
                    case 0xD0: u8(s); break;                  // stereo
                    case 0xCF: u16(s); break;                 // stptrtoseq
                    case 0xCE: u16(s); break;                 // ldptr
                    case 0xCD: u8(s); break;
                    case 0xCC: s.value = u8(s); break;        // ldi
                    case 0xCB: {                              // ldseq: value = table[value]
                        uint16_t addr = u16(s);
                        size_t at = static_cast<size_t>(addr) + static_cast<size_t>(static_cast<uint8_t>(s.value));
                        s.value = at < data.size() ? data[at] : 0;
                        break;
                    }
                    case 0xCA: u8(s); break;                  // mutebhv
                    case 0xC9: s.value &= u8(s); break;
                    case 0xC8: s.value -= u8(s); break;
                    case 0xC7: u8(s); u16(s); break;          // stseq
                    case 0xC6: ch.font = u8(s); break;        // font
                    case 0xC5: u16(s); break;                 // dyntbl
                    case 0xC4: ch.large_notes = true; break;
                    case 0xC3: ch.large_notes = false; break;
                    case 0xC2: break;                         // dyntblv
                    case 0xC1: ch.instrument = u8(s); break;  // instr
                    case 0xC0: break;
                    default: warn("unknown channel command 0x" + hex(cmd)); break;
                    }
                    continue;
                }
                int low = cmd & 0x07;
                switch (cmd & 0xF8) {
                case 0x00: s.value = ch.layers[low & 3].enabled ? 0 : 1; break;   // testlayer
                case 0x08: break;
                case 0x10: break;
                case 0x18: break;
                case 0x20: start_channel(low, u16(s)); break;   // ldchan (from a channel)
                case 0x28: u16(s); break;
                case 0x30: u8(s); break;                        // stcio
                case 0x38: u8(s); break;
                case 0x40: u8(s); break;                        // ldcio
                case 0x48: u8(s); break;
                case 0x50: break;                               // subio
                case 0x58: break;
                case 0x60: s.value = 0; break;                  // ldio
                case 0x68: break;
                case 0x70: break;                               // stio
                case 0x78: {                                    // rldlayer
                    int16_t rel = static_cast<int16_t>(u16(s));
                    start_layer(ch, low & 3, static_cast<uint32_t>(static_cast<int32_t>(s.pc) + rel));
                    break;
                }
                case 0x80: s.value = 0; break;
                case 0x88: start_layer(ch, low & 3, u16(s)); break;   // ldlayer
                case 0x90: disable_layer(ch.layers[low & 3]); break;   // dellayer
                case 0x98: warn("channel 0x98 dynldlayer is not supported"); break;
                default: warn("unknown channel command 0x" + hex(cmd)); break;
                }
            }
        }

        // ---- a layer script
        void run_layer(int channel_index, int layer_index) {
            Channel& ch = seq.ch[channel_index];
            Layer& layer = ch.layers[layer_index];
            Script& s = layer.s;
            while (true) {
                uint8_t cmd = u8(s);
                if (cmd >= 0xF2) {
                    if (cmd == 0xFD) {   // delay: a rest
                        end_note(layer);
                        layer.delay = compressed(s);
                        if (layer.delay == 0) continue;
                        return;
                    }
                    if (cmd == 0xFE) {
                        end_note(layer);
                        layer.delay = 1;
                        return;
                    }
                    if (flow(s, cmd)) {
                        disable_layer(layer);
                        return;
                    }
                    continue;
                }
                if (cmd >= 0xC0) {
                    switch (cmd) {
                    case 0xC0:   // ldelay: a rest
                        end_note(layer);
                        layer.delay = compressed(s);
                        if (layer.delay == 0) continue;
                        return;
                    case 0xC1: layer.velocity = u8(s); break;          // shortvel
                    case 0xC2: layer.transpose = s8(s); break;
                    case 0xC3: layer.last_delay = compressed(s); break; // shortdelay
                    case 0xC4: break;                                   // legato on
                    case 0xC5: break;                                   // legato off
                    case 0xC6: layer.instrument = u8(s); break;
                    case 0xC7: {                                        // portamento
                        uint8_t mode = u8(s);
                        u8(s);
                        if (mode & 0x80) u8(s); else compressed(s);
                        break;
                    }
                    case 0xC8: break;                                   // portamento off
                    case 0xC9: layer.gate = u8(s); break;               // shortgate
                    case 0xCA: layer.pan = u8(s) & 0x7F; break;         // notepan
                    case 0xCB: u16(s); u8(s); break;                    // env
                    case 0xCC: u8(s); break;                            // drumpan
                    case 0xCD: u8(s); break;                            // stereo
                    case 0xCE: layer.bend = static_cast<int>(std::round(s8(s) * 12.0 / 127.0)); break;
                    case 0xCF: u8(s); break;                            // releaserate
                    case 0xF1: u8(s); break;
                    case 0xF0: break;
                    default: warn("unknown layer command 0x" + hex(cmd)); break;
                    }
                    continue;
                }
                // A note.
                int delay;
                int velocity;
                int gate;
                if (ch.large_notes) {
                    switch (cmd & 0xC0) {
                    case 0x00:
                        delay = compressed(s);
                        velocity = u8(s);
                        gate = u8(s);
                        layer.last_delay = delay;
                        break;
                    case 0x40:
                        delay = compressed(s);
                        velocity = u8(s);
                        gate = 0;
                        layer.last_delay = delay;
                        break;
                    default:
                        delay = layer.last_delay;
                        velocity = u8(s);
                        gate = u8(s);
                        break;
                    }
                }
                else {
                    switch (cmd & 0xC0) {
                    case 0x00:
                        delay = compressed(s);
                        layer.last_delay = delay;
                        break;
                    case 0x40:
                        delay = compressed(s);
                        layer.last_delay = delay;
                        break;
                    default:
                        delay = layer.last_delay;
                        break;
                    }
                    velocity = layer.velocity;
                    gate = layer.gate;
                }
                if (velocity > 127 || velocity < 0) velocity = 127;
                int note = cmd & 0x3F;
                end_note(layer);
                layer.delay = delay;
                int64_t gate_delay = (static_cast<int64_t>(gate) * delay) >> 8;
                int64_t sounding = delay - gate_delay;
                int instrument = layer.instrument >= 0 ? layer.instrument : ch.instrument;
                if (instrument == oot_sfx_instrument) {
                    if (delay == 0) continue;
                    return;
                }
                int key;
                if (instrument == oot_drum_instrument) {
                    key = 35 + (note % 47);   // the kit's GM keys, a guess per drum
                }
                else {
                    key = note + midi_offset + ch.transpose + layer.transpose + seq.transpose + bend_semitones(ch) + layer.bend;
                }
                int program = program_for(channel_index, ch.font, instrument);
                if (program != ch.out_program) {
                    cseq::add_program(song, channel_index, out_tick(), program);
                    ch.out_program = program;
                }
                if (ch.out_volume < 0 || ch.out_pan < 0) {
                    sync_channel(channel_index);
                }
                if (sounding > 0 && velocity > 0) {
                    notes.push_back({ channel_index, tick, tick + sounding, key, velocity, program });
                    layer.note_index = static_cast<int>(notes.size()) - 1;
                    channel_notes[channel_index]++;
                    channel_font[channel_index] = ch.font;
                    channel_instrument[channel_index] = instrument;
                    channel_program[channel_index] = program;
                }
                if (delay == 0) continue;
                return;
            }
        }

        static std::string hex(int v) {
            char buf[8];
            std::snprintf(buf, sizeof buf, "%02X", v);
            return buf;
        }

        // ---- one tick of everything
        void step() {
            if (!seq.finished) {
                if (seq.delay > 1) {
                    seq.delay--;
                }
                else {
                    seq.delay = 0;
                    run_sequence();
                }
            }
            for (int i = 0; i < 16; i++) {
                Channel& ch = seq.ch[i];
                if (!ch.enabled) continue;
                if (ch.delay > 0) {
                    ch.delay--;
                }
                if (ch.delay == 0) {
                    run_channel(i);
                }
                if (!ch.enabled) continue;
                for (int l = 0; l < 4; l++) {
                    Layer& layer = ch.layers[l];
                    if (!layer.enabled) continue;
                    if (layer.delay > 0) {
                        layer.delay--;
                    }
                    if (layer.delay == 0 && layer.enabled) {
                        run_layer(i, l);
                    }
                }
            }
        }

        bool anything_running() const {
            if (!seq.finished) return true;
            for (const Channel& ch : seq.ch) {
                if (ch.enabled) return true;
            }
            return false;
        }

        // The state that decides everything that follows, as bytes.
        std::string snapshot() const {
            std::string out;
            auto put = [&out](int64_t v) { out.append(reinterpret_cast<const char*>(&v), sizeof v); };
            auto put_script = [&](const Script& s) {
                put(s.pc); put(s.depth); put(s.value);
                for (int i = 0; i < s.depth; i++) { put(s.stack[i]); put(s.loops[i]); }
            };
            put_script(seq.s);
            put(seq.delay); put(seq.finished); put(seq.tempo); put(seq.volume); put(seq.transpose);
            for (const Channel& ch : seq.ch) {
                put(ch.enabled);
                if (!ch.enabled) continue;
                put_script(ch.s);
                put(ch.delay); put(ch.large_notes); put(ch.font); put(ch.instrument); put(ch.volume);
                put(ch.pan); put(ch.reverb); put(ch.transpose); put(ch.bend);
                for (const Layer& l : ch.layers) {
                    put(l.enabled);
                    if (!l.enabled) continue;
                    put_script(l.s);
                    put(l.delay); put(l.transpose); put(l.last_delay); put(l.gate); put(l.velocity);
                    put(l.instrument); put(l.bend); put(l.pan);
                }
            }
            return out;
        }
    };
}

void oot::convert(const std::vector<uint8_t>& data, const Options& options, cseq::Song& song, Report& report) {
    Machine m(data, options, report, song);
    std::unordered_map<std::string, int64_t> seen;
    int64_t loop_start = -1;
    int64_t end = -1;
    m.sync_tempo();
    while (true) {
        std::string state = m.snapshot();
        auto it = seen.find(state);
        if (it != seen.end() && !options.one_shot) {
            loop_start = it->second;
            end = m.tick;
            break;
        }
        seen.emplace(std::move(state), m.tick);
        m.step();
        m.tick++;
        if (!m.anything_running()) {
            // Let the last notes ring to their own ends.
            int64_t last = m.tick;
            for (const Note& n : m.notes) last = std::max(last, n.end);
            end = last;
            break;
        }
        if (m.tick >= options.max_ticks) {
            report.warnings.push_back("no loop or end found within the tick limit; cut there and looped whole");
            loop_start = 0;
            end = m.tick;
            break;
        }
    }
    if (options.one_shot && loop_start >= 0) {
        loop_start = -1;
    }

    // Notes into the song, cut at the end.
    for (const Note& n : m.notes) {
        if (n.start >= end) continue;
        int64_t stop = std::min(n.end, end);
        cseq::add_note(song, n.channel, n.start * tick_scale, n.key, n.velocity, (stop - n.start) * tick_scale);
    }
    song.length = end * tick_scale;
    song.loop_start = loop_start >= 0 ? loop_start * tick_scale : -1;
    report.loop_start = loop_start;
    report.length = end;
    for (int i = 0; i < 16; i++) {
        if (m.channel_notes[i] > 0) {
            report.channels.push_back({ i, m.channel_font[i], m.channel_instrument[i], m.channel_program[i], m.channel_notes[i] });
        }
    }
}
