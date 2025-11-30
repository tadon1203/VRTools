#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <typeindex>
#include <vector>

#include "IEvent.hpp"

using ListenerID = size_t;

class EventManager {
public:
    static EventManager& instance() {
        static EventManager inst;
        return inst;
    }

    template <typename EventType>
    ListenerID subscribe(std::function<void(EventType&)> callback) {
        std::lock_guard lock(m_mutex);

        ListenerID id = ++m_lastId;
        auto typeIdx  = std::type_index(typeid(EventType));

        auto wrapper = [callback](IEvent& e) { callback(static_cast<EventType&>(e)); };

        m_listeners[typeIdx].push_back({ id, wrapper });
        return id;
    }

    void unsubscribe(ListenerID id) {
        std::lock_guard lock(m_mutex);
        for (auto& [type, list] : m_listeners) {
            auto it =
                std::remove_if(list.begin(), list.end(), [id](const ListenerEntry& entry) { return entry.id == id; });

            if (it != list.end()) {
                list.erase(it, list.end());
                return;
            }
        }
    }

    template <typename EventType>
    void fire(EventType& event) {
        std::lock_guard lock(m_mutex);

        auto typeIdx = std::type_index(typeid(EventType));
        auto it      = m_listeners.find(typeIdx);

        if (it != m_listeners.end()) {
            for (auto& entry : it->second) {
                entry.callback(event);
                if constexpr (std::is_base_of_v<ICancellableEvent, EventType>) {
                    if (event.isCancelled()) {
                        break;
                    }
                }
            }
        }
    }

private:
    EventManager()  = default;
    ~EventManager() = default;

    struct ListenerEntry {
        ListenerID id;
        std::function<void(IEvent&)> callback;
    };

    std::mutex m_mutex;
    ListenerID m_lastId = 0;
    std::map<std::type_index, std::vector<ListenerEntry>> m_listeners;
};
