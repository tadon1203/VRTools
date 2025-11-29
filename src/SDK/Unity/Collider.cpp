#include "Collider.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {
    Bounds Collider::get_bounds() {
        return Il2Cpp::invoke<Bounds>(this, "UnityEngine.PhysicsModule.dll", "UnityEngine", "Collider", "get_bounds");
    }
}
