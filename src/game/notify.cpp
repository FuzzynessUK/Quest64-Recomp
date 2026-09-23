#include <mutex>

#include "notify.h"

namespace {
    std::mutex queue_mutex;
    std::vector<zelda64::notify::Message> queue;
}

void zelda64::notify::post(std::string text, Kind kind) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    queue.push_back({ std::move(text), kind });
}

std::vector<zelda64::notify::Message> zelda64::notify::take() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::vector<Message> out;
    out.swap(queue);
    return out;
}
