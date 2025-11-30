#pragma once

#include <d3d11.h>
#include <dxgi.h>

class Renderer {
public:
    static Renderer& instance();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    void initialize();
    void shutdown();

private:
    Renderer()  = default;
    ~Renderer() = default;

    static HRESULT WINAPI hookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static HRESULT(WINAPI* m_originalPresent)(IDXGISwapChain*, UINT, UINT);

    static HRESULT WINAPI hookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height,
        DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static HRESULT(WINAPI* m_originalResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    bool onWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool m_isInitialized = false;
    HWND m_windowHandle  = nullptr;

    ID3D11Device* m_device                     = nullptr;
    ID3D11DeviceContext* m_context             = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
};
