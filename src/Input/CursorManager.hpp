#pragma once

#include <mutex>
#include <string>
#include <unordered_set>

#include "SDK/Unity/Cursor.hpp"

class CursorManager {
public:
    static CursorManager& instance();

    void initialize();
    void update();

    void requestUnlock(const std::string& id);

    void releaseUnlock(const std::string& id);

    // Called by hooks
    void onSetLockState(UnityEngine::CursorLockMode& value);
    void onSetVisible(bool& value);

private:
    CursorManager()  = default;
    ~CursorManager() = default;

    bool m_initialized = false;
    bool m_isApplying  = false;
    bool m_wasUnlocked = false;

    std::mutex m_mutex;
    std::unordered_set<std::string> m_requests;

    UnityEngine::CursorLockMode m_lastLockMode = UnityEngine::CursorLockMode::None;
    bool m_lastVisible                         = true;
};
