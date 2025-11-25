#include "Core.hpp"

#include "Features/FeatureManager.hpp"
#include "Features/LogWindow.hpp"
#include "Features/Menu.hpp"
#include "Hooking/GameHooks.hpp"
#include "Hooking/HookManager.hpp"
#include "Hooking/WndProcHook.hpp"
#include "Input/InputManager.hpp"
#include "Logger.hpp"
#include "Rendering/Renderer.hpp"
#include "Settings/SettingsManager.hpp"
#include "Wrappers/Il2Cpp/Il2Cpp.hpp"

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

        Il2Cpp::initialize();
        HookManager::instance().initialize();
        WndProcHook::instance().initialize();
        Renderer::instance().initialize();
        InputManager::instance().initialize();
        GameHooks::initialize();

        auto& fm = FeatureManager::instance();
        fm.registerFeature<Menu>();
        fm.registerFeature<LogWindow>();

        fm.initializeAll();

        auto settings = SettingsManager::instance().loadFromFile();
        fm.loadConfig(settings);

        auto currentConfig = fm.saveConfig();
        SettingsManager::instance().saveToFile(currentConfig);

        for (const auto& feature : fm.getFeatures()) {
            if (auto vkCode = feature->getKeybind()) {
                InputManager::instance().registerKeybind(*vkCode, [feature = feature.get()] { feature->onKeybind(); });
            }
        }

        HookManager::instance().enableAll();
        Logger::instance().info("Core initialized successfully.");

    } catch (const std::exception& e) {
        Logger::instance().error("Fatal error: {}", e.what());
        m_isRunning = false;
    }
}

void Core::shutdown() {
    Logger::instance().info("Core shutting down...");

    auto settings = FeatureManager::instance().saveConfig();
    SettingsManager::instance().saveToFile(settings);

    HookManager::instance().shutdown();
    Renderer::instance().shutdown();
    WndProcHook::instance().shutdown();

    Logger::instance().info("Core shutdown.");
}
