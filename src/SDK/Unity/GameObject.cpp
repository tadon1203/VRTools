#include "GameObject.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {

    Transform* GameObject::getTransform() const {
        auto result = Il2Cpp::invoke<Il2CppObject*>(
            this, "UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_transform");
        return reinterpret_cast<Transform*>(result);
    }

    Component* GameObject::getComponent(const std::string& typeName) const {
        Il2CppString* str = Il2Cpp::newString(typeName.c_str());
        auto result       = Il2Cpp::invoke<Il2CppObject*>(
            this, "UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponent", str);
        return reinterpret_cast<Component*>(result);
    }
}
