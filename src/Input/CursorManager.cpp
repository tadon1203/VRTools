#include "CursorManager.hpp"

#include "Core/Logger.hpp"

CursorManager& CursorManager::instance() {
    static CursorManager inst;
    return inst;
}

void CursorManager::initialize() {
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
        return;
    }

    if (m_shouldUnlock) {
        m_isApplying = true;
        UnityEngine::Cursor::set_lockState(UnityEngine::CursorLockMode::None);
        UnityEngine::Cursor::set_visible(true);
        m_isApplying = false;
    }
}

void CursorManager::setUnlock(bool unlock) {
    if (m_shouldUnlock == unlock) {
        return;
    }

    m_shouldUnlock = unlock;

    m_isApplying = true;
    if (m_shouldUnlock) {
        UnityEngine::Cursor::set_lockState(UnityEngine::CursorLockMode::None);
        UnityEngine::Cursor::set_visible(true);
    } else {
        UnityEngine::Cursor::set_lockState(m_lastLockMode);
        UnityEngine::Cursor::set_visible(m_lastVisible);
    }
    m_isApplying = false;
}

void CursorManager::onSetLockState(UnityEngine::CursorLockMode& value) {
    // to avoid infinite loop
    if (m_isApplying) {
        return;
    }

    // Capture what the game wants to do
    m_lastLockMode = value;

    if (m_shouldUnlock) {
        value = UnityEngine::CursorLockMode::None;
    }
}

void CursorManager::onSetVisible(bool& value) {
    if (m_isApplying) {
        return;
    }

    m_lastVisible = value;

    if (m_shouldUnlock) {
        value = true;
    }
}
