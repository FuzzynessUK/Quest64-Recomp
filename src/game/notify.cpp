#include <mutex>

#include "notify.h"

namespace {
    std::mutex queue_mutex;
    std::vector<std::string> queue;
}

void zelda64::notify::post(std::string text) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    queue.push_back(std::move(text));
}

std::vector<std::string> zelda64::notify::take() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::vector<std::string> out;
    out.swap(queue);
    return out;
}
