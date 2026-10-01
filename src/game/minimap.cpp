#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <map>
#include <mutex>
#include <utility>

#include "minimap.h"
#include "archipelago.h"
#include "hardmode.h"
#include "randomizer.h"
#include "randomizer/chest_data.h"
#include "randomizer/spirit_data.h"
#include "recomp.h"
#include "zelda_debug.h"

// See include/minimap.h for what is read and where. The outline is read again
// whenever the map, the submap, the navigation pointer or the submap's entry
// in it changes; the markers every frame, since chests open and Brian walks.
namespace {
    constexpr int32_t nav_data_ptr = 0x80084F2C;
    constexpr int32_t submap_header_ptr = 0x80084F1C;
    constexpr int32_t chest_table = 0x800862E0;
    constexpr int32_t chest_count_addr = 0x800869A0;
    constexpr int32_t chest_stride = 0x6C;
    constexpr int32_t chest_flags = 0x800869D8;
    constexpr int32_t spirit_flags = 0x80086AE8;
    constexpr int32_t spirit_table = 0x80086A00;
    constexpr int32_t spirit_stride = 0x18;
    constexpr int32_t player = 0x8007BACC;
    // Where Brian is (see minimap.h for why not D_80085368).
    constexpr int32_t map_word = 0x80084EE4;
    constexpr int32_t submap_word = 0x80084EE8;
    constexpr int32_t gGameMode = 0x8007B2E0;
    constexpr int32_t gGameState = 0x8007B2E4;
    // 0x80 leaving through an exit, 0x40 the next map loading.
    constexpr int32_t game_state_moving = 0xC0;
    constexpr int32_t gBattleState = 0x8008C592;
    constexpr int32_t text_windows = 0x800905E0;
    constexpr int32_t text_window_size = 0x89C;
    constexpr int text_window_count = 4;
    constexpr uint32_t text_window_open = 0x30000000;
    // How far a door may be from a building's wall to light that building.
    constexpr float door_reach = 25.0f;

    std::mutex state_mutex;
    zelda64::minimap::Snapshot state;
    std::atomic<bool> any_frame{ false };

    int last_map = -1, last_submap = -1;
    uint32_t last_nav = 0;
    uint32_t last_entry[4] = {};
    // The outline that is the submap's outer edge (the largest), and per exit
    // (by its index in the header's list) the outline its door is on, worked
    // out once per outline.
    int outer_group = -1;
    std::vector<int> exit_groups;
    bool exit_groups_dirty = true;

    bool ram_ptr(uint32_t p) {
        return p >= 0x80000000u && p < 0x80800000u;
    }

    float read_f32(uint8_t* rdram, int32_t addr) {
        uint32_t bits = static_cast<uint32_t>(MEM_W(0, addr));
        float f;
        std::memcpy(&f, &bits, sizeof f);
        return f;
    }

    bool sane(float v) {
        return std::isfinite(v) && std::fabs(v) < 100000.0f;
    }

    // The submap's outlines; empty if anything along the way looks wrong.
    std::vector<std::vector<zelda64::minimap::Point>> read_outline(uint8_t* rdram, uint32_t nav, int submap) {
        std::vector<std::vector<zelda64::minimap::Point>> out;
        int32_t entry = static_cast<int32_t>(nav + 0x10u * static_cast<uint32_t>(submap));
        uint32_t verts = static_cast<uint32_t>(MEM_W(0x0, entry));
        uint32_t indices = static_cast<uint32_t>(MEM_W(0x4, entry));
        uint32_t groups = static_cast<uint32_t>(MEM_W(0x8, entry));
        uint32_t group_count = static_cast<uint32_t>(MEM_W(0xC, entry));
        if (!ram_ptr(verts) || !ram_ptr(indices) || !ram_ptr(groups) || group_count == 0 || group_count > 4096) {
            return out;
        }
        for (uint32_t g = 0; g < group_count; g++) {
            int32_t group = static_cast<int32_t>(groups + g * 8);
            uint32_t first = static_cast<uint32_t>(MEM_W(0, group));
            uint32_t count = static_cast<uint32_t>(MEM_W(4, group));
            if (count > 4096 || first > 65536) {
                return {};
            }
            std::vector<zelda64::minimap::Point> chain;
            for (uint32_t k = 0; k <= count; k++) {
                uint32_t index = static_cast<uint32_t>(MEM_W(0, static_cast<int32_t>(indices + (first + k) * 4)));
                if (index > 65536) {
                    return {};
                }
                int32_t v = static_cast<int32_t>(verts + index * 8);
                float x = read_f32(rdram, v), z = read_f32(rdram, v + 4);
                if (!sane(x) || !sane(z)) {
                    return {};
                }
                chain.push_back({ x, z });
            }
            if (chain.size() >= 2) {
                out.push_back(std::move(chain));
            }
        }
        return out;
    }

    // The sixteen gift NPCs, in Archipelago's giver order (Merrow's
    // itemgranters). Each map has one NPC list (map table +0x38: { u16
    // count, records }), records 0x2C bytes with the submap as the u16 at
    // +0, the item at +7 and x, z as f32 at +0x1C, +0x20; read from the US
    // ROM for the records Merrow names. No option moves an NPC.
    struct GiverSpot { uint8_t map, submap; float x, z; };
    constexpr GiverSpot givers[] = {
        { 13,  7, -100.0f, 104.0f },  // Pat, Monastery kitchen
        { 16, 13,    5.0f,   0.0f },  // Mable, Dondoran bar
        { 16, 13,    0.0f, -34.0f },  // Maggie, Dondoran bar
        { 17,  0,  -14.0f, -14.0f },  // Galita, Larapool inn
        { 20,  2,  -10.0f,   0.0f },  // Hector, Normoon house
        { 22, 20,  -20.0f,  -2.0f },  // Rhett, Limelin tavern
        { 23, 18,    0.0f, -19.0f },  // Morris, Brannoch house
        { 23,  5,    0.0f,  -4.0f },  // Bronze, Greenoch shop
        { 11,  4,  -32.0f, -83.0f },  // Shannon, Brannoch Castle
        { 34,  3,    0.0f,   0.0f },  // Shannon, Mammon's World
        { 15, 11,    0.0f, -14.0f },  // Ingram, Melrode wingsmith
        { 16, 14,   -2.0f, -14.0f },  // Thom, Dondoran wingsmith
        { 18,  4,    0.0f, -14.0f },  // Jiryo, Larapool wingsmith
        { 20, 11,    0.0f,  -4.0f },  // Anette, Normoon wingsmith
        { 22, 21,    0.0f, -15.0f },  // Ring, Limelin wingsmith
        { 23, 14,    0.0f,  -4.0f },  // Tom, Brannoch wingsmith
    };
    constexpr int giver_count = static_cast<int>(std::size(givers));

    // Archipelago's view of the givers, refreshed each frame: whether each
    // is a check in the seed being played, and whether it has been checked.
    // In vanilla (or not connected) a giver is never a check: the game keeps
    // no "given" state, so there is nothing to look for.
    bool giver_ap = false;
    std::vector<uint8_t> giver_in_seed, giver_checked;

    void read_givers() {
        static const std::vector<int64_t> locations = [] {
            std::vector<int64_t> out;
            for (int i = 0; i < giver_count; i++) {
                out.push_back(zelda64::archipelago::id_base + zelda64::archipelago::group_giver + i);
            }
            return out;
        }();
        giver_ap = zelda64::archipelago::tracker_view(locations, giver_in_seed, giver_checked);
    }

    bool giver_open(int i) {
        return giver_ap && giver_in_seed[i] && !giver_checked[i];
    }

    // Checks by submap: kind 0 a chest, 1 a spirit, with its save flag's bit,
    // 2 a giver by its index above. Chests and spirits from the randomizer's
    // placements when it moved them, else the game's.
    struct Check { uint8_t kind; uint8_t id; };
    std::map<std::pair<int, int>, std::vector<Check>> checks_by_submap;
    bool checks_built = false;

    void build_checks() {
        checks_built = true;
        checks_by_submap.clear();
        using namespace zelda64::randomizer;
        bool moved = active_options().mode == Mode::Randomizer && !zelda64::hardmode::active();
        const NativeState& native = native_state();
        if (moved && !native.chest_placements.empty()) {
            for (const ChestPlacement& c : native.chest_placements) {
                checks_by_submap[{ c.map, c.submap }].push_back({ 0, c.id });
            }
        }
        else {
            for (const merrow::chests::Chest& c : merrow::chests::chests) {
                checks_by_submap[{ c.map, c.submap }].push_back({ 0, c.id });
            }
        }
        // Spirit ids run in table order, in the game's table and in the one
        // the randomizer writes (quest64_randomizer_spirits).
        int id = 0;
        if (moved && !native.spirit_slots.empty()) {
            for (const auto& slot : native.spirit_slots) {
                for (const SpiritPlacement& p : slot) {
                    checks_by_submap[{ p.map, p.submap }].push_back({ 1, static_cast<uint8_t>(id++) });
                }
            }
        }
        else {
            for (const merrow::spirits::Slot& slot : merrow::spirits::vanilla_slots) {
                for (int k = 0; k < slot.count; k++) {
                    checks_by_submap[{ slot.map, slot.submap }].push_back({ 1, static_cast<uint8_t>(id++) });
                }
            }
        }
        for (int i = 0; i < giver_count; i++) {
            checks_by_submap[{ givers[i].map, givers[i].submap }].push_back({ 2, static_cast<uint8_t>(i) });
        }
    }

    // What lies through a door to {map, submap}: that submap and the rest of
    // its map reachable from there without coming back to where Brian is.
    // Cached per destination until he changes submap.
    std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> regions;

    const std::vector<std::pair<int, int>>& region_through(int here_map, int here_submap, int map, int submap) {
        auto found = regions.find({ map, submap });
        if (found != regions.end()) {
            return found->second;
        }
        std::vector<std::pair<int, int>> seen;
        if (map != here_map || submap != here_submap) {
            seen.push_back({ map, submap });
        }
        std::vector<std::pair<int, int>> next;
        for (size_t i = 0; i < seen.size() && seen.size() < 64; i++) {
            zelda64::exit_destinations(seen[i].first, seen[i].second, next);
            for (const auto& d : next) {
                if (d.first != map || (d.first == here_map && d.second == here_submap)) {
                    continue;
                }
                if (std::find(seen.begin(), seen.end(), d) == seen.end()) {
                    seen.push_back(d);
                }
            }
        }
        return regions.emplace(std::make_pair(map, submap), std::move(seen)).first->second;
    }

    bool flag_set(uint8_t* rdram, int32_t base, int id) {
        return (MEM_BU(0, base + (id >> 3)) & (1u << (id & 7))) != 0;
    }

    // A gift NPC anywhere through the door, unless Archipelago says it has
    // been checked already.
    bool region_has_giver(const std::vector<std::pair<int, int>>& region) {
        for (const auto& at : region) {
            auto found = checks_by_submap.find(at);
            if (found == checks_by_submap.end()) {
                continue;
            }
            for (const Check& c : found->second) {
                if (c.kind == 2 && !(giver_ap && giver_checked[c.id])) {
                    return true;
                }
            }
        }
        return false;
    }

    bool region_has_checks(uint8_t* rdram, const std::vector<std::pair<int, int>>& region) {
        for (const auto& at : region) {
            auto found = checks_by_submap.find(at);
            if (found == checks_by_submap.end()) {
                continue;
            }
            for (const Check& c : found->second) {
                bool open = c.kind == 2 ? giver_open(c.id)
                                        : !flag_set(rdram, c.kind == 0 ? chest_flags : spirit_flags, c.id);
                if (open) {
                    return true;
                }
            }
        }
        return false;
    }

    float segment_distance(const zelda64::minimap::Point& a, const zelda64::minimap::Point& b, float x, float z) {
        float dx = b.x - a.x, dz = b.z - a.z;
        float len = dx * dx + dz * dz;
        float t = len > 0.0f ? std::clamp(((x - a.x) * dx + (z - a.z) * dz) / len, 0.0f, 1.0f) : 0.0f;
        return std::hypot(a.x + dx * t - x, a.z + dz * t - z);
    }

    // The outline nearest (x, z), if it is close and not the outer edge.
    int door_group(const std::vector<std::vector<zelda64::minimap::Point>>& outline, float x, float z) {
        int best = -1;
        float best_d = door_reach;
        for (size_t g = 0; g < outline.size(); g++) {
            const auto& chain = outline[g];
            for (size_t i = 1; i < chain.size(); i++) {
                float d = segment_distance(chain[i - 1], chain[i], x, z);
                if (d < best_d) {
                    best_d = d;
                    best = static_cast<int>(g);
                }
            }
        }
        return best == outer_group ? -1 : best;
    }
}

void zelda64::minimap::on_frame(uint8_t* rdram) {
    any_frame.store(true);
    int32_t map = static_cast<int32_t>(MEM_W(0, map_word));
    int32_t submap = static_cast<int32_t>(MEM_W(0, submap_word));
    bool field = MEM_HU(0, gGameMode) == 1 && (MEM_HU(0, gBattleState) & 1) == 0 && map >= 0;
    uint32_t nav = static_cast<uint32_t>(MEM_W(0, nav_data_ptr));

    std::lock_guard lock{ state_mutex };
    state.text_boxes.clear();
    for (int i = 0; i < text_window_count; i++) {
        int32_t w = text_windows + i * text_window_size;
        if ((static_cast<uint32_t>(MEM_W(0, w)) & text_window_open) == 0) {
            continue;
        }
        float bw = MEM_H(8, w), bh = MEM_H(0xA, w);
        if (bw > 0 && bh > 0) {
            state.text_boxes.push_back({ static_cast<float>(MEM_H(4, w)), static_cast<float>(MEM_H(6, w)), bw, bh });
        }
    }
    if (!field || !ram_ptr(nav) || submap < 0 || submap > 32) {
        state.valid = false;
        // Read the outline afresh on the way back, whatever it was before.
        last_map = -1;
        return;
    }
    // Through a door and the next submap loading: keep showing the last one
    // until the new one is in place.
    if ((MEM_W(0, gGameState) & game_state_moving) != 0) {
        return;
    }
    if (!checks_built) {
        build_checks();
    }
    uint32_t entry[4];
    for (int i = 0; i < 4; i++) {
        entry[i] = static_cast<uint32_t>(MEM_W(i * 4, static_cast<int32_t>(nav + 0x10u * static_cast<uint32_t>(submap))));
    }
    if (map != last_map || submap != last_submap || nav != last_nav || std::memcmp(entry, last_entry, sizeof entry) != 0) {
        last_map = map;
        last_submap = submap;
        last_nav = nav;
        std::memcpy(last_entry, entry, sizeof entry);
        state.outline = read_outline(rdram, nav, submap);
        state.mesh_version++;
        regions.clear();
        exit_groups_dirty = true;
        float min_x = 1e9f, min_z = 1e9f, max_x = -1e9f, max_z = -1e9f;
        outer_group = -1;
        float outer_area = -1.0f;
        for (size_t g = 0; g < state.outline.size(); g++) {
            float gx0 = 1e9f, gz0 = 1e9f, gx1 = -1e9f, gz1 = -1e9f;
            for (const Point& p : state.outline[g]) {
                gx0 = std::min(gx0, p.x); gx1 = std::max(gx1, p.x);
                gz0 = std::min(gz0, p.z); gz1 = std::max(gz1, p.z);
            }
            min_x = std::min(min_x, gx0); max_x = std::max(max_x, gx1);
            min_z = std::min(min_z, gz0); max_z = std::max(max_z, gz1);
            float area = (gx1 - gx0) * (gz1 - gz0);
            if (area > outer_area) {
                outer_area = area;
                outer_group = static_cast<int>(g);
            }
        }
        if (state.outline.empty() || max_x - min_x < 1.0f || max_z - min_z < 1.0f) {
            state.outline.clear();
            min_x = min_z = 0; max_x = max_z = 1;
        }
        state.min_x = min_x; state.min_z = min_z; state.max_x = max_x; state.max_z = max_z;
    }
    state.valid = !state.outline.empty();
    if (!state.valid) {
        return;
    }

    state.brian_x = read_f32(rdram, player + 0x0);
    state.brian_z = read_f32(rdram, player + 0x8);
    state.heading = read_f32(rdram, player + 0x10);

    state.chests.clear();
    uint32_t chests = static_cast<uint32_t>(MEM_W(0, chest_count_addr));
    for (uint32_t i = 0; i < std::min<uint32_t>(chests, 32); i++) {
        int32_t c = chest_table + static_cast<int32_t>(i) * chest_stride;
        float x = read_f32(rdram, c), z = read_f32(rdram, c + 8);
        if (!sane(x) || !sane(z)) {
            continue;
        }
        int id = MEM_HU(0x62, c);
        bool opened = id < 88 && flag_set(rdram, chest_flags, id);
        state.chests.push_back({ x, z, opened });
    }

    // Givers: grey once Archipelago has the check, lit the same as a door
    // while it is a check still to get.
    read_givers();
    state.givers.clear();
    for (int i = 0; i < giver_count; i++) {
        if (givers[i].map == map && givers[i].submap == submap) {
            Marker m{ givers[i].x, givers[i].z, giver_ap && giver_checked[i] != 0 };
            m.checks = giver_open(i);
            state.givers.push_back(m);
        }
    }

    state.spirits.clear();
    uint32_t spirits = static_cast<uint32_t>(MEM_W(0, spirit_table));
    for (uint32_t i = 0; i < std::min<uint32_t>(spirits, 32); i++) {
        int32_t s = spirit_table + 8 + static_cast<int32_t>(i) * spirit_stride;
        float x = read_f32(rdram, s), z = read_f32(rdram, s + 8);
        if (sane(x) && sane(z)) {
            state.spirits.push_back({ x, z, false });
        }
    }

    state.exits.clear();
    uint32_t header = static_cast<uint32_t>(MEM_W(0, submap_header_ptr));
    if (ram_ptr(header)) {
        uint32_t exits = static_cast<uint32_t>(MEM_W(0x4, static_cast<int32_t>(header)));
        uint32_t exit_count = static_cast<uint32_t>(MEM_W(0x8, static_cast<int32_t>(header)));
        if (ram_ptr(exits)) {
            uint32_t n = std::min<uint32_t>(exit_count, 64);
            if (exit_groups_dirty || exit_groups.size() != n) {
                exit_groups.assign(n, -1);
                for (uint32_t i = 0; i < n; i++) {
                    int32_t e = static_cast<int32_t>(exits + i * 0x24);
                    float x = read_f32(rdram, e), z = read_f32(rdram, e + 4);
                    if (sane(x) && sane(z)) {
                        exit_groups[i] = door_group(state.outline, x, z);
                    }
                }
                exit_groups_dirty = false;
            }
            for (uint32_t i = 0; i < n; i++) {
                int32_t e = static_cast<int32_t>(exits + i * 0x24);
                float x = read_f32(rdram, e), z = read_f32(rdram, e + 4);
                if (!sane(x) || !sane(z)) {
                    continue;
                }
                Marker m{ x, z, false };
                int dst_map = MEM_HU(0x1E, e), dst_submap = MEM_HU(0x20, e);
                if (dst_map < zelda64::map_count() && dst_submap < zelda64::submap_count(dst_map)) {
                    const auto& region = region_through(map, submap, dst_map, dst_submap);
                    m.checks = region_has_checks(rdram, region);
                    m.giver = region_has_giver(region);
                }
                m.group = exit_groups[i];
                state.exits.push_back(m);
            }
        }
    }
}

bool zelda64::minimap::snapshot(Snapshot& out, uint32_t mesh_have) {
    if (!any_frame.load()) {
        return false;
    }
    std::lock_guard lock{ state_mutex };
    out.valid = state.valid;
    out.brian_x = state.brian_x;
    out.brian_z = state.brian_z;
    out.heading = state.heading;
    out.chests = state.chests;
    out.spirits = state.spirits;
    out.givers = state.givers;
    out.text_boxes = state.text_boxes;
    out.exits = state.exits;
    if (mesh_have != state.mesh_version) {
        out.mesh_version = state.mesh_version;
        out.outline = state.outline;
        out.min_x = state.min_x; out.min_z = state.min_z;
        out.max_x = state.max_x; out.max_z = state.max_z;
    }
    return true;
}

namespace {
    std::atomic<int> zoom_steps{ 0 };
}

void zelda64::minimap::zoom(int direction) {
    zoom_steps.fetch_add(direction);
}

int zelda64::minimap::take_zoom_steps() {
    return zoom_steps.exchange(0);
}
