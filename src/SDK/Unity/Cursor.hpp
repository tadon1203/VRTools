#pragma once

#include "SDK/Il2Cpp/Structs.hpp"

namespace UnityEngine {

    enum class CursorLockMode { None = 0, Locked = 1, Confined = 2 };

    class Cursor {
    public:
        static void set_lockState(CursorLockMode value);
        static CursorLockMode get_lockState();

        static void set_visible(bool value);
        static bool get_visible();
    };
}
