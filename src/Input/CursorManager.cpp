#include "CursorManager.hpp"

#include "Core/Logger.hpp"

CursorManager& CursorManager::instance() {
    static CursorManager inst;
    return inst;
}

void CursorManager::initialize() {
    if (m_initialized) {
        return;
    }

    try {
        m_lastLockMode = UnityEngine::Cursor::get_lockState();
        m_lastVisible  = UnityEngine::Cursor::get_visible();
        m_initialized  = true;
    } catch (const std::exception& e) {
        Logger::instance().warn("Failed to initialize CursorManager state: {}", e.what());
    }
}

void CursorManager::update() {
    if (!m_initialized) {
        initialize();
    }

    bool hasRequests;
    {
        std::lock_guard lock(m_mutex);
        hasRequests = !m_requests.empty();
    }

    if (hasRequests) {
        m_isApplying = true;
        UnityEngine::Cursor::set_lockState(UnityEngine::CursorLockMode::None);
        UnityEngine::Cursor::set_visible(true);
        m_isApplying  = false;
        m_wasUnlocked = true;
    } else if (m_wasUnlocked) {
        m_isApplying = true;
        UnityEngine::Cursor::set_lockState(m_lastLockMode);
        UnityEngine::Cursor::set_visible(m_lastVisible);
        m_isApplying  = false;
        m_wasUnlocked = false;
    }
}

void CursorManager::requestUnlock(const std::string& id) {
    std::lock_guard lock(m_mutex);
    m_requests.insert(id);
}

void CursorManager::releaseUnlock(const std::string& id) {
    std::lock_guard lock(m_mutex);
    m_requests.erase(id);
}

void CursorManager::onSetLockState(UnityEngine::CursorLockMode& value) {
    if (m_isApplying) {
        return;
    }

    std::lock_guard lock(m_mutex);
    m_lastLockMode = value;

    if (!m_requests.empty()) {
        value = UnityEngine::CursorLockMode::None;
    }
}

void CursorManager::onSetVisible(bool& value) {
    if (m_isApplying) {
        return;
    }

    std::lock_guard lock(m_mutex);
    m_lastVisible = value;

    if (!m_requests.empty()) {
        value = true;
    }
}
