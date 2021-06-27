#pragma once
#include "event.h"
#include "event_handler.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <sstream>

class EventHandlerManager
{
    std::unordered_map<event_key_t, std::shared_ptr<EventHandler>> handlers;
    event_key_t lastKey;
    std::mutex mtx;

public:
    EventHandlerManager() :
        lastKey(0)
    {}

    Event add(std::shared_ptr<EventHandler> handler)
    {
        std::lock_guard<std::mutex> lock(mtx);
        event_key_t old = lastKey;
        while (handlers.count(++lastKey) > 0) {
            if (lastKey == old) {
                throw std::runtime_error("no more space for new event handler");
            }
        }
        handlers.insert({ lastKey, handler });
        return Event(lastKey);
    }

    bool contains(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return handlers.count(key) > 0;
    }

    std::shared_ptr<EventHandler> get(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (handlers.count(key) == 0) {
            throw std::runtime_error("failed to find eventHandler");
        }
        return handlers[key];
    }

    size_t count()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return handlers.size();
    }

    void remove(event_key_t key)
    {
        std::lock_guard<std::mutex> lock(mtx);
         handlers.erase(key);
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        handlers.clear();
    }

    std::string getKeys() // for debug
    {
        std::stringstream ss;
        for (const auto& kv : handlers) {
            ss << kv.first << ",";
        }
        return ss.str();
    }
};

