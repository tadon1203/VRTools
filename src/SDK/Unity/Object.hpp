#pragma once
#include <string>
#include <vector>

#include "SDK/Il2Cpp/Binding.hpp"
#include "SDK/Il2Cpp/Structs.hpp"

namespace UnityEngine {
    class Object : public Il2CppObject {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Object");

        [[nodiscard]] std::string getName() {
            auto* s = this->call<Il2CppString*>("get_name");
            return s ? s->toString() : "";
        }

        static std::vector<Object*> findObjectsOfType(Il2CppObject* type, bool includeInactive = false) {
            if (!type) {
                return {};
            }
            auto* arr = callStatic<Il2CppArray<Object*>*>("FindObjectsOfType", type, includeInactive);
            return arr ? arr->toVector() : std::vector<Object*>{};
        }
    };
}
