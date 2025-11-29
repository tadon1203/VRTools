#pragma once

#include "SDK/Il2Cpp/Binding.hpp"
#include "SDK/Il2Cpp/Structs.hpp"

namespace UnityEngine {
    enum class CursorLockMode { None = 0, Locked = 1, Confined = 2 };

    class Cursor {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Cursor");

        static void set_lockState(CursorLockMode value) { callStatic("set_lockState", value); }

        static CursorLockMode get_lockState() { return callStatic<CursorLockMode>("get_lockState"); }

        static void set_visible(bool value) { callStatic("set_visible", value); }

        static bool get_visible() { return callStatic<bool>("get_visible"); }
    };
}
