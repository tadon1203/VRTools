#include "GameHooks.hpp"

#include "Core/Logger.hpp"
#include "Features/Framework/FeatureManager.hpp"
#include "Hooking/HookManager.hpp"
#include "Input/InputManager.hpp"
#include "SDK/Game/Photon/EventData.hpp"
#include "SDK/Il2Cpp/Il2Cpp.hpp"
#include "UI/NotificationManager.hpp"
#include "Utils/PatternScan.hpp"

void (*GameHooks::m_originalUpdate)(void*) = nullptr;

void (*GameHooks::m_originalOnEvent)(void*, void*) = nullptr;

bool (*GameHooks::m_originalRaiseEvent)(uint8_t, Il2CppObject*, void* raiseEventOptions, Photon::SendOptions) = nullptr;

void GameHooks::initialize() {
    try {
        setupMainLoopHook();
        setupNetworkEventHook();
        setupRaiseEventHook();
        Logger::instance().info("Game hooks initialized.");
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to initialize game hooks: {}", e.what());
    }
}

void GameHooks::setupMainLoopHook() {
    const MethodInfo* updateMethod = Il2Cpp::resolveMethod("VRC.Udon.dll", "VRC.Udon", "UdonManager", "Update", 0);
    if (!updateMethod) {
        throw std::runtime_error("Failed to find update method.");
    }

    HookManager::instance().createHook("MainLoopUpdate", updateMethod->methodPointer, &hookUpdate, &m_originalUpdate);
}

void GameHooks::setupNetworkEventHook() {
    const uintptr_t onEventAddress = Utils::findPattern("GameAssembly.dll", "40 55 56 57 41 56 48 8D 6C 24 F8");
    if (!onEventAddress) {
        throw std::runtime_error("Could not find NetworkManager::OnEvent pattern.");
    }

    HookManager::instance().createHook(
        "NetworkManager_OnEvent", reinterpret_cast<void*>(onEventAddress), &hookOnEvent, &m_originalOnEvent);
}

void GameHooks::setupRaiseEventHook() {
    const uintptr_t raiseEventAddress =
        Utils::findPattern("GameAssembly.dll", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 88");
    if (!raiseEventAddress) {
        throw std::runtime_error("Could not find PhotonNetwork::RaiseEvent pattern.");
    }

    HookManager::instance().createHook(
        "PhotonNetwork_RaiseEvent", reinterpret_cast<void*>(raiseEventAddress), &hookRaiseEvent, &m_originalRaiseEvent);
}

void GameHooks::hookUpdate(void* instance) {
    InputManager::instance().update();
    FeatureManager::instance().updateAll();
    NotificationManager::instance().update();

    if (m_originalUpdate) {
        m_originalUpdate(instance);
    }
}

void GameHooks::hookOnEvent(void* instance, void* eventDataPtr) {
    auto eventData          = static_cast<Photon::EventData*>(eventDataPtr);
    bool shouldCallOriginal = true;

    if (eventData) {
        shouldCallOriginal = FeatureManager::instance().onEventAll(eventData);
    }

    if (shouldCallOriginal && m_originalOnEvent) {
        m_originalOnEvent(instance, eventDataPtr);
    }
}

bool GameHooks::hookRaiseEvent(
    uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {
    bool shouldCallOriginal =
        FeatureManager::instance().onRaiseEventAll(eventCode, content, raiseEventOptions, sendOptions);

    if (shouldCallOriginal && m_originalRaiseEvent) {
        return m_originalRaiseEvent(eventCode, content, raiseEventOptions, sendOptions);
    }

    return false;
}
