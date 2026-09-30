#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <mutex>

#include "minimap.h"
#include "recomp.h"

// See include/minimap.h for what is read and where. The outline is read again
// whenever the map, the submap or the navigation pointer changes; the markers
// every frame, since chests open and Brian walks.
namespace {
    constexpr int32_t nav_data_ptr = 0x80084F2C;
    constexpr int32_t submap_header_ptr = 0x80084F1C;
    constexpr int32_t chest_table = 0x800862E0;
    constexpr int32_t chest_count_addr = 0x800869A0;
    constexpr int32_t chest_stride = 0x6C;
    constexpr int32_t chest_flags = 0x800869D8;
    constexpr int32_t spirit_table = 0x80086A00;
    constexpr int32_t spirit_stride = 0x18;
    constexpr int32_t player = 0x8007BACC;
    constexpr int32_t map_byte = 0x8008536B;
    constexpr int32_t submap_byte = 0x8008536F;
    constexpr int32_t gGameMode = 0x8007B2E0;
    constexpr int32_t gBattleState = 0x8008C592;
    constexpr int32_t gNextMap = 0x80084EE4;

    std::mutex state_mutex;
    zelda64::minimap::Snapshot state;
    std::atomic<bool> any_frame{ false };

    int last_map = -1, last_submap = -1;
    uint32_t last_nav = 0;

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
}

void zelda64::minimap::on_frame(uint8_t* rdram) {
    any_frame.store(true);
    bool field = MEM_HU(0, gGameMode) == 1 && (MEM_HU(0, gBattleState) & 1) == 0 &&
                 static_cast<int32_t>(MEM_W(0, gNextMap)) != -1;
    uint32_t nav = static_cast<uint32_t>(MEM_W(0, nav_data_ptr));
    int map = MEM_BU(0, map_byte);
    int submap = MEM_BU(0, submap_byte);

    std::lock_guard lock{ state_mutex };
    if (!field || !ram_ptr(nav) || submap > 32) {
        state.valid = false;
        return;
    }
    if (map != last_map || submap != last_submap || nav != last_nav) {
        last_map = map;
        last_submap = submap;
        last_nav = nav;
        state.outline = read_outline(rdram, nav, submap);
        state.mesh_version++;
        float min_x = 1e9f, min_z = 1e9f, max_x = -1e9f, max_z = -1e9f;
        for (const auto& chain : state.outline) {
            for (const Point& p : chain) {
                min_x = std::min(min_x, p.x); max_x = std::max(max_x, p.x);
                min_z = std::min(min_z, p.z); max_z = std::max(max_z, p.z);
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
        bool opened = id < 88 && (MEM_BU(0, chest_flags + (id >> 3)) & (1u << (id & 7))) != 0;
        state.chests.push_back({ x, z, opened });
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
            for (uint32_t i = 0; i < std::min<uint32_t>(exit_count, 64); i++) {
                int32_t e = static_cast<int32_t>(exits + i * 0x24);
                float x = read_f32(rdram, e), z = read_f32(rdram, e + 4);
                if (sane(x) && sane(z)) {
                    state.exits.push_back({ x, z, false });
                }
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
