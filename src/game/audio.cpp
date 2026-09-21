#include <filesystem>
#include <fstream>
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
using zelda64::audio::Options;

namespace {
    Options active;
    bool active_loaded = false;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "audio.json";
    }

    // The 73 background-music slots (address, vanilla track) Merrow lists in
    // bgmdata. The usable tracks are 0-26 and 31-41, so a roll of 0-37 skips
    // the unused four in the middle, as Merrow's does.
    constexpr int usable_tracks = 38;
    constexpr int unused_gap_start = 27;
    constexpr int unused_gap_size = 4;
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
    get("music_shuffle", o.music_shuffle);
    return o;
}

void zelda64::audio::save_options(const Options& o) {
    nlohmann::json j;
    j["music_shuffle"] = o.music_shuffle;
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

void zelda64::audio::apply_at_boot(uint8_t*) {
    const Options& options = active_options();
    if (!options.music_shuffle) {
        return;
    }

    // Reads back whatever the earlier boot patches left, so this stacks on
    // top of them rather than replacing them.
    std::span<const uint8_t> rom = recomp::get_rom();
    std::vector<uint8_t> patched(rom.begin(), rom.end());

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> roll(0, usable_tracks - 1);
    for (size_t i = 0; i * 2 + 1 < data::bgmdata.size(); i++) {
        int track = roll(rng);
        if (track >= unused_gap_start) {
            track += unused_gap_size;
        }
        uint32_t address = static_cast<uint32_t>(std::stoul(data::bgmdata[i * 2], nullptr, 16));
        if (address < patched.size()) {
            patched[address] = static_cast<uint8_t>(track);
        }
    }

    recomp::set_rom_contents(std::move(patched));
}
