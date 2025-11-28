#include "Transform.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {
    Vector3 Transform::get_position() const {
        return Il2Cpp::invoke<Vector3>(this, "UnityEngine.CoreModule.dll", "UnityEngine", "Transform", "get_position");
    }
}
