#include "Cursor.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {

    void Cursor::set_lockState(CursorLockMode value) {
        Il2Cpp::invoke<void>("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "set_lockState", value);
    }

    CursorLockMode Cursor::get_lockState() {
        auto result = Il2Cpp::invoke<int32_t>("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "get_lockState");
        return static_cast<CursorLockMode>(result);
    }

    void Cursor::set_visible(bool value) {
        Il2Cpp::invoke<void>("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "set_visible", value);
    }

    bool Cursor::get_visible() {
        return Il2Cpp::invoke<bool>("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor", "get_visible");
    }

}
