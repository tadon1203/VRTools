#pragma once

#include "SDK/Unity/Cursor.hpp"

class CursorManager {
public:
    static CursorManager& instance();

    void initialize();
    void update();

    void setUnlock(bool unlock);

    void onSetLockState(UnityEngine::CursorLockMode& value);
    void onSetVisible(bool& value);

private:
    CursorManager()  = default;
    ~CursorManager() = default;

    bool m_initialized  = false;
    bool m_shouldUnlock = false;
    bool m_isApplying   = false;

    UnityEngine::CursorLockMode m_lastLockMode = UnityEngine::CursorLockMode::None;
    bool m_lastVisible                         = true;
};
