#include "GameObject.hpp"

#include "Wrappers/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {

    Transform* GameObject::getTransform() const {
        auto result = Il2Cpp::invoke<Il2CppObject*>(
            this, "UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_transform");
        return reinterpret_cast<Transform*>(result);
    }
}
