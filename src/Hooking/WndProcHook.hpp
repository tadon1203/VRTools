#pragma once

#include <functional>

#include <Windows.h>

using WndProcCallback = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;

class WndProcHook {
public:
    static WndProcHook& instance();

    WndProcHook(const WndProcHook&)            = delete;
    WndProcHook& operator=(const WndProcHook&) = delete;

    void initialize();
    void shutdown();

    void addCallback(const WndProcCallback& callback);

private:
    WndProcHook()  = default;
    ~WndProcHook() = default;

    static LRESULT CALLBACK hookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_windowHandle       = nullptr;
    WNDPROC m_originalWndProc = nullptr;
    std::vector<WndProcCallback> m_callbacks;
};
