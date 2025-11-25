#include "HookManager.hpp"

HookManager& HookManager::instance() {
    static HookManager inst;
    return inst;
}

void HookManager::initialize() {
    const MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        throw std::runtime_error("Failed to initialize MinHook.");
    }
}

void HookManager::shutdown() {
    m_hooks.clear();
    MH_Uninitialize();
}

void HookManager::enableAll() {
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        throw std::runtime_error("Failed to enable all hooks.");
    }
}
