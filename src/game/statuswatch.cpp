// Diagnostic for the battle status system (temporary, 2026-09-21): logs
// every byte that changes in Brian's block and in each enemy's battle
// struct while a battle is running, to status_watch.txt next to the
// settings. Bytes that change more than a few times a battle (positions,
// animation timers, HP) are muted after their first few lines, so what is
// left is turn-counter-shaped: the status effect durations the earlier
// static searches (DOCS/HANDOFF.md "JP Buffs + Debuffs") could not find.
#include <cstdio>
#include <cstring>
#include <filesystem>

#include "zelda_config.h"
#include "recomp.h"

namespace {
    constexpr int32_t gPlayerMainData = 0x8007BA80;
    constexpr int32_t gBattleState = 0x8008C592;   // bit 0: a battle is running
    constexpr int watch_size = 0x200;
    constexpr int max_enemies = 8;
    constexpr int max_lines = 6000;
    constexpr int mute_after = 8;   // changes per battle before an offset goes quiet

    struct Block {
        const char* name;
        int32_t base = 0;
        uint8_t last[watch_size] = {};
        uint8_t changes[watch_size] = {};
        bool primed = false;
    };

    Block brian{ "brian" };
    Block enemies[max_enemies] = { { "enemy0" }, { "enemy1" }, { "enemy2" }, { "enemy3" }, { "enemy4" }, { "enemy5" }, { "enemy6" }, { "enemy7" } };
    int enemy_count = 0;
    bool was_in_battle = false;
    int frame = 0;
    int lines = 0;
    FILE* log_file = nullptr;

    void out(const char* fmt, auto... args) {
        if (lines >= max_lines) {
            return;
        }
        if (log_file == nullptr) {
            std::filesystem::path path = zelda64::get_app_folder_path() / "status_watch.txt";
            log_file = std::fopen(path.string().c_str(), "w");
            if (log_file == nullptr) {
                lines = max_lines;
                return;
            }
        }
        std::fprintf(log_file, fmt, args...);
        std::fflush(log_file);
        lines++;
    }

    void scan(uint8_t* rdram, Block& b) {
        if (b.base == 0) {
            return;
        }
        uint8_t now[watch_size];
        for (int i = 0; i < watch_size; i++) {
            now[i] = static_cast<uint8_t>(MEM_BU(0, b.base + i));
        }
        if (b.primed) {
            for (int i = 0; i < watch_size; i++) {
                if (now[i] == b.last[i]) {
                    continue;
                }
                if (b.changes[i] < mute_after) {
                    b.changes[i]++;
                    out("f%d %s+0x%03X: %d -> %d%s\n", frame, b.name, i, b.last[i], now[i],
                        b.changes[i] == mute_after ? " (muted)" : "");
                }
            }
        }
        std::memcpy(b.last, now, watch_size);
        b.primed = true;
    }
}

// func_80008FE0 at 0x8000908C, monster set-up at the start of a battle:
// a0 is the enemy's battle struct.
extern "C" void quest64_status_watch_enemy(uint8_t*, recomp_context* ctx) {
    int32_t base = static_cast<int32_t>(ctx->r4);
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].base == base) {
            return;
        }
    }
    if (enemy_count < max_enemies) {
        enemies[enemy_count].base = base;
        enemies[enemy_count].primed = false;
        std::memset(enemies[enemy_count].changes, 0, watch_size);
        out("f%d enemy%d struct at 0x%08X\n", frame, enemy_count, base);
        enemy_count++;
    }
}

// Once per frame from the cheats frame hook.
extern "C" void quest64_status_watch_frame(uint8_t* rdram) {
    frame++;
    bool in_battle = (MEM_HU(0, gBattleState) & 1) != 0;
    if (in_battle && !was_in_battle) {
        out("f%d --- battle start ---\n", frame);
        brian.base = gPlayerMainData;
        brian.primed = false;
        std::memset(brian.changes, 0, watch_size);
        enemy_count = 0;
        for (Block& e : enemies) {
            e.base = 0;
        }
    }
    if (!in_battle && was_in_battle) {
        out("f%d --- battle end ---\n", frame);
    }
    was_in_battle = in_battle;
    if (!in_battle) {
        return;
    }
    scan(rdram, brian);
    for (int i = 0; i < enemy_count; i++) {
        scan(rdram, enemies[i]);
    }
}
