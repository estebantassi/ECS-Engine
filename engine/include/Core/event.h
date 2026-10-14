#pragma once

#include <functional>

template<typename... Args>
class Event
{
public:
    using Callback = std::function<void(Args...)>;

    size_t Subscribe(Callback callback)
    {
        size_t id = m_NextID++;
        m_Callbacks[id] = callback;
        return id;
    }

    void Unsubscribe(size_t id)
    {
        m_Callbacks.erase(id);
    }

    void Invoke(Args... args)
    {
        for (auto& [id, callback] : m_Callbacks)
            callback(args...);
    }

private:
    size_t m_NextID = 0;
    std::unordered_map<size_t, Callback> m_Callbacks;
};