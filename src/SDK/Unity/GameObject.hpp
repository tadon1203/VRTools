#pragma once
#include "Object.hpp"
#include "Transform.hpp"

namespace UnityEngine {
    class GameObject : public Object {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "GameObject");

        Transform* getTransform() { return reinterpret_cast<Transform*>(this->call<Il2CppObject*>("get_transform")); }

        template <typename T>
        T* getComponent() {
            Il2CppObject* typeObj = T::getStaticType();
            if (!typeObj) {
                return nullptr;
            }
            auto* res = this->call<Il2CppObject*>("GetComponent", typeObj);
            return reinterpret_cast<T*>(res);
        }
    };
}
