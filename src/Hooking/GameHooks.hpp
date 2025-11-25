#pragma once

#include <cstdint>

#include "Wrappers/Il2Cpp/Structs.hpp"
#include "Wrappers/Photon/SendOptions.hpp"

class GameHooks {
public:
    static void initialize();

private:
    static void setupMainLoopHook();
    static void setupNetworkEventHook();
    static void setupRaiseEventHook();

    static void hookUpdate(void* instance);
    static void hookOnEvent(void* instance, void* eventData);
    static bool hookRaiseEvent(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    static void (*m_originalUpdate)(void* instance);
    static void (*m_originalOnEvent)(void* instance, void* eventData);
    static bool (*m_originalRaiseEvent)(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);
};
