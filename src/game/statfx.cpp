#include <cstring>
#include <mutex>

#include "statfx.h"
#include "enhancements.h"
#include "recomp.h"

using zelda64::statfx::Anchor;
using zelda64::statfx::Event;
using zelda64::statfx::Stat;

namespace {
    // Brian's live stats (gPlayerMainData), the same block the cheats tab
    // edits: a rise in the maximum is what the JP game celebrates.
    constexpr int32_t gPlayerMainData = 0x8007BA80;
    constexpr int32_t player_max_hp = gPlayerMainData + 0x06;
    constexpr int32_t player_max_mp = gPlayerMainData + 0x0A;
    constexpr int32_t player_agility = gPlayerMainData + 0x0C;
    constexpr int32_t player_defense = gPlayerMainData + 0x0E;

    // gPlayerData1: the object the movement handlers are given, with his
    // world position in the first three floats.
    constexpr int32_t gPlayerData1 = 0x8007BACC;

    // gGameMode is 1 in the field and 3 in a battle (the setter at
    // 0x80026CAC picks one or the other); 2 is the file select and 4 the
    // title, where the stats are whatever the save has. gNextMap is -1
    // until a map is loaded.
    constexpr int32_t gGameMode = 0x8007B2E0;
    constexpr int32_t gNextMap = 0x80084EE4;
    constexpr int game_mode_field = 1;
    constexpr int game_mode_battle = 3;

    // The float matrices func_80012Cxx hands to guPerspectiveF and
    // guLookAtReflectF every frame, the game's only camera set-up. Both are
    // libultra row-vector matrices: clip = world * view * projection.
    constexpr int32_t camera_projection = 0x80086E48;
    constexpr int32_t camera_view = 0x80086E88;

    // How tall Brian is in world units, for the top of the burst. Walking
    // covers about two units a frame.
    constexpr float brian_height = 22.0f;

    // A rise bigger than this is a save loading or a cheat, not a stat-up.
    constexpr int max_rise = 30;

    struct Watched {
        Stat stat;
        int32_t address;
    };
    constexpr Watched watched[] = {
        { Stat::HP, player_max_hp },
        { Stat::MP, player_max_mp },
        { Stat::Defense, player_defense },
        { Stat::Agility, player_agility },
    };
    constexpr size_t watched_count = sizeof(watched) / sizeof(watched[0]);

    int previous[watched_count] = {};
    bool primed = false;

    std::mutex events_mutex;
    std::vector<Event> events;
    Anchor shared_anchor;

    float read_f32(uint8_t* rdram, int32_t addr) {
        int32_t bits = MEM_W(0, addr);
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    struct Vec4 {
        float x, y, z, w;
    };

    // v * M for a libultra float matrix stored row-major at `addr`.
    Vec4 transform(uint8_t* rdram, int32_t addr, const Vec4& v) {
        float m[4][4];
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                m[r][c] = read_f32(rdram, addr + (r * 4 + c) * 4);
            }
        }
        return {
            v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0],
            v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1],
            v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2],
            v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3],
        };
    }

    // World point to NDC; false when it is behind the camera.
    bool project(uint8_t* rdram, float x, float y, float z, float& out_x, float& out_y) {
        Vec4 clip = transform(rdram, camera_projection, transform(rdram, camera_view, { x, y, z, 1.0f }));
        if (clip.w <= 0.0001f) {
            return false;
        }
        out_x = clip.x / clip.w;
        out_y = clip.y / clip.w;
        return out_x == out_x && out_y == out_y;
    }

    void update_anchor(uint8_t* rdram, bool in_game) {
        Anchor a;
        if (in_game) {
            float x = read_f32(rdram, gPlayerData1 + 0x0);
            float y = read_f32(rdram, gPlayerData1 + 0x4);
            float z = read_f32(rdram, gPlayerData1 + 0x8);
            a.valid = project(rdram, x, y, z, a.feet_x, a.feet_y)
                && project(rdram, x, y + brian_height, z, a.head_x, a.head_y);
        }
        std::lock_guard<std::mutex> lock(events_mutex);
        shared_anchor = a;
    }
}

void zelda64::statfx::on_frame(uint8_t* rdram) {
    if (!zelda64::enhancements::active_options().stat_up_effect) {
        return;
    }
    int mode = MEM_HU(0, gGameMode);
    bool in_game = (mode == game_mode_field || mode == game_mode_battle)
        && static_cast<int32_t>(MEM_W(0, gNextMap)) != -1;
    update_anchor(rdram, in_game);

    if (!in_game) {
        // Whatever the stats are when play resumes is the new baseline, so
        // loading a save never reads as a rise.
        primed = false;
        return;
    }

    int current[watched_count];
    for (size_t i = 0; i < watched_count; i++) {
        current[i] = MEM_HU(0, watched[i].address);
    }
    if (primed) {
        for (size_t i = 0; i < watched_count; i++) {
            int rise = current[i] - previous[i];
            if (rise > 0 && rise <= max_rise && previous[i] > 0) {
                std::lock_guard<std::mutex> lock(events_mutex);
                events.push_back({ watched[i].stat, rise });
            }
        }
    }
    for (size_t i = 0; i < watched_count; i++) {
        previous[i] = current[i];
    }
    primed = true;
}

std::vector<Event> zelda64::statfx::take_events() {
    std::lock_guard<std::mutex> lock(events_mutex);
    std::vector<Event> out;
    out.swap(events);
    return out;
}

Anchor zelda64::statfx::anchor() {
    std::lock_guard<std::mutex> lock(events_mutex);
    return shared_anchor;
}
