#include "InputManager.hpp"

#include <windowsx.h> // GET_X_LPARAM, GET_Y_LPARAM

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

void InputManager::update() {
    for (const auto& [vkCode, callback] : m_keybinds) {
        if (wasKeyPressed(vkCode)) {
            if (callback) {
                callback();
            }
        }
    }
    m_scrollDelta   = 0;
    m_prevKeyStates = m_keyStates;
}

// --- Helper ---
int InputManager::mouseButtonToVk(MouseButton button) {
    switch (button) {
    case MouseButton::Left:
        return VK_LBUTTON;
    case MouseButton::Right:
        return VK_RBUTTON;
    case MouseButton::Middle:
        return VK_MBUTTON;
    case MouseButton::Side1:
        return VK_XBUTTON1;
    case MouseButton::Side2:
        return VK_XBUTTON2;
    default:
        return 0;
    }
}

// --- Keyboard Queries ---
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

bool InputManager::wasKeyReleased(int vkCode) const {
    if (vkCode < 0 || vkCode >= 256) {
        return false;
    }
    return !m_keyStates[vkCode] && m_prevKeyStates[vkCode];
}

bool InputManager::isMouseButtonDown(MouseButton button) const { return isKeyDown(mouseButtonToVk(button)); }

bool InputManager::wasMouseButtonPressed(MouseButton button) const { return wasKeyPressed(mouseButtonToVk(button)); }

MousePos InputManager::getMousePosition() const { return m_mousePos; }

int InputManager::getScrollDelta() const { return m_scrollDelta; }

void InputManager::registerKeybind(int vkCode, const KeybindCallback& callback) { m_keybinds[vkCode] = callback; }

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

    case WM_XBUTTONDOWN:
        {
            int xButton = GET_XBUTTON_WPARAM(wParam);
            if (xButton == XBUTTON1) {
                m_keyStates[VK_XBUTTON1] = true;
            }
            if (xButton == XBUTTON2) {
                m_keyStates[VK_XBUTTON2] = true;
            }
            break;
        }
    case WM_XBUTTONUP:
        {
            int xButton = GET_XBUTTON_WPARAM(wParam);
            if (xButton == XBUTTON1) {
                m_keyStates[VK_XBUTTON1] = false;
            }
            if (xButton == XBUTTON2) {
                m_keyStates[VK_XBUTTON2] = false;
            }
            break;
        }

    case WM_MOUSEMOVE:
        m_mousePos.x = GET_X_LPARAM(lParam);
        m_mousePos.y = GET_Y_LPARAM(lParam);
        break;

    case WM_MOUSEWHEEL:
        m_scrollDelta += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        break;
    }
    return false;
}
