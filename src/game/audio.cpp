#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include "audio.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "recomp.h"

namespace data = merrow::data;
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
    return o;
}

void zelda64::audio::save_options(const Options& o) {
    nlohmann::json j;
    j["music_shuffle"] = static_cast<int>(o.music_shuffle);
    j["sfx_shuffle"] = o.sfx_shuffle;
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
    if (options.music_shuffle == MusicShuffle::Off && !options.sfx_shuffle) {
        return;
    }

    std::mt19937 rng{ std::random_device{}() };

    if (options.music_shuffle != MusicShuffle::Off) {
        // Reads back whatever the earlier boot patches left, so this stacks
        // on top of them rather than replacing them. The table is in the
        // boot segment (ROM 0x1000.., 1MB), which has already been copied to
        // RAM at 0x80000400 by now, so each byte goes to both: the ROM copy
        // alone is never read again.
        constexpr uint32_t boot_rom_start = 0x1000;
        constexpr int32_t boot_ram_start = 0x80000400;
        std::span<const uint8_t> rom = recomp::get_rom();
        std::vector<uint8_t> patched(rom.begin(), rom.end());
        for (size_t i = 0; i * 2 + 1 < data::bgmdata.size(); i++) {
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
void zelda64::audio::on_frame(uint8_t* rdram) {
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
