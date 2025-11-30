#pragma once

#include "APIUser.hpp"
#include "Core/Logger.hpp"
#include "SDK/Il2Cpp/Binding.hpp"
#include "SDK/Unity/GameObject.hpp"

namespace VRC {
    class VRC_Player : public UnityEngine::Component {
    public:
        static Il2CppClass* getStaticClass() {
            static Il2CppClass* klass = nullptr;
            if (!klass) {
                klass = Il2Cpp::findClassByField("Assembly-CSharp", "_laserSelectRegion");
            }
            return klass;
        }

        static Il2CppObject* getStaticType() {
            static Il2CppObject* type = nullptr;
            if (!type) {
                auto* k = getStaticClass();
                type    = Il2Cpp::Exports::il2cpp_type_get_object(Il2Cpp::Exports::il2cpp_class_get_type(k));
                if (!type) {
                    throw std::runtime_error("VRC_Player: Failed to get Static Type Object.");
                }
            }
            return type;
        }

        Core::APIUser* get_user() {
            static const MethodInfo* getterMethod = nullptr;

            if (!getterMethod) {
                getterMethod = Il2Cpp::resolveMethodByReturnType(getStaticClass(), Core::APIUser::getStaticClass());
            }
            return Il2Cpp::invoke<Core::APIUser*>(getterMethod, this);
        }

        static VRC_Player* get(UnityEngine::GameObject* go) {
            if (!go) {
                return nullptr;
            }

            Il2CppObject* type = getStaticType();
            return reinterpret_cast<VRC_Player*>(go->call<Il2CppObject*>("GetComponent", type));
        }
    };
}
