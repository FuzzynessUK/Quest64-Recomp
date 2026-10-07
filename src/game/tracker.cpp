#include <atomic>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <mutex>
#include <set>

#include "tracker.h"
#include "archipelago.h"
#include "enhancements.h"
#include "speedrun.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "recomp.h"

// The tracker's game side: its settings, the monsters beaten in the save being
// played, and the snapshot the windows are drawn from (ui_tracker.cpp). See
// include/tracker.h.
namespace {
    using json = nlohmann::json;

    // ---- options
    std::mutex options_mutex;
    zelda64::tracker::Options current;
    bool options_loaded = false;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "tracker.json";
    }

    void load_options_file() {
        options_loaded = true;
        std::ifstream in(options_path());
        if (!in.is_open()) {
            return;
        }
        json j = json::parse(in, nullptr, false);
        if (!j.is_object()) {
            return;
        }
        auto get = [&](const char* key, auto& field) {
            if (j.contains(key)) {
                try { field = j[key].get<std::remove_reference_t<decltype(field)>>(); }
                catch (const json::exception&) {}
            }
        };
        get("enabled", current.enabled);
        get("item_tracker", current.item_tracker);
        get("check_tracker", current.check_tracker);
        get("locked", current.locked);
        get("hide_done_areas", current.hide_done_areas);
        get("hide_borders", current.hide_borders);
        get("hide_titles", current.hide_titles);
        get("show_wings", current.show_wings);
        get("show_souls", current.show_souls);
        get("notes", current.notes);
        get("notes_x", current.notes_x);
        get("notes_y", current.notes_y);
        get("notes_w", current.notes_w);
        get("notes_h", current.notes_h);
        get("check_h", current.check_h);
        int background = 1;
        get("background", background);
        current.item_background = current.check_background = current.notes_background = background;
        get("item_background", current.item_background);
        get("check_background", current.check_background);
        get("notes_background", current.notes_background);
        get("item_x", current.item_x);
        get("item_y", current.item_y);
        get("check_x", current.check_x);
        get("check_y", current.check_y);
    }

    // ---- monsters beaten
    // The game keeps no record of which kinds have been beaten, so the kill
    // hook's are kept here and filed against the save's contents whenever the
    // game writes one, the same way the connector files its item mark. A file
    // loaded gets its own back; a new game starts empty.
    std::set<int> kills;
    std::map<std::string, std::vector<int>> kills_by_save;
    bool kills_file_loaded = false;
    bool was_on_title = false;

    std::filesystem::path kills_path() {
        return zelda64::get_app_folder_path() / "tracker_kills.json";
    }

    void load_kills_file() {
        if (kills_file_loaded) {
            return;
        }
        kills_file_loaded = true;
        std::ifstream in(kills_path());
        if (!in.is_open()) {
            return;
        }
        json j = json::parse(in, nullptr, false);
        if (!j.is_object()) {
            return;
        }
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (!it->is_array()) {
                continue;
            }
            std::vector<int> ids;
            for (const json& v : *it) {
                if (v.is_number_integer()) {
                    ids.push_back(v.get<int>());
                }
            }
            kills_by_save[it.key()] = std::move(ids);
        }
    }

    // ---- gift NPCs talked to, kept the same way (tracker_givers.json, a
    // bitmask per save).
    std::atomic<uint32_t> talked{ 0 };
    std::map<std::string, uint32_t> talked_by_save;
    bool talked_file_loaded = false;

    std::filesystem::path talked_path() {
        return zelda64::get_app_folder_path() / "tracker_givers.json";
    }

    void load_talked_file() {
        if (talked_file_loaded) {
            return;
        }
        talked_file_loaded = true;
        std::ifstream in(talked_path());
        if (!in.is_open()) {
            return;
        }
        json j = json::parse(in, nullptr, false);
        if (!j.is_object()) {
            return;
        }
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it->is_number_unsigned() || it->is_number_integer()) {
                talked_by_save[it.key()] = it->get<uint32_t>();
            }
        }
    }

    // ---- the snapshot
    std::mutex snapshot_mutex;
    zelda64::tracker::Snapshot latest;
    std::atomic<uint32_t> snapshot_version{ 0 };
    int frames_until_snapshot = 0;
    constexpr int snapshot_every = 15;

    // Save flags: a bit per chest and per spirit, and a byte of bosses beaten
    // (bit n-1 for boss n), as the connector reads them.
    constexpr int32_t chest_flags = 0x800869D8;
    constexpr int32_t spirit_flags = 0x80086AE8;
    constexpr int32_t bosses_beaten = 0x8007D19C;
    constexpr int32_t gNextMap = 0x80084EE4;
    constexpr int32_t room_map = 0x80084EE4;
    constexpr int32_t room_submap = 0x80084EE8;

    bool flag_set(uint8_t* rdram, int32_t base, int id) {
        return (MEM_BU(0, base + (id >> 3)) & (1u << (id & 7))) != 0;
    }

    std::vector<int64_t> all_locations() {
        std::vector<int64_t> out;
        out.reserve(zelda64::tracker::checks.size());
        for (const zelda64::tracker::CheckInfo& c : zelda64::tracker::checks) {
            out.push_back(c.location);
        }
        return out;
    }
}

const zelda64::tracker::Options& zelda64::tracker::options() {
    std::lock_guard lock{ options_mutex };
    if (!options_loaded) {
        load_options_file();
    }
    return current;
}

void zelda64::tracker::set_options(const Options& options) {
    std::lock_guard lock{ options_mutex };
    options_loaded = true;
    current = options;
    json j;
    j["enabled"] = current.enabled;
    j["item_tracker"] = current.item_tracker;
    j["check_tracker"] = current.check_tracker;
    j["locked"] = current.locked;
    j["hide_done_areas"] = current.hide_done_areas;
    j["hide_borders"] = current.hide_borders;
    j["hide_titles"] = current.hide_titles;
    j["show_wings"] = current.show_wings;
    j["show_souls"] = current.show_souls;
    j["notes"] = current.notes;
    j["notes_x"] = current.notes_x;
    j["notes_y"] = current.notes_y;
    j["notes_w"] = current.notes_w;
    j["notes_h"] = current.notes_h;
    j["check_h"] = current.check_h;
    j["item_background"] = current.item_background;
    j["check_background"] = current.check_background;
    j["notes_background"] = current.notes_background;
    j["item_x"] = current.item_x;
    j["item_y"] = current.item_y;
    j["check_x"] = current.check_x;
    j["check_y"] = current.check_y;
    std::ofstream out(options_path());
    if (out.is_open()) {
        out << j.dump(2);
    }
}

void zelda64::tracker::monster_killed(int id) {
    if (id >= 0 && id < 256) {
        kills.insert(id);
    }
}

void zelda64::tracker::giver_talked(int i) {
    if (i >= 0 && i < 32) {
        talked.fetch_or(1u << i);
    }
}

uint32_t zelda64::tracker::givers_talked() {
    return talked.load();
}

void zelda64::tracker::save_progress(const std::string& save_key) {
    load_talked_file();
    talked_by_save[save_key] = talked.load();
    {
        json t = json::object();
        for (const auto& [key, bits] : talked_by_save) {
            t[key] = bits;
        }
        std::ofstream tout(talked_path());
        if (tout.is_open()) {
            tout << t.dump();
        }
    }
    load_kills_file();
    kills_by_save[save_key] = std::vector<int>(kills.begin(), kills.end());
    json j = json::object();
    for (const auto& [key, ids] : kills_by_save) {
        j[key] = ids;
    }
    std::ofstream out(kills_path());
    if (out.is_open()) {
        out << j.dump();
    }
}

void zelda64::tracker::load_progress(const std::string& save_key) {
    load_kills_file();
    kills.clear();
    auto it = kills_by_save.find(save_key);
    if (it != kills_by_save.end()) {
        kills.insert(it->second.begin(), it->second.end());
    }
    load_talked_file();
    auto t = talked_by_save.find(save_key);
    talked.store(t == talked_by_save.end() ? 0u : t->second);
}

void zelda64::tracker::on_frame(uint8_t* rdram) {
    // Back on the title screen: whatever comes next is a new game or a file
    // being loaded, which brings its own kills with it.
    bool on_title = zelda64::speedrun::title_showing();
    if (on_title && !was_on_title) {
        kills.clear();
        talked.store(0);
    }
    was_on_title = on_title;

    if (--frames_until_snapshot > 0) {
        return;
    }
    frames_until_snapshot = snapshot_every;

    Snapshot s;
    s.in_game = !on_title && static_cast<int32_t>(MEM_W(0, gNextMap)) != -1;
    const size_t count = checks.size();
    s.found.assign(count, 0);
    s.present.assign(count, 1);

    static const std::vector<int64_t> locations = all_locations();
    std::vector<uint8_t> in_seed, checked;
    s.archipelago = zelda64::archipelago::tracker_view(locations, in_seed, checked);
    s.souls_mode = zelda64::archipelago::tracker_souls(s.souls);

    uint8_t beaten = s.in_game ? MEM_BU(0, bosses_beaten) : 0;
    for (size_t i = 0; i < count; i++) {
        const CheckInfo& c = checks[i];
        int index = static_cast<int>(c.location & 0xFFF);
        bool found = false;
        if (s.in_game) {
            switch (c.kind) {
                case Kind::Chest:   found = flag_set(rdram, chest_flags, index); break;
                case Kind::Spirit:  found = flag_set(rdram, spirit_flags, index); break;
                case Kind::Boss:    found = index >= 1 && index <= 8 && (beaten & (1u << (index - 1))) != 0; break;
                case Kind::Monster: found = kills.count(index) != 0; break;
                case Kind::Giver:   break;
            }
        }
        if (s.archipelago) {
            s.present[i] = in_seed[i];
            found = found || checked[i];
        }
        else if (c.kind == Kind::Giver) {
            // Nothing in the save says a gift was taken.
            s.present[i] = 0;
        }
        s.found[i] = found ? 1 : 0;
    }

    if (s.in_game) {
        int counts[256];
        zelda64::enhancements::bag_counts(rdram, counts);
        for (int id = 0; id < 32; id++) {
            s.item_counts[id] = counts[id];
        }
        // The room Brian is in, from the pair the minimap reads (minimap.h:
        // the exit record changes before the new map has loaded). A room,
        // not a map: one building set serves several areas.
        int map = static_cast<int>(MEM_W(0, room_map));
        int submap = static_cast<int>(MEM_W(0, room_submap));
        if (map >= 0 && map < static_cast<int>(room_area.size()) && submap >= 0 &&
            submap < static_cast<int>(room_area[static_cast<size_t>(map)].size())) {
            s.area = room_area[static_cast<size_t>(map)][static_cast<size_t>(submap)];
        }
    }

    std::lock_guard lock{ snapshot_mutex };
    bool changed = latest.in_game != s.in_game || latest.archipelago != s.archipelago ||
        latest.found != s.found || latest.present != s.present || latest.area != s.area ||
        latest.souls_mode != s.souls_mode || latest.souls != s.souls ||
        !std::equal(std::begin(s.item_counts), std::end(s.item_counts), std::begin(latest.item_counts));
    if (!changed) {
        return;
    }
    s.version = snapshot_version.fetch_add(1) + 1;
    latest = std::move(s);
}

bool zelda64::tracker::snapshot(Snapshot& out, uint32_t have) {
    if (snapshot_version.load() == have) {
        return false;
    }
    std::lock_guard lock{ snapshot_mutex };
    out = latest;
    return true;
}

std::string zelda64::tracker::load_notes() {
    std::ifstream in(zelda64::get_app_folder_path() / "tracker_notes.txt", std::ios::binary);
    if (!in.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

void zelda64::tracker::save_notes(const std::string& text) {
    std::ofstream out(zelda64::get_app_folder_path() / "tracker_notes.txt", std::ios::binary);
    if (out.is_open()) {
        out << text;
    }
}
