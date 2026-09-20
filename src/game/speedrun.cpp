#include <atomic>
#include <chrono>
#include <cstdio>
#include <string>

#include "speedrun.h"
#include "recomp.h"

namespace {
    using clock = std::chrono::steady_clock;

    std::atomic<bool> is_running = false;
    std::atomic<bool> is_finished = false;
    clock::time_point started_at{};
    // Held so the finished time stops moving once Mammon is down.
    std::atomic<long long> final_ms = 0;
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
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - started_at).count();
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
    final_ms.store(0);
    is_finished.store(false);
    is_running.store(true);
}

void zelda64::speedrun::stop() {
    if (!is_running.load() || is_finished.load()) {
        return;
    }
    final_ms.store(std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - started_at).count());
    is_finished.store(true);
    is_running.store(false);
}

void zelda64::speedrun::reset() {
    is_running.store(false);
    is_finished.store(false);
    final_ms.store(0);
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
