#include "archipelago_client.h"

// By its path: on Windows "Archipelago.h" would find our own
// include/archipelago.h first.
#include "../../lib/APCpp/Archipelago.h"

#include <atomic>
#include <deque>
#include <exception>
#include <mutex>
#include <set>

namespace q64ap {
namespace {
    // Must match the apworld's game name exactly (tools/archipelago_world.pl).
    constexpr const char* game_name = "Quest 64 Recompiled";
    // The Archipelago version the apworld is built for. APCpp would otherwise
    // announce 0.5.1, which a newer server may refuse.
    AP_NetworkVersion ap_version{ 0, 6, 7 };

    // So a misbehaving server cannot grow either queue without limit.
    constexpr size_t max_queued = 20000;

    std::mutex queue_mutex;
    std::deque<Event> events;       // guarded by queue_mutex
    std::deque<Message> messages;   // guarded by queue_mutex
    uint32_t item_index = 0;        // guarded by queue_mutex
    std::atomic<bool> active{ false };

    std::mutex log_mutex;
    std::function<void(const std::string&)> log_out;   // guarded by log_mutex

    void log(const std::string& line) {
        std::lock_guard lock{ log_mutex };
        if (log_out) {
            log_out(line);
        }
    }

    // True on a thread that is inside one of the calls below. APCpp logs from
    // those as well as from its network thread, and only on the network
    // thread may its message queue be touched.
    thread_local bool in_our_call = false;
    struct OurCall {
        OurCall() { in_our_call = true; }
        ~OurCall() { in_our_call = false; }
    };

    Event make_event(EventType type, int64_t id = 0, uint32_t index = 0, bool notify = false) {
        Event e;
        e.type = type;
        e.id = id;
        e.index = index;
        e.notify = notify;
        return e;
    }

    void push_event(Event e) {
        std::lock_guard lock{ queue_mutex };
        if (events.size() < max_queued) {
            events.push_back(std::move(e));
        }
    }

    // Copies APCpp's messages into ours. Only called on APCpp's network
    // thread, which is the only thread that ever adds to its queue.
    void drain_messages() {
        while (AP_IsMessagePending()) {
            AP_Message* msg = AP_GetLatestMessage();
            if (msg != nullptr) {
                Message out;
                out.text = msg->text;
                if (msg->type == AP_MessageType::ItemSend) {
                    // APCpp only ever makes an ItemSend one of these.
                    auto* send = static_cast<AP_ItemSendMessage*>(msg);
                    out.item_send = true;
                    out.item = send->item;
                    out.sender = send->sendPlayer;
                    out.receiver = send->recvPlayer;
                }
                std::lock_guard lock{ queue_mutex };
                if (messages.size() < max_queued) {
                    messages.push_back(std::move(out));
                }
            }
            AP_ClearLatestMessage();
        }
    }

    // ---- APCpp's callbacks, on its network thread ----

    void on_log(std::string line) {
        log(line);
        if (!in_our_call) {
            drain_messages();
        }
    }

    void on_items_cleared() {
        drain_messages();
        std::lock_guard lock{ queue_mutex };
        item_index = 0;
        // The whole list is coming again, so items not yet taken go.
        std::erase_if(events, [](const Event& e) { return e.type == EventType::ItemReceived; });
        events.push_back(make_event(EventType::ResetItems));
    }

    void on_item_received(int64_t item, bool notify) {
        drain_messages();
        std::lock_guard lock{ queue_mutex };
        uint32_t index = item_index++;
        if (events.size() < max_queued) {
            events.push_back(make_event(EventType::ItemReceived, item, index, notify));
        }
    }

    void on_location_checked(int64_t location) {
        drain_messages();
        push_event(make_event(EventType::LocationChecked, location));
    }

    void on_slot_data(const std::string& key, const std::string& value) {
        Event e = make_event(EventType::SlotData);
        e.key = key;
        e.value = value;
        push_event(std::move(e));
    }

    std::string strip_scheme(const std::string& address) {
        for (const char* prefix : { "wss://", "ws://" }) {
            std::string p = prefix;
            if (address.compare(0, p.size(), p) == 0) {
                return address.substr(p.size());
            }
        }
        return address;
    }
}

void set_log(std::function<void(const std::string&)> f) {
    std::lock_guard lock{ log_mutex };
    log_out = std::move(f);
}

bool connect(const ConnectInfo& info) {
    OurCall guard;
    if (info.address.empty() || info.slot.empty()) {
        return false;
    }
    disconnect();
    {
        std::lock_guard lock{ queue_mutex };
        events.clear();
        messages.clear();
        item_index = 0;
    }
    std::string address = strip_scheme(info.address);
    try {
        AP_Init(address.c_str(), game_name, info.slot.c_str(), info.password.c_str());
        AP_SetClientVersion(&ap_version);
        AP_SetLoggingCallback(on_log);
        AP_SetItemClearCallback(on_items_cleared);
        AP_SetItemRecvCallback(on_item_received);
        AP_SetLocationCheckedCallback(on_location_checked);
        for (const std::string& key : info.slot_data_keys) {
            AP_RegisterSlotDataRawCallback(key, [key](std::string value) { on_slot_data(key, value); });
        }
        // The game announces what arrives itself, with its own names.
        AP_EnableQueueItemRecvMsgs(false);
        AP_Start();
    }
    catch (const std::exception& e) {
        // AP_Init makes a data package cache folder in the working directory,
        // which can fail; nothing about that is worth taking the game down.
        log(std::string("APCpp could not start: ") + e.what());
        if (AP_IsInit()) {
            AP_Shutdown();
        }
        return false;
    }
    active = true;
    return true;
}

void disconnect() {
    OurCall guard;
    active = false;
    if (AP_IsInit()) {
        // Stops and joins APCpp's network thread before clearing its state,
        // so no callback runs after this returns.
        AP_Shutdown();
    }
    std::lock_guard lock{ queue_mutex };
    events.clear();
    messages.clear();
    item_index = 0;
}

Status status() {
    OurCall guard;
    if (!AP_IsInit()) {
        return Status::Idle;
    }
    switch (AP_GetConnectionStatus()) {
        case AP_ConnectionStatus::Connected:         return Status::Connected;
        case AP_ConnectionStatus::Authenticated:     return Status::Authenticated;
        case AP_ConnectionStatus::ConnectionRefused: return Status::Refused;
        case AP_ConnectionStatus::Disconnected:
        default:                                     return Status::Disconnected;
    }
}

int our_slot() {
    OurCall guard;
    return AP_IsInit() ? AP_GetPlayerID() : -1;
}

void send_locations(const std::vector<int64_t>& location_ids) {
    OurCall guard;
    if (!active || location_ids.empty()) {
        return;
    }
    AP_SendItem(std::set<int64_t>(location_ids.begin(), location_ids.end()));   // APCpp's name for LocationChecks
}

void send_goal_complete() {
    OurCall guard;
    if (active) {
        AP_StoryComplete();
    }
}

bool poll_event(Event& out) {
    std::lock_guard lock{ queue_mutex };
    if (events.empty()) {
        return false;
    }
    out = std::move(events.front());
    events.pop_front();
    return true;
}

bool poll_message(Message& out) {
    std::lock_guard lock{ queue_mutex };
    if (messages.empty()) {
        return false;
    }
    out = std::move(messages.front());
    messages.pop_front();
    return true;
}
}
