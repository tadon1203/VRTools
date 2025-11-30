#include "GameHooks.hpp"

#include "Core/Events/GameEvents.hpp"
#include "Core/Events/NetworkEvents.hpp"
#include "Core/Logger.hpp"
#include "Features/FeatureManager.hpp"
#include "Hooking/HookManager.hpp"
#include "Input/CursorManager.hpp"
#include "Input/InputManager.hpp"
#include "SDK/Game/Photon/EventData.hpp"
#include "SDK/Game/PlayerManager.hpp"
#include "SDK/Il2Cpp/Il2Cpp.hpp"
#include "SDK/Unity/Cursor.hpp"
#include "UI/NotificationManager.hpp"
#include "Utils/PatternScan.hpp"

void (*GameHooks::m_originalUpdate)(void*)                                                                    = nullptr;
void (*GameHooks::m_originalOnEvent)(void*, void*)                                                            = nullptr;
bool (*GameHooks::m_originalRaiseEvent)(uint8_t, Il2CppObject*, void* raiseEventOptions, Photon::SendOptions) = nullptr;
void (*GameHooks::m_originalSetLockState)(UnityEngine::CursorLockMode)                                        = nullptr;
void (*GameHooks::m_originalSetVisible)(bool)                                                                 = nullptr;

void GameHooks::initialize() {
    setupMainLoopHook();
    setupNetworkEventHook();
    setupRaiseEventHook();
    setupCursorHooks();
    Logger::instance().info("Game hooks initialized.");
}

void GameHooks::setupMainLoopHook() {
    const MethodInfo* updateMethod = Il2Cpp::resolveMethod("VRC.Udon.dll", "VRC.Udon", "UdonManager", "Update", 0);

    HookManager::instance().createHook("MainLoopUpdate", updateMethod->methodPointer, &hookUpdate, &m_originalUpdate);
}

void GameHooks::setupNetworkEventHook() {
    const uintptr_t onEventAddress = Utils::findPattern("GameAssembly.dll", "40 55 56 57 41 56 48 8D 6C 24 F8");

    HookManager::instance().createHook(
        "NetworkManager_OnEvent", reinterpret_cast<void*>(onEventAddress), &hookOnEvent, &m_originalOnEvent);
}

void GameHooks::setupRaiseEventHook() {
    const uintptr_t raiseEventAddress =
        Utils::findPattern("GameAssembly.dll", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 88");

    HookManager::instance().createHook(
        "PhotonNetwork_RaiseEvent", reinterpret_cast<void*>(raiseEventAddress), &hookRaiseEvent, &m_originalRaiseEvent);
}

void GameHooks::setupCursorHooks() {
    const MethodInfo* setLockStateMethod =
        Il2Cpp::resolveMethod("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "set_lockState", 1);

    const MethodInfo* setVisibleMethod =
        Il2Cpp::resolveMethod("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "set_visible", 1);

    HookManager::instance().createHook(
        "Cursor_set_lockState", setLockStateMethod->methodPointer, &hookSetLockState, &m_originalSetLockState);

    HookManager::instance().createHook(
        "Cursor_set_visible", setVisibleMethod->methodPointer, &hookSetVisible, &m_originalSetVisible);
}
void GameHooks::hookUpdate(void* instance) {
    try {
        InputManager::instance().update();
        PlayerManager::instance().update();
        NotificationManager::instance().update();
        CursorManager::instance().update();

        FrameUpdateEvent e;
        EventManager::instance().fire(e);

    } catch (const std::exception& e) {
        Logger::instance().error("GameHooks: Exception in Update loop: {}", e.what());
    }

    if (m_originalUpdate) {
        m_originalUpdate(instance);
    }
}

void GameHooks::hookOnEvent(void* instance, void* eventDataPtr) {
    auto eventData = static_cast<Photon::EventData*>(eventDataPtr);

    if (eventData) {
        PhotonEventEvent e(eventData);
        EventManager::instance().fire(e);

        if (e.isCancelled()) {
            return;
        }
    }

    if (m_originalOnEvent) {
        m_originalOnEvent(instance, eventDataPtr);
    }
}

bool GameHooks::hookRaiseEvent(
    uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {

    RaiseEventEvent e(eventCode, content, raiseEventOptions, sendOptions);
    EventManager::instance().fire(e);

    if (e.isCancelled()) {
        return false;
    }

    if (m_originalRaiseEvent) {
        return m_originalRaiseEvent(eventCode, content, raiseEventOptions, sendOptions);
    }
    return false;
}

void GameHooks::hookSetLockState(UnityEngine::CursorLockMode value) {
    CursorManager::instance().onSetLockState(value);

    if (m_originalSetLockState) {
        m_originalSetLockState(value);
    }
}

void GameHooks::hookSetVisible(bool value) {
    CursorManager::instance().onSetVisible(value);

    if (m_originalSetVisible) {
        m_originalSetVisible(value);
    }
}
