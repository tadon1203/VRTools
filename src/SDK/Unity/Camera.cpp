#include "Camera.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {

    Camera* Camera::get_main() {
        auto result = Il2Cpp::invoke<Il2CppObject*>("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "get_main");
        return reinterpret_cast<Camera*>(result);
    }

    Vector3 Camera::WorldToScreenPoint(Vector3 position) {
        return Il2Cpp::invoke<Vector3>(
            this, "UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "WorldToScreenPoint", position);
    }
}
