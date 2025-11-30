#pragma once

#include <Windows.h>

#include <array>
#include <functional>
#include <unordered_map>

struct MousePos {
    long x;
    long y;
};

enum class MouseButton { Left, Right, Middle, Side1, Side2 };

using KeybindCallback = std::function<void()>;

class InputManager {
public:
    static InputManager& instance();

    InputManager(const InputManager&)            = delete;
    InputManager& operator=(const InputManager&) = delete;

    void initialize();
    void update();

    [[nodiscard]] bool isKeyDown(int vkCode) const;
    [[nodiscard]] bool wasKeyPressed(int vkCode) const;
    [[nodiscard]] bool wasKeyReleased(int vkCode) const;

    [[nodiscard]] bool isMouseButtonDown(MouseButton button) const;
    [[nodiscard]] bool wasMouseButtonPressed(MouseButton button) const;

    [[nodiscard]] MousePos getMousePosition() const;
    [[nodiscard]] int getScrollDelta() const;

    void registerKeybind(int vkCode, const KeybindCallback& callback);

private:
    InputManager()  = default;
    ~InputManager() = default;

    bool onWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    [[nodiscard]] static int mouseButtonToVk(MouseButton button);

    std::array<bool, 256> m_keyStates{};
    std::array<bool, 256> m_prevKeyStates{};
    std::unordered_map<int, KeybindCallback> m_keybinds;

    MousePos m_mousePos{ 0, 0 };
    int m_scrollDelta = 0;
};
