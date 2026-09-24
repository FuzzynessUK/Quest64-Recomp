#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "speedrun.h"
#include "archipelago.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "recomp.h"

namespace {
    using clock = std::chrono::steady_clock;

    std::atomic<bool> is_running = false;
    std::atomic<bool> is_finished = false;
    clock::time_point started_at{};
    // Held so the finished time stops moving once Mammon is down.
    std::atomic<long long> final_ms = 0;
    // Where this run started from: 0 for a new file, the saved time when a
    // file was continued. carry_ms is what the next start() will take.
    std::atomic<long long> offset_ms = 0;
    std::atomic<long long> carry_ms = 0;
    // The title overlay sets this every frame it runs; the run begins on
    // the first frame it stops, which is the moment New has been taken.
    std::atomic<bool> title_frame = false;
    bool title_was_running = false;
    std::atomic<bool> armed = false;

    long long elapsed_ms() {
        if (is_finished.load()) {
            return final_ms.load();
        }
        if (!is_running.load()) {
            return 0;
        }
        return offset_ms.load() +
            std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - started_at).count();
    }
}

void zelda64::speedrun::arm() {
    reset();
    armed.store(true);
    title_was_running = false;
    title_frame.store(false);
}

// Runs from the per-frame hook. The title overlay flag is consumed here so
// a frame in which the overlay did not run reads as the title being over.
void zelda64::speedrun::update(bool) {
    bool now = title_frame.exchange(false);
    if (armed.load() && title_was_running && !now) {
        armed.store(false);
        start();
    }
    title_was_running = now;
}

void zelda64::speedrun::start() {
    started_at = clock::now();
    // A file that was continued brings its saved time with it.
    offset_ms.store(carry_ms.exchange(0));
    final_ms.store(0);
    is_finished.store(false);
    is_running.store(true);
}

void zelda64::speedrun::stop() {
    if (!is_running.load() || is_finished.load()) {
        return;
    }
    final_ms.store(elapsed_ms());
    is_finished.store(true);
    is_running.store(false);
}

void zelda64::speedrun::reset() {
    is_running.store(false);
    is_finished.store(false);
    final_ms.store(0);
    offset_ms.store(0);
}

long long zelda64::speedrun::elapsed() {
    return elapsed_ms();
}

void zelda64::speedrun::carry(long long ms) {
    carry_ms.store(ms);
}

bool zelda64::speedrun::running() {
    return is_running.load();
}

bool zelda64::speedrun::finished() {
    return is_finished.load();
}

std::string zelda64::speedrun::display() {
    long long total = elapsed_ms();
    long long seconds = (total / 1000) % 60;
    long long minutes = (total / 60000) % 60;
    long long hours = total / 3600000;

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02lld:%02lld:%02lld", hours, minutes, seconds);
    return buffer;
}

// Hooked just before func_80007A50 calls the message display with
// gMsg_Shannon_Mammon_has_been_destroyed (0x80054C04), which is the moment the
// run is over.
extern "C" void quest64_speedrun_stop(recomp_context* ctx) {
    zelda64::speedrun::stop();
}

// Hooked at the top of the title overlay's per-frame update, so it fires only
// while the title screen is actually on screen.
extern "C" void quest64_speedrun_title_frame() {
    title_frame.store(true);
}

// ---- carrying the time in the save file
//
// Quest 64 saves to the Controller Pak, and wraps osPfsReadWriteFile in two
// helpers that differ only in the flag they pass: func_800319E0 reads
// (PFS_READ, the 0 at 0x80031A24) and func_80031A44 writes (PFS_WRITE, the 1
// at 0x80031AA8). Both take (port, file_no, offset, nbytes, buffer), the
// fifth on the caller's stack at sp+0x10 - a hook at the first instruction
// runs before the prologue, so sp is still the caller's there.
//
// The saved bytes themselves are the key. The game's own data is left
// untouched - there is no room in it for a time, and changing it would fail
// the game's checksum - so the time goes in run_times.json beside the other
// settings, filed under the file number and a checksum of exactly the bytes
// that were written. A read of the same bytes finds it again.
//
// Keying on the content rather than the slot is what keeps the file-select
// screen from confusing matters: a listing that reads a header, or a
// different length, hashes differently and matches nothing. The time is not
// applied when it is found either, only handed to the timer to start from,
// so a read during play cannot wind the clock back.
namespace {
    std::filesystem::path run_times_path() {
        return zelda64::get_app_folder_path() / "run_times.json";
    }

    std::map<std::string, long long> run_times;
    bool run_times_loaded = false;

    void load_run_times() {
        if (run_times_loaded) {
            return;
        }
        run_times_loaded = true;
        std::ifstream in(run_times_path());
        if (!in.is_open()) {
            return;
        }
        nlohmann::json j;
        try {
            in >> j;
        }
        catch (nlohmann::json::parse_error&) {
            return;
        }
        if (!j.is_object()) {
            return;
        }
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it->is_number_integer()) {
                run_times[it.key()] = it->get<long long>();
            }
        }
    }

    void save_run_times() {
        nlohmann::json j = nlohmann::json::object();
        for (const auto& [key, ms] : run_times) {
            j[key] = ms;
        }
        std::ofstream out(run_times_path());
        if (out.is_open()) {
            out << j.dump(2);
        }
    }

    // FNV-1a over the bytes as the game sees them, so a save is recognised
    // by its contents.
    std::string save_key(uint8_t* rdram, int file_no, int32_t buffer, int nbytes) {
        uint32_t hash = 2166136261u;
        for (int i = 0; i < nbytes; i++) {
            hash = (hash ^ static_cast<uint8_t>(MEM_B(0, buffer + i))) * 16777619u;
        }
        char key[48];
        std::snprintf(key, sizeof key, "%d:%d:%08X", file_no, nbytes, hash);
        return key;
    }

    std::ofstream pak_log;
    void pak_note(const std::string& line) {
        if (!pak_log.is_open()) {
            pak_log.open(zelda64::get_app_folder_path() / "run_times.txt");
        }
        pak_log << line << "\n";
        pak_log.flush();
    }

    // What the read wrapper was asked for, kept from its entry until the
    // call has returned and the buffer holds something.
    int read_file_no = 0;
    int32_t read_buffer = 0;
    int read_bytes = 0;
}

// func_8002B510, at its first instruction: the Controller Pak menu is being
// opened, and a0 says from where - it is ORed into the menu's mode flags
// (0x8008FD0C). The field loop passes 4 when a save NPC opens it
// (0x8000207C); the main menu passes 0 for Load (0x80001680) and 8 for its
// other entry (0x80001690). Both run the same menu in the same game mode, so
// this is the one thing that tells an in-game save from a load.
namespace {
    bool pak_menu_from_field = false;
}

extern "C" void quest64_speedrun_pak_menu_open(recomp_context* ctx) {
    pak_menu_from_field = (ctx->r4 & 4) != 0;
}

// func_80031A44, at its first instruction: the game is writing a save.
extern "C" void quest64_speedrun_pak_write(uint8_t* rdram, recomp_context* ctx) {
    load_run_times();
    int file_no = static_cast<int32_t>(ctx->r5);
    int nbytes = static_cast<int32_t>(ctx->r7);
    int32_t buffer = static_cast<int32_t>(MEM_W(0, static_cast<int32_t>(ctx->r29) + 0x10));
    if (nbytes <= 0 || nbytes > 0x8000 || buffer == 0) {
        return;
    }
    long long ms = zelda64::speedrun::elapsed();
    std::string key = save_key(rdram, file_no, buffer, nbytes);
    run_times[key] = ms;
    save_run_times();
    pak_note("write " + key + " at " + std::to_string(ms) + " ms");
    // The connector files its own note against the same key: how much of
    // what Archipelago has sent this save has been given. This is the only
    // place that sees a save going past, so it does the asking.
    zelda64::archipelago::save_progress(key);
}

// func_800319E0, at its first instruction: note what is being read.
extern "C" void quest64_speedrun_pak_read_begin(uint8_t* rdram, recomp_context* ctx) {
    read_file_no = static_cast<int32_t>(ctx->r5);
    read_bytes = static_cast<int32_t>(ctx->r7);
    read_buffer = static_cast<int32_t>(MEM_W(0, static_cast<int32_t>(ctx->r29) + 0x10));
}

// 0x80031A34, the instruction after that wrapper's call has returned, where
// the buffer holds the save. v0 is osPfsReadWriteFile's result, 0 for ok.
extern "C" void quest64_speedrun_pak_read_end(uint8_t* rdram, recomp_context* ctx) {
    load_run_times();
    if (static_cast<int32_t>(ctx->r2) != 0 || read_bytes <= 0 || read_bytes > 0x8000 || read_buffer == 0) {
        return;
    }
    std::string key = save_key(rdram, read_file_no, read_buffer, read_bytes);
    // The in-game save menu reads every file just to list it. That is not
    // a file being continued, and taking it for one wound the connector's
    // item mark back to when that file was saved - everything since, Level
    // Ups included, was handed over a second time. Only the main menu's
    // Load screen loads.
    if (pak_menu_from_field) {
        pak_note("read  " + key + " - in game (the save menu), not a load");
        return;
    }
    // Before the early return below: the connector wants to know about a
    // save whether or not the timer has a time for it.
    zelda64::archipelago::load_progress(key);
    auto it = run_times.find(key);
    if (it == run_times.end()) {
        pak_note("read  " + key + " - no time stored for it");
        return;
    }
    zelda64::speedrun::carry(it->second);
    pak_note("read  " + key + " - the run starts from " + std::to_string(it->second) + " ms");
}
