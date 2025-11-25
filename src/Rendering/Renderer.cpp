#include "Renderer.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <stdexcept>
#include <string>

#include "Core/Logger.hpp"
#include "Features/FeatureManager.hpp"
#include "Hooking/HookManager.hpp"
#include "Hooking/WndProcHook.hpp"
#include "UI/NotificationManager.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT(WINAPI* Renderer::m_originalPresent)(IDXGISwapChain*, UINT, UINT) = nullptr;

Renderer& Renderer::instance() {
    static Renderer inst;
    return inst;
}

void Renderer::initialize() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr,
        nullptr, nullptr, "DummyWindowClass", nullptr };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(
        wc.lpszClassName, "Dummy Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    if (!hWnd) {
        throw std::runtime_error("Failed to create dummy window.");
    }

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount                        = 2;
    sd.BufferDesc.Width                   = 0;
    sd.BufferDesc.Height                  = 0;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = hWnd;
    sd.SampleDesc.Count                   = 1;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    constexpr D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 2,
        D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context);

    if (FAILED(res)) {
        DestroyWindow(hWnd);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        throw std::runtime_error("Failed to create D3D11 device.");
    }

    void** vtable        = *reinterpret_cast<void***>(swapChain);
    void* presentAddress = vtable[8];

    swapChain->Release();
    device->Release();
    context->Release();
    DestroyWindow(hWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    HookManager::instance().createHook(
        "DXGIPresent", presentAddress, &hookPresent, reinterpret_cast<void**>(&m_originalPresent));
    Logger::instance().info("DXGI::Present hooked successfully.");
}

void Renderer::shutdown() {
    if (m_isInitialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        m_isInitialized = false;
    }
}

bool Renderer::onWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!m_isInitialized) {
        return false;
    }

    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    const ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse) {
        switch (uMsg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEMOVE:
            return true;
        }
    }
    if (io.WantCaptureKeyboard) {
        switch (uMsg) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
            return true;
        }
    }
    return false;
}

HRESULT Renderer::hookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    auto& self = instance();

    if (!self.m_isInitialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&self.m_device)))) {
            self.m_device->GetImmediateContext(&self.m_context);

            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            self.m_windowHandle = desc.OutputWindow;

            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
            self.m_device->CreateRenderTargetView(pBackBuffer, nullptr, &self.m_renderTargetView);
            pBackBuffer->Release();

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();

            char windir[MAX_PATH];
            GetWindowsDirectoryA(windir, MAX_PATH);
            const std::string fontPath = std::string(windir) + "\\Fonts\\";

            ImFontConfig font_config;

            io.Fonts->AddFontFromFileTTF((fontPath + "segoeui.ttf").c_str(), 18.0f, &font_config);

            font_config.MergeMode = true;

            io.Fonts->AddFontFromFileTTF((fontPath + "meiryo.ttc").c_str(), 18.0f, &font_config);
            io.Fonts->AddFontFromFileTTF((fontPath + "msyh.ttc").c_str(), 18.0f, &font_config);
            io.Fonts->AddFontFromFileTTF((fontPath + "malgun.ttf").c_str(), 18.0f, &font_config);
            io.Fonts->AddFontFromFileTTF((fontPath + "LeelawUI.ttf").c_str(), 18.0f, &font_config);

            ImGui::GetStyle().CircleTessellationMaxError = 0.1f;
            io.IniFilename                               = nullptr;

            ImGui_ImplWin32_Init(self.m_windowHandle);
            ImGui_ImplDX11_Init(self.m_device, self.m_context);

            WndProcHook::instance().addCallback([&self](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
                return self.onWndProc(hWnd, uMsg, wParam, lParam);
            });

            self.m_isInitialized = true;
            Logger::instance().info("ImGui initialized.");
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    FeatureManager::instance().renderAll();
    NotificationManager::instance().render();

    ImGui::Render();
    self.m_context->OMSetRenderTargets(1, &self.m_renderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return m_originalPresent(pSwapChain, SyncInterval, Flags);
}
