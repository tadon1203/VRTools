#pragma once

#include <MinHook.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

class HookManager {
public:
    static HookManager& instance();

    HookManager(const HookManager&)            = delete;
    HookManager& operator=(const HookManager&) = delete;

    void initialize();
    void shutdown();

    template <typename T>
    void createHook(const std::string& name, void* target, void* detour, T** original) {
        if (m_hooks.count(name)) {
            throw std::runtime_error("Hook with name '" + name + "' already exists.");
        }

        const MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<void**>(original));

        if (status != MH_OK) {
            const char* errorStr = MH_StatusToString(status);
            throw std::runtime_error("Failed to create hook '" + name + "': " + std::string(errorStr));
        }

        m_hooks[name] = target;
    }

    void enableAll();

private:
    HookManager()  = default;
    ~HookManager() = default;

    std::unordered_map<std::string, void*> m_hooks;
};
