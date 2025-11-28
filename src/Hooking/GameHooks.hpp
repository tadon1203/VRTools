#pragma once

#include <cstdint>

#include "SDK/Game/Photon/SendOptions.hpp"
#include "SDK/Il2Cpp/Structs.hpp"
#include "SDK/Unity/Cursor.hpp"

class GameHooks {
public:
    static void initialize();

private:
    static void setupMainLoopHook();
    static void setupNetworkEventHook();
    static void setupRaiseEventHook();
    static void setupCursorHooks();

    static void hookUpdate(void* instance);
    static void hookOnEvent(void* instance, void* eventData);
    static bool hookRaiseEvent(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    static void hookSetLockState(UnityEngine::CursorLockMode value);
    static void hookSetVisible(bool value);

    static void (*m_originalUpdate)(void* instance);
    static void (*m_originalOnEvent)(void* instance, void* eventData);
    static bool (*m_originalRaiseEvent)(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    static void (*m_originalSetLockState)(UnityEngine::CursorLockMode);
    static void (*m_originalSetVisible)(bool);
};
