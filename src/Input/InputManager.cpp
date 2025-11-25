#include "InputManager.hpp"

#include "Hooking/WndProcHook.hpp"

InputManager& InputManager::instance() {
    static InputManager inst;
    return inst;
}

void InputManager::initialize() {
    WndProcHook::instance().addCallback([this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return this->onWndProc(hWnd, uMsg, wParam, lParam);
    });
}

bool InputManager::isKeyDown(int vkCode) const {
    if (vkCode < 0 || vkCode >= 256) {
        return false;
    }
    return m_keyStates[vkCode];
}

bool InputManager::wasKeyPressed(int vkCode) const {
    if (vkCode < 0 || vkCode >= 256) {
        return false;
    }
    return m_keyStates[vkCode] && !m_prevKeyStates[vkCode];
}

void InputManager::registerKeybind(int vkCode, const KeybindCallback& callback) { m_keybinds[vkCode] = callback; }

void InputManager::update() {
    for (const auto& pair : m_keybinds) {
        if (wasKeyPressed(pair.first)) {
            pair.second();
        }
    }

    m_prevKeyStates = m_keyStates;
}

bool InputManager::onWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256) {
            m_keyStates[wParam] = true;
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256) {
            m_keyStates[wParam] = false;
        }
        break;
    case WM_LBUTTONDOWN:
        m_keyStates[VK_LBUTTON] = true;
        break;
    case WM_LBUTTONUP:
        m_keyStates[VK_LBUTTON] = false;
        break;
    case WM_RBUTTONDOWN:
        m_keyStates[VK_RBUTTON] = true;
        break;
    case WM_RBUTTONUP:
        m_keyStates[VK_RBUTTON] = false;
        break;
    case WM_MBUTTONDOWN:
        m_keyStates[VK_MBUTTON] = true;
        break;
    case WM_MBUTTONUP:
        m_keyStates[VK_MBUTTON] = false;
        break;
    }
    return false;
}
