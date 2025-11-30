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
                if (!klass) {
                    Logger::instance().error("Failed to resolve VRC.Player!");
                }
            }
            return klass;
        }

        static Il2CppObject* getStaticType() {
            static Il2CppObject* type = nullptr;
            if (!type && getStaticClass()) {
                auto* k = getStaticClass();
                type    = Il2Cpp::Exports::il2cpp_type_get_object(Il2Cpp::Exports::il2cpp_class_get_type(k));
            }
            return type;
        }

        Core::APIUser* get_user() {
            static const MethodInfo* getterMethod = nullptr;

            if (!getterMethod) {
                getterMethod = Il2Cpp::resolveMethodByReturnType(getStaticClass(), Core::APIUser::getStaticClass());
                if (!getterMethod) {
                    Logger::instance().error("Failed to resolve APIUser getter!");
                }
            }

            if (!getterMethod) {
                return nullptr;
            }

            return Il2Cpp::invoke<Core::APIUser*>(getterMethod, this);
        }

        static VRC_Player* get(UnityEngine::GameObject* go) {
            if (!go) {
                return nullptr;
            }
            Il2CppObject* type = getStaticType();
            if (!type) {
                return nullptr;
            }
            return reinterpret_cast<VRC_Player*>(go->call<Il2CppObject*>("GetComponent", type));
        }
    };
}
