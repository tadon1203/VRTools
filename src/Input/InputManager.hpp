#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include <Windows.h>

using KeybindCallback = std::function<void()>;

class InputManager {
public:
    static InputManager& instance();

    InputManager(const InputManager&)            = delete;
    InputManager& operator=(const InputManager&) = delete;

    void initialize();

    bool isKeyDown(int vkCode) const;
    bool wasKeyPressed(int vkCode) const;

    void registerKeybind(int vkCode, const KeybindCallback& callback);

    void update();

private:
    InputManager()  = default;
    ~InputManager() = default;

    bool onWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    std::array<bool, 256> m_keyStates{};
    std::array<bool, 256> m_prevKeyStates{};
    std::unordered_map<int, KeybindCallback> m_keybinds;
};
