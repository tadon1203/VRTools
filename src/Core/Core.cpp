#include "Core.hpp"

#include "CrashHandler.hpp"
#include "Features/FeatureManager.hpp"
#include "Features/Modules/Visuals/ESP.hpp"
#include "HUD/Components/LogComponent.hpp"
#include "HUD/HUDManager.hpp"
#include "Hooking/GameHooks.hpp"
#include "Hooking/HookManager.hpp"
#include "Hooking/WndProcHook.hpp"
#include "Input/InputManager.hpp"
#include "Logger.hpp"
#include "Rendering/Renderer.hpp"
#include "SDK/Il2Cpp/Il2Cpp.hpp"
#include "Settings/SettingsManager.hpp"
#include "UI/Menu/MenuManager.hpp"

Core& Core::instance() {
    static Core inst;
    return inst;
}

void Core::run() {
    m_isRunning = true;
    initialize();

    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    shutdown();
}

void Core::requestShutdown() { m_isRunning = false; }
bool Core::isRunning() const { return m_isRunning; }

void Core::initialize() {
    try {
        Logger::instance().info("Core initializing...");

        CrashHandler::instance().initialize();

        Il2Cpp::initialize();
        HookManager::instance().initialize();
        WndProcHook::instance().initialize();
        Renderer::instance().initialize();
        InputManager::instance().initialize();
        GameHooks::initialize();

        auto& fm = FeatureManager::instance();
        fm.registerFeature<ESP>();
        fm.initializeAll();

        auto& hud = HUDManager::instance();
        hud.registerComponent<LogComponent>();
        hud.initializeAll();

        SettingsManager::instance().registerHandler(&fm);
        SettingsManager::instance().registerHandler(&hud);

        MenuManager::instance().initialize();
        InputManager::instance().registerKeybind(VK_INSERT, [] { MenuManager::instance().toggle(); });

        SettingsManager::instance().loadAll();

        for (const auto& feature : fm.getFeatures()) {
            if (int vkCode = feature->getKeybind(); vkCode != 0) {
                InputManager::instance().registerKeybind(vkCode, [feature = feature.get()] { feature->onKeybind(); });
            }
        }

        HookManager::instance().enableAll();

        Logger::instance().info("Core initialized successfully.");
        NotificationManager::instance().info("Menu", "Press [INSERT] to open the menu.", 8.0f);

    } catch (const std::exception& e) {
        Logger::instance().error("Fatal error: {}", e.what());
        m_isRunning = false;
    }
}

void Core::shutdown() {
    Logger::instance().info("Core shutting down...");

    SettingsManager::instance().saveAll();

    HookManager::instance().shutdown();
    Renderer::instance().shutdown();
    WndProcHook::instance().shutdown();

    Logger::instance().info("Core shutdown.");
}
