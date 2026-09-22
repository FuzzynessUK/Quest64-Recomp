#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <numeric>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include "audio.h"
#include "enhancements.h"
#include "notify.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
#include "zelda_support.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"

namespace data = merrow::data;
using zelda64::audio::CustomMusic;
using zelda64::audio::MusicShuffle;
using zelda64::audio::Options;

namespace {
    Options active;
    bool active_loaded = false;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "audio.json";
    }

    // The map music table at 0x80053B00 (ROM 0x054700): 70 six-byte rows of
    // map, submap and track, read by func_8002684C. Merrow's bgmdata lists
    // the track byte of each row plus three code-segment immediates
    // (credits, battle, Mammon) that a ROM write cannot reach here; those
    // are covered by the request hooks instead, so only the table is
    // written.
    constexpr uint32_t bgm_table_start = 0x054700;
    constexpr uint32_t bgm_table_end = 0x0548B0;

    // Tracks. Merrow's map-music pool is 0-26 and 31-41 (27-30 unused), so
    // a roll of 0-37 skips the gap, as Merrow's does; that pool is what any
    // remapped track becomes. The game itself starts tracks beyond it by
    // number - the victory fanfare is 0x2B, the Mammon fight 0x29 - so the
    // ids that can be remapped run further.
    constexpr int track_count = 64;
    constexpr int usable_tracks = 38;
    constexpr int unused_gap_start = 27;
    constexpr int unused_gap_size = 4;

    // Sound effects: the per-effect volume table at 0x80053CAC holds 84
    // entries, of which the first 70 are non-zero; the rest are unused ids.
    constexpr int sfx_count = 70;

    // Per-session tables, built at boot; identity until then.
    std::array<int8_t, track_count> bgm_remap{};
    // Track -> library file name for the tracks custom music replaced.
    std::map<int, std::string> session_songs;

    // The track the main BGM player is on: func_80026658 copies each
    // request here (0x8002673C, a byte; -1 when stopped) right before it
    // loads and starts the sequence, so a change is a start.
    constexpr int32_t bgm_current_track = 0x8008FCC0;
    int last_seen_track = -2;
    std::array<int8_t, sfx_count> sfx_remap{};

    // With the shuffle on, a looping ambience (a waterfall, say) can land on
    // any effect and then never stop, so every effect is cut three seconds
    // after its latest start. The cut is the game's own stop request:
    // func_80025B3C queues { id, volume 0, 0x40 } into the ring buffer the
    // plays go through (eight slots at 0x8005390C / 0x8005392C / 0x8005394C,
    // index at 0x80053970, mirrored to 0x8008FCB0) and the audio thread
    // stops that id. A one-shot is long over by then, so stopping it does
    // nothing; a loop ends.
    constexpr float sfx_cutoff_seconds = 3.0f;
    constexpr int32_t sfx_ring_id = 0x8005390C;
    constexpr int32_t sfx_ring_volume = 0x8005392C;
    constexpr int32_t sfx_ring_param = 0x8005394C;
    constexpr int32_t sfx_ring_index = 0x80053970;
    constexpr int32_t sfx_stop_pending = 0x8008FCB0;
    constexpr int sfx_ring_slots = 8;
    using sfx_clock = std::chrono::steady_clock;
    std::array<sfx_clock::time_point, sfx_count> sfx_started{};
    std::array<bool, sfx_count> sfx_live{};

    void queue_sfx_stop(uint8_t* rdram, int id) {
        int32_t index = static_cast<int32_t>(MEM_W(0, sfx_ring_index)) & (sfx_ring_slots - 1);
        MEM_W(0, sfx_ring_id + index * 4) = id;
        MEM_W(0, sfx_ring_volume + index * 4) = 0;
        MEM_W(0, sfx_ring_param + index * 4) = 0x40;
        index = (index + 1) & (sfx_ring_slots - 1);
        MEM_W(0, sfx_ring_index) = index;
        MEM_W(0, sfx_stop_pending) = index;
    }

    int roll_track(std::mt19937& rng) {
        int track = std::uniform_int_distribution<int>(0, usable_tracks - 1)(rng);
        if (track >= unused_gap_start) {
            track += unused_gap_size;
        }
        return track;
    }

    bool usable_track(int track) {
        return track >= 0 && track < track_count &&
            (track < unused_gap_start || track >= unused_gap_start + unused_gap_size);
    }

    // The music sequence bank: an ALSeqFile at ROM 0xEBABD0 (u16 revision,
    // u16 count, then { u32 offset, u32 len } per sequence, offsets from
    // the bank's own start). func_80025040 DMAs the table into RAM at init
    // and alSeqFileNew turns the offsets into ROM addresses; each play
    // then DMAs the sequence (func_800252D8) into the player's 0x8000-byte
    // buffer (func_8002513C), so that is the size limit. The ROM's tail
    // from 0xF94348 is 0xFF padding, which is where replacements go.
    constexpr uint32_t seq_bank_start = 0xEBABD0;
    constexpr uint32_t rom_free_start = 0xF94348;
    constexpr uint32_t seq_buffer_size = 0x8000;
    constexpr uint32_t seq_header_size = 17 * 4;

    uint32_t read_u32(const std::vector<uint8_t>& v, size_t at) {
        return (static_cast<uint32_t>(v[at]) << 24) | (static_cast<uint32_t>(v[at + 1]) << 16) |
            (static_cast<uint32_t>(v[at + 2]) << 8) | v[at + 3];
    }

    void write_u32(std::vector<uint8_t>& v, size_t at, uint32_t value) {
        v[at + 0] = static_cast<uint8_t>(value >> 24);
        v[at + 1] = static_cast<uint8_t>(value >> 16);
        v[at + 2] = static_cast<uint8_t>(value >> 8);
        v[at + 3] = static_cast<uint8_t>(value);
    }

    // ---- the library in the ROM, and live changes
    //
    // At boot every file in the folder is appended to the ROM copy (the
    // free tail first, then the ROM grows; a slack of a few MB is added so
    // files dropped in later can be placed while the game runs - the DMA
    // reader indexes the buffer with no size check, and a fixed-size buffer
    // is safe to write into from another thread). Which file a track plays
    // is then only the sequence bank's entry for it: at boot in the ROM
    // copy, and later in the RAM copy of the table that func_80025040 made
    // (pointer at 0x800538F0, entries { ROM address, length } after
    // alSeqFileNew), read by func_800252D8 every time a track starts. So
    // the menu's choices apply to the next play with an 8-byte write,
    // done on the game thread in on_frame.
    constexpr int32_t seq_table_pointer = 0x800538F0;
    constexpr size_t rom_slack = 4 * 1024 * 1024;
    constexpr int32_t bgm_request_track = 0x8008FCC1;
    constexpr int32_t bgm_request_flags = 0x8008FCC2;

    struct Placed {
        uint32_t rom;   // position in the ROM copy
        uint32_t len;
    };
    std::map<std::string, Placed> placed;
    std::array<Placed, zelda64::audio::game_track_count> original_entry{};
    uint32_t rom_cursor = 0;
    size_t rom_capacity = 0;
    bool library_in_rom = false;
    std::ofstream live_log;

    std::mutex live_mutex;
    std::map<int, std::string> pending_entries;   // track -> name, "" for the game's own
    bool pending_all = false;
    int pending_preview = -2;                     // -2 nothing, -1 stop, else a track
    int preview_return = -2;                      // the track to go back to, -2 none

    // Loads a file's bytes into the ROM copy at the cursor. `rom` is the
    // buffer to write (the boot copy, or the live buffer).
    bool place_file(uint8_t* rom, size_t capacity, const std::string& name, std::ostream& log) {
        if (placed.count(name)) {
            return placed[name].len != 0;
        }
        std::ifstream in(zelda64::audio::library_path(name), std::ios::binary);
        std::vector<uint8_t> seq((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        if (seq.size() < seq_header_size + 2 || seq.size() > seq_buffer_size) {
            log << "  " << name << ".seq: skipped, " << seq.size() << " bytes (a sequence is between "
                << (seq_header_size + 2) << " and " << seq_buffer_size << ")\n";
            placed[name] = { 0, 0 };
            return false;
        }
        for (int t = 0; t < 16; t++) {
            uint32_t offset = read_u32(seq, t * 4);
            if (offset != 0 && (offset < seq_header_size || offset >= seq.size())) {
                log << "  " << name << ".seq: skipped, a track offset points outside the file\n";
                placed[name] = { 0, 0 };
                return false;
            }
        }
        uint32_t padded = (static_cast<uint32_t>(seq.size()) + 15) & ~static_cast<uint32_t>(15);
        if (rom_cursor + padded > capacity) {
            log << "  " << name << ".seq: no room left in the ROM; relaunch to make room\n";
            return false;
        }
        std::copy(seq.begin(), seq.end(), rom + rom_cursor);
        placed[name] = { rom_cursor, static_cast<uint32_t>(seq.size()) };
        log << "  " << name << ".seq: " << seq.size() << " bytes at ROM 0x" << std::hex << rom_cursor << std::dec << "\n";
        rom_cursor += padded;
        return true;
    }

    // Which library file each track gets: the menu's choices (a name that
    // is not in the folder is reported and left as the game's own).
    std::map<int, std::string> plan_custom_music(const Options& options, std::ostream& log) {
        std::map<int, std::string> plan;
        if (options.custom_music != CustomMusic::Custom) {
            return plan;
        }
        for (const auto& [track, name] : options.custom_tracks) {
            if (track < 0 || track >= zelda64::audio::game_track_count || name.empty()) {
                continue;
            }
            auto it = placed.find(name);
            if (it == placed.end()) {
                log << "  track " << track << " (" << zelda64::audio::track_label(track) << "): \"" << name
                    << ".seq\" is not in the folder, left as the game's own\n";
                continue;
            }
            if (it->second.len != 0) {
                plan[track] = name;
            }
        }
        return plan;
    }

    // Boot: the whole library into the ROM copy, the chosen entries set.
    void apply_custom_music(std::vector<uint8_t>& patched, const Options& options) {
        std::filesystem::path folder = zelda64::audio::library_folder();
        live_log.open(zelda64::get_app_folder_path() / "custom_music.txt");
        std::ostream& log = live_log;
        log << "Custom music folder: " << folder.string() << "\n";
        std::vector<std::string> library = zelda64::audio::library_files();
        log << "  " << library.size() << " file(s) in the library\n";

        uint32_t count = (static_cast<uint32_t>(patched[seq_bank_start + 2]) << 8) | patched[seq_bank_start + 3];
        for (int track = 0; track < zelda64::audio::game_track_count && static_cast<uint32_t>(track) < count; track++) {
            uint32_t entry = seq_bank_start + 4 + static_cast<uint32_t>(track) * 8;
            original_entry[track] = { read_u32(patched, entry), read_u32(patched, entry + 4) };
        }

        // Room for every file plus slack, in whole MB past the free tail.
        size_t need = 0;
        std::error_code ec;
        for (const std::string& name : library) {
            need += (static_cast<size_t>(std::filesystem::file_size(zelda64::audio::library_path(name), ec)) + 15) & ~static_cast<size_t>(15);
        }
        rom_cursor = rom_free_start;
        size_t wanted = rom_free_start + need + rom_slack;
        if (wanted > patched.size()) {
            patched.resize(((wanted + 0xFFFFF) / 0x100000) * 0x100000, 0xFF);
            log << "  ROM grown to " << patched.size() << " bytes\n";
        }
        rom_capacity = patched.size();
        for (const std::string& name : library) {
            place_file(patched.data(), rom_capacity, name, log);
        }
        library_in_rom = true;

        std::map<int, std::string> plan = plan_custom_music(options, log);
        session_songs.clear();
        for (const auto& [track, name] : plan) {
            uint32_t entry = seq_bank_start + 4 + static_cast<uint32_t>(track) * 8;
            write_u32(patched, entry, placed[name].rom - seq_bank_start);
            write_u32(patched, entry + 4, placed[name].len);
            log << "  track " << track << " (" << zelda64::audio::track_label(track) << ") <- " << name << "\n";
            session_songs[track] = name;
        }
        if (plan.empty()) {
            log << "  nothing replaced at boot\n";
        }
        log.flush();
    }

    // Game thread: the RAM table entry for one track.
    void write_live_entry(uint8_t* rdram, int track, const std::string& name) {
        int32_t table = static_cast<int32_t>(MEM_W(0, seq_table_pointer));
        if (table == 0 || track < 0 || track >= zelda64::audio::game_track_count) {
            return;
        }
        // The table holds ROM addresses: alSeqFileNew added the bank's
        // address (0xEBABD0, the constant the boot code hands
        // func_8002513C) to each file offset. Not derived from an entry:
        // any entry, entry 0 included, may already point at a custom file.
        uint32_t base = seq_bank_start;
        Placed target = original_entry[track];
        if (!name.empty()) {
            std::span<const uint8_t> rom = recomp::get_rom();
            if (!place_file(const_cast<uint8_t*>(rom.data()), rom.size(), name, live_log)) {
                live_log.flush();
                return;
            }
            target = placed[name];
            target.rom -= seq_bank_start;
        }
        MEM_W(0, table + 4 + track * 8) = base + target.rom;
        MEM_W(0, table + 8 + track * 8) = target.len;
        if (name.empty()) {
            session_songs.erase(track);
        }
        else {
            session_songs[track] = name;
        }
        live_log << "  live: track " << track << " (" << zelda64::audio::track_label(track) << ") <- "
                 << (name.empty() ? "the game's own" : name) << "\n";
        live_log.flush();
    }

    // Game thread: UpdateBGM's own writes (0x800267B8): the request byte
    // and the pending bit, consumed by func_80026658 next frame. The game
    // ignores a request for the track already requested, so a restart of
    // the same track goes through a stop first.
    void request_track(uint8_t* rdram, int track) {
        MEM_B(0, bgm_request_track) = static_cast<int8_t>(track);
        MEM_H(0, bgm_request_flags) = static_cast<int16_t>(MEM_HU(0, bgm_request_flags) | 1);
    }
    // The 44 sequences: who plays each (map music table rows at ROM
    // 0x054700 for the areas; the by-number callers for the events: boss
    // pick 0x8001CA28 gives 0 or 0x29 for Mammon, battle 0xD, title 0x1B,
    // death 0x1E, credits 0x14, victory 0x2B on the jingle player). The
    // unnamed ones loop but no table row or immediate reaches them.
    const char* const track_labels[zelda64::audio::game_track_count] = {
        "Boss battle",                                  // 0
        "Baragoon Tunnel",                              // 1
        "Melrode",                                      // 2
        "Dondoran Castle",                              // 3
        "Track 4",                                      // 4
        "Shamwood",                                     // 5
        "Hidden rooms and shrines",                     // 6
        "Buildings (shops, inns, houses)",              // 7
        "Limelin Castle",                               // 8
        "Blue Cave (inner)",                            // 9
        "Cull Hazard",                                  // 10
        "Blue Cave (deep)",                             // 11
        "Forests (inner paths)",                        // 12
        "Battle",                                       // 13
        "Nepty's arena",                                // 14
        "Melrode Monastery",                            // 15
        "Track 16",                                     // 16
        "Track 17",                                     // 17
        "Boil Hole",                                    // 18
        "Track 19",                                     // 19
        "Credits",                                      // 20
        "Fields (Holy Plains, Dondoran Flats, West Carmagh, West Limelin)",   // 21
        "Dindom Dries",                                 // 22
        "Blue Cave",                                    // 23
        "Forests (Connor, Glencoe, Windward)",          // 24
        "Dondoran",                                     // 25
        "Isle of Skye",                                 // 26
        "Title screen",                                 // 27
        "Jingle 28",                                    // 28
        "Jingle 29",                                    // 29
        "Death",                                        // 30
        "Brannoch Castle",                              // 31
        "Track 32",                                     // 32
        "Churches, halls and Mammon's World",           // 33
        "Larapool",                                     // 34
        "Connor Forest",                                // 35
        "Limelin",                                      // 36
        "Normoon",                                      // 37
        "Mammon's World (inner)",                       // 38
        "Brannoch and Baragoon Moor",                   // 39
        "Track 40",                                     // 40
        "Mammon battle",                                // 41
        "Track 42",                                     // 42
        "Victory fanfare",                              // 43
    };

    const int track_menu_order_list[zelda64::audio::game_track_count] = {
        27, 13, 0, 41, 43, 30, 20,
        15, 2, 33, 7, 21, 25, 3, 35, 24, 12, 34, 23, 9, 11, 10, 37, 36, 8, 22, 18, 1, 39, 31, 26, 14, 5, 6, 38,
        4, 16, 17, 19, 28, 29, 32, 40, 42,
    };
}

const char* zelda64::audio::track_label(int track) {
    return track >= 0 && track < game_track_count ? track_labels[track] : "";
}

bool zelda64::audio::track_is_jingle(int track) {
    return track == 28 || track == 29 || track == 30 || track == 43;
}

const int* zelda64::audio::track_menu_order() {
    return track_menu_order_list;
}

std::filesystem::path zelda64::audio::library_folder() {
    return std::filesystem::absolute(zelda64::get_program_path() / "custom_music");
}

// Names are UTF-8 (path::u8string): the menu shows them, the settings
// file stores them (a non-UTF-8 string makes the JSON writer throw), and
// library_path turns one back into a path.
std::filesystem::path zelda64::audio::library_path(const std::string& name) {
    return library_folder() / std::filesystem::u8path(name + ".seq");
}

std::vector<std::string> zelda64::audio::library_files(int* too_big) {
    std::vector<std::string> names;
    int big = 0;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(library_folder(), ec)) {
        if (entry.is_regular_file() && entry.path().extension() == ".seq") {
            // A file the game's sequence buffer cannot hold is never
            // offered; the converter says so when it writes one.
            if (entry.file_size(ec) > seq_buffer_size) {
                big++;
                continue;
            }
            std::u8string u8 = entry.path().stem().u8string();
            names.emplace_back(u8.begin(), u8.end());
        }
    }
    std::sort(names.begin(), names.end());
    if (too_big != nullptr) {
        *too_big = big;
    }
    return names;
}

namespace {
}

Options zelda64::audio::load_options() {
    Options o;
    std::ifstream in(options_path());
    if (!in.good()) {
        return o;
    }
    nlohmann::json j;
    try {
        in >> j;
    }
    catch (nlohmann::json::parse_error&) {
        return o;
    }
    auto get = [&j](const char* key, auto& out) {
        auto it = j.find(key);
        if (it != j.end()) {
            try {
                out = it->get<std::remove_reference_t<decltype(out)>>();
            }
            catch (nlohmann::json::type_error&) {}
        }
    };
    // The first version stored music_shuffle as a bool meaning Towns.
    auto music = j.find("music_shuffle");
    if (music != j.end()) {
        if (music->is_boolean()) {
            o.music_shuffle = music->get<bool>() ? MusicShuffle::Towns : MusicShuffle::Off;
        }
        else if (music->is_number_integer()) {
            o.music_shuffle = static_cast<MusicShuffle>(std::clamp(music->get<int>(), 0, 2));
        }
    }
    get("sfx_shuffle", o.sfx_shuffle);
    // custom_music was a bool for one build (track_NN.seq files) and had a
    // per-launch shuffle (1) for another; anything but off is Custom now.
    auto custom = j.find("custom_music");
    if (custom != j.end()) {
        if (custom->is_boolean()) {
            o.custom_music = custom->get<bool>() ? CustomMusic::Custom : CustomMusic::Off;
        }
        else if (custom->is_number_integer()) {
            o.custom_music = custom->get<int>() != 0 ? CustomMusic::Custom : CustomMusic::Off;
        }
    }
    // The menu offers Custom in place of the track shuffle, not on top.
    if (o.custom_music == CustomMusic::Custom) {
        o.music_shuffle = MusicShuffle::Off;
    }
    auto tracks = j.find("custom_tracks");
    if (tracks != j.end() && tracks->is_object()) {
        for (const auto& [key, value] : tracks->items()) {
            try {
                int track = std::stoi(key);
                if (value.is_string() && track >= 0 && track < zelda64::audio::game_track_count) {
                    o.custom_tracks[track] = value.get<std::string>();
                }
            }
            catch (std::exception&) {}
        }
    }
    return o;
}

void zelda64::audio::save_options(const Options& o) {
    nlohmann::json j;
    j["music_shuffle"] = static_cast<int>(o.music_shuffle);
    j["sfx_shuffle"] = o.sfx_shuffle;
    j["custom_music"] = o.custom_music == CustomMusic::Custom ? 2 : 0;
    nlohmann::json tracks = nlohmann::json::object();
    for (const auto& [track, name] : o.custom_tracks) {
        if (!name.empty()) {
            tracks[std::to_string(track)] = name;
        }
    }
    j["custom_tracks"] = tracks;
    std::ofstream out(options_path());
    out << j.dump(4);
}

const Options& zelda64::audio::active_options() {
    if (!active_loaded) {
        active = load_options();
        active_loaded = true;
    }
    return active;
}

void zelda64::audio::apply_at_boot(uint8_t* rdram) {
    const Options& options = active_options();
    std::iota(bgm_remap.begin(), bgm_remap.end(), 0);
    std::iota(sfx_remap.begin(), sfx_remap.end(), 0);
    // The library is placed whenever the folder has files, so the menu can
    // switch tracks to it live in any mode.
    bool custom_music = !library_files().empty();
    if (options.music_shuffle == MusicShuffle::Off && !options.sfx_shuffle && !custom_music) {
        return;
    }

    std::mt19937 rng{ std::random_device{}() };

    if (options.music_shuffle != MusicShuffle::Off || custom_music) {
        // Reads back whatever the earlier boot patches left, so this stacks
        // on top of them rather than replacing them. The table is in the
        // boot segment (ROM 0x1000.., 1MB), which has already been copied to
        // RAM at 0x80000400 by now, so each byte goes to both: the ROM copy
        // alone is never read again. The sequence bank, by contrast, is
        // DMA'd from the ROM at init, so its entries need the ROM copy only.
        constexpr uint32_t boot_rom_start = 0x1000;
        constexpr int32_t boot_ram_start = 0x80000400;
        std::span<const uint8_t> rom = recomp::get_rom();
        std::vector<uint8_t> patched(rom.begin(), rom.end());
        if (custom_music) {
            apply_custom_music(patched, options);
        }
        for (size_t i = 0; options.music_shuffle != MusicShuffle::Off && i * 2 + 1 < data::bgmdata.size(); i++) {
            uint32_t address = static_cast<uint32_t>(std::stoul(data::bgmdata[i * 2], nullptr, 16));
            if (address >= bgm_table_start && address < bgm_table_end && address < patched.size()) {
                uint8_t track = static_cast<uint8_t>(roll_track(rng));
                patched[address] = track;
                MEM_B(0, boot_ram_start + static_cast<int32_t>(address - boot_rom_start)) = static_cast<int8_t>(track);
            }
        }
        recomp::set_rom_contents(std::move(patched));
    }

    if (options.music_shuffle == MusicShuffle::All) {
        for (int track = 0; track < track_count; track++) {
            if (usable_track(track)) {
                bgm_remap[track] = static_cast<int8_t>(roll_track(rng));
            }
        }
    }

    if (options.sfx_shuffle) {
        std::shuffle(sfx_remap.begin(), sfx_remap.end(), rng);
    }
}

// UpdateBGM (0x800267B8) and func_800267F8: a0 is the track to start, or
// -1 to stop. Only the tracks the game starts by number come through here;
// the map table's rows are read by func_8002684C and were rewritten above.
extern "C" void quest64_audio_bgm(uint8_t*, recomp_context* ctx) {
    if (zelda64::audio::active_options().music_shuffle != MusicShuffle::All) {
        return;
    }
    int track = static_cast<int8_t>(ctx->r4 & 0xFF);
    if (usable_track(track)) {
        ctx->r4 = S32(bgm_remap[track]);
    }
}

// func_800263A8: the second sequence player, used for jingles started by
// number (a0 is the track).
extern "C" void quest64_audio_jingle(uint8_t*, recomp_context* ctx) {
    if (zelda64::audio::active_options().music_shuffle != MusicShuffle::All) {
        return;
    }
    int track = static_cast<int32_t>(ctx->r4);
    if (usable_track(track)) {
        ctx->r4 = S32(bgm_remap[track]);
    }
}

// func_80025B8C: the routine every sound effect ends in, queued or not,
// with the effect id in a0 (volume in a1, 0x40 in a2). A volume of zero is
// a stop request (func_80025B3C), left alone.
extern "C" void quest64_audio_sfx(uint8_t*, recomp_context* ctx) {
    if (!zelda64::audio::active_options().sfx_shuffle) {
        return;
    }
    int32_t id = static_cast<int32_t>(ctx->r4);
    if (id < 0 || id >= sfx_count) {
        return;
    }
    if (static_cast<int32_t>(ctx->r5) == 0) {
        return;
    }
    int played = sfx_remap[id];
    ctx->r4 = S32(played);
    sfx_started[played] = sfx_clock::now();
    sfx_live[played] = true;
}

// Once per frame from the cheats frame hook: cut whatever has been going
// for three seconds since it last started.
std::string zelda64::audio::song_name(int track) {
    auto it = session_songs.find(track);
    if (it != session_songs.end()) {
        return it->second;
    }
    return track_label(track);
}

void zelda64::audio::apply_tracks_live(const std::map<int, std::string>& tracks) {
    std::lock_guard lock(live_mutex);
    pending_entries.clear();
    for (int track = 0; track < game_track_count; track++) {
        auto it = tracks.find(track);
        pending_entries[track] = it != tracks.end() ? it->second : std::string();
    }
    pending_all = true;
}

void zelda64::audio::preview_track(int track) {
    std::lock_guard lock(live_mutex);
    pending_preview = track < 0 ? -1 : track;
}

bool zelda64::audio::library_loaded() {
    return library_in_rom;
}

void zelda64::audio::on_frame(uint8_t* rdram) {
    // The menu's live changes: table entries, then a preview request.
    if (library_in_rom) {
        std::map<int, std::string> entries;
        int preview = -2;
        {
            std::lock_guard lock(live_mutex);
            if (pending_all) {
                entries.swap(pending_entries);
                pending_all = false;
            }
            preview = pending_preview;
            pending_preview = -2;
        }
        for (const auto& [track, name] : entries) {
            write_live_entry(rdram, track, name);
        }
        int requested = static_cast<int8_t>(MEM_B(0, bgm_request_track));
        if (preview >= 0) {
            if (preview_return == -2) {
                preview_return = static_cast<int8_t>(MEM_B(0, bgm_current_track));
            }
            if (requested == preview) {
                // The track already requested: stop this frame, start next.
                request_track(rdram, -1);
                std::lock_guard lock(live_mutex);
                pending_preview = preview;
            }
            else {
                request_track(rdram, preview);
            }
        }
        else if (preview == -1 && preview_return != -2) {
            request_track(rdram, preview_return);
            preview_return = -2;
        }
    }

    // "Show song name" (Layout tab): a line whenever the main player
    // starts a track.
    int track = static_cast<int8_t>(MEM_B(0, bgm_current_track));
    if (track != last_seen_track) {
        bool announce = last_seen_track != -2 && track >= 0 && track < game_track_count
            && zelda64::enhancements::active_options().song_notice;
        last_seen_track = track;
        if (announce) {
            zelda64::notify::post("Now playing: " + song_name(track));
        }
        // Diagnostics: what the player was handed for this track.
        if (library_in_rom && track >= 0 && track < game_track_count) {
            int32_t table = static_cast<int32_t>(MEM_W(0, seq_table_pointer));
            int32_t player = 0x8008F978;
            int32_t buffer = static_cast<int32_t>(MEM_W(0, player + 0x190));
            live_log << "  start: track " << track << " entry 0x" << std::hex
                     << static_cast<uint32_t>(MEM_W(0, table + 4 + track * 8)) << " len 0x"
                     << static_cast<uint32_t>(MEM_W(0, table + 8 + track * 8)) << " buffer 0x" << buffer << ":";
            for (int i = 0; i < 24; i++) {
                live_log << " " << static_cast<int>(MEM_BU(0, buffer + i));
            }
            live_log << std::dec << "\n";
            live_log.flush();
        }
    }

    if (!active_options().sfx_shuffle) {
        return;
    }
    sfx_clock::time_point now = sfx_clock::now();
    for (int id = 0; id < sfx_count; id++) {
        if (!sfx_live[id]) {
            continue;
        }
        if (std::chrono::duration<float>(now - sfx_started[id]).count() >= sfx_cutoff_seconds) {
            queue_sfx_stop(rdram, id);
            sfx_live[id] = false;
        }
    }
}
