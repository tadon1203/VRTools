#include "WndProcHook.hpp"

#include <stdexcept>

#include "Core/Logger.hpp"

WndProcHook& WndProcHook::instance() {
    static WndProcHook inst;
    return inst;
}

void WndProcHook::initialize() {
    m_windowHandle = FindWindowA("UnityWndClass", nullptr);
    if (!m_windowHandle) {
        throw std::runtime_error("Failed to find game window.");
    }

    m_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(m_windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hookWndProc)));

    if (!m_originalWndProc) {
        throw std::runtime_error("Failed to hook WndProc.");
    }
    Logger::instance().info("WndProc hooked successfully.");
}

void WndProcHook::shutdown() {
    if (m_originalWndProc && m_windowHandle) {
        SetWindowLongPtr(m_windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_originalWndProc));
        m_originalWndProc = nullptr;
        Logger::instance().info("WndProc unhooked.");
    }
}

void WndProcHook::addCallback(const WndProcCallback& callback) { m_callbacks.emplace_back(callback); }

LRESULT WndProcHook::hookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto& self = instance();

    bool wasEventConsumed = false;

    for (auto it = self.m_callbacks.rbegin(); it != self.m_callbacks.rend(); ++it) {
        if ((*it)(hWnd, uMsg, wParam, lParam)) {
            wasEventConsumed = true;
        }
    }

    if (wasEventConsumed) {
        return TRUE;
    }

    return CallWindowProc(self.m_originalWndProc, hWnd, uMsg, wParam, lParam);
}
