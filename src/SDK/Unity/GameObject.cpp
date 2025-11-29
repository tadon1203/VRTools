#include "GameObject.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace UnityEngine {

    Transform* GameObject::getTransform() const {
        auto result = Il2Cpp::invoke<Il2CppObject*>(
            this, "UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "get_transform");
        return reinterpret_cast<Transform*>(result);
    }

    Component* GameObject::getComponent(const char* asmName, const char* ns, const char* cls) const {
        static const MethodInfo* getCompMethod = nullptr;
        if (!getCompMethod) {
            getCompMethod = Il2Cpp::resolveMethod(
                "UnityEngine.CoreModule.dll", "UnityEngine", "GameObject", "GetComponent", 1, "System.Type");
        }

        Il2CppClass* klass = Il2Cpp::findClass(asmName, ns, cls);
        if (!klass) {
            return nullptr;
        }

        Il2CppObject* typeObj = Il2Cpp::getSystemType(klass);
        if (!typeObj) {
            return nullptr;
        }

        auto result = Il2Cpp::invoke<Il2CppObject*>(getCompMethod, const_cast<GameObject*>(this), typeObj);
        return reinterpret_cast<Component*>(result);
    }
}
