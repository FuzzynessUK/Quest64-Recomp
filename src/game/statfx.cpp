#include <cstdio>
#include <cstring>
#include <filesystem>
#include <mutex>

#include "statfx.h"
#include "enhancements.h"
#include "zelda_config.h"
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
    constexpr int32_t camera_eye = 0x80086DCC;
    constexpr int32_t camera_at = 0x80086DD8;
    constexpr int32_t camera_fovy = 0x80086EC8;

    // Calibration log, statfx_debug.txt next to the settings: a line a
    // second while the effect is on, to check the projection against what
    // is seen on screen. Capped so it cannot grow without bound.
    // Calibration aid, off while playing: set log_enabled to true to get it back.
    constexpr bool log_enabled = false;
    constexpr int log_every_frames = 60;
    constexpr int log_max_lines = 600;
    int log_frame = 0;
    int log_lines = 0;
    FILE* log_file = nullptr;

    // Where Brian's feet and head sit relative to the position the game
    // stores for him, in world units. Calibrated by eye: the position is
    // his feet (a halo centred below it sat at his feet), a halo centred 11
    // above it floated just over his head, and the camera aims 12 above it.
    // Walking covers about two units a frame.
    constexpr float brian_feet_offset = 0.0f;
    constexpr float brian_head_offset = 9.0f;

    // A rise bigger than this is a save loading or a cheat, not a stat-up.
    constexpr int max_rise = 30;

    // The sound. Eltale Monsters queues effect 0x35 four frames after a
    // stat levels up (JP 0x80009080; the US twin of that routine at
    // 0x800078DC has no such call, and nothing in the US code plays 0x35,
    // though the bank still has it). It goes in the way func_800268D4
    // queues a delayed effect: the 16 x 3-byte table at D_8008FCC8 -
    // frames left, id, volume - that func_80026658 counts down and plays
    // through func_80025B8C. The volume is the id's entry in the table at
    // 0x80053CAC scaled by the request (0xFF) and the master byte at
    // D_8008FCC6, as the game does it.
    constexpr int stat_up_sfx = 0x35;
    constexpr int stat_up_sfx_delay = 4;
    // The game asks for 0xFF; this chime is played at 80% of that.
    constexpr unsigned stat_up_sfx_request = 0xCC;
    constexpr int32_t sfx_queue = 0x8008FCC8;
    constexpr int sfx_queue_slots = 16;
    constexpr int32_t sfx_volume_table = 0x80053CAC;
    constexpr int32_t sfx_master_volume = 0x8008FCC6;

    void queue_sfx(uint8_t* rdram, int id, int delay) {
        for (int slot = 0; slot < sfx_queue_slots; slot++) {
            int32_t entry = sfx_queue + slot * 3;
            if (MEM_BU(0, entry) != 0) {
                continue;
            }
            uint32_t volume = (MEM_BU(0, sfx_volume_table + id) * stat_up_sfx_request * MEM_BU(0, sfx_master_volume)) >> 16;
            MEM_B(0, entry) = static_cast<int8_t>(delay);
            MEM_B(1, entry) = static_cast<int8_t>(id);
            MEM_B(2, entry) = static_cast<int8_t>(volume);
            return;
        }
    }

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

    void log_state(uint8_t* rdram, float x, float y, float z, const Anchor& a) {
        if (!log_enabled) {
            return;
        }
        if (++log_frame < log_every_frames || log_lines >= log_max_lines) {
            return;
        }
        log_frame = 0;
        if (log_file == nullptr) {
            std::filesystem::path path = zelda64::get_app_folder_path() / "statfx_debug.txt";
            log_file = std::fopen(path.string().c_str(), "w");
            if (log_file == nullptr) {
                log_lines = log_max_lines;
                return;
            }
        }
        float at_x, at_y;
        bool at_ok = project(rdram, read_f32(rdram, camera_at), read_f32(rdram, camera_at + 4), read_f32(rdram, camera_at + 8), at_x, at_y);
        std::fprintf(log_file,
            "mode %d map %d | brian %.1f %.1f %.1f | eye %.1f %.1f %.1f | at %.1f %.1f %.1f | fovy %.1f | feet ndc %d %.3f %.3f | head ndc %.3f %.3f | at ndc %d %.3f %.3f\n",
            MEM_HU(0, gGameMode), static_cast<int32_t>(MEM_W(0, gNextMap)),
            x, y, z,
            read_f32(rdram, camera_eye), read_f32(rdram, camera_eye + 4), read_f32(rdram, camera_eye + 8),
            read_f32(rdram, camera_at), read_f32(rdram, camera_at + 4), read_f32(rdram, camera_at + 8),
            read_f32(rdram, camera_fovy),
            a.valid ? 1 : 0, a.feet_x, a.feet_y, a.head_x, a.head_y,
            at_ok ? 1 : 0, at_x, at_y);
        std::fflush(log_file);
        log_lines++;
    }

    void update_anchor(uint8_t* rdram, bool in_game) {
        Anchor a;
        if (in_game) {
            float x = read_f32(rdram, gPlayerData1 + 0x0);
            float y = read_f32(rdram, gPlayerData1 + 0x4);
            float z = read_f32(rdram, gPlayerData1 + 0x8);
            a.valid = project(rdram, x, y + brian_feet_offset, z, a.feet_x, a.feet_y)
                && project(rdram, x, y + brian_head_offset, z, a.head_x, a.head_y);
            log_state(rdram, x, y, z, a);
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
        bool rose = false;
        for (size_t i = 0; i < watched_count; i++) {
            int rise = current[i] - previous[i];
            if (rise > 0 && rise <= max_rise && previous[i] > 0) {
                std::lock_guard<std::mutex> lock(events_mutex);
                events.push_back({ watched[i].stat, rise });
                rose = true;
            }
        }
        // One chime even when two stats rise on the same frame.
        if (rose) {
            queue_sfx(rdram, stat_up_sfx, stat_up_sfx_delay);
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
