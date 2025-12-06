#pragma once

#include "SDK/Il2Cpp/Binding.hpp"
#include "SDK/Il2Cpp/Structs.hpp"

namespace VRC::Core {
    class APIUser : public Il2CppObject {
    public:
        IL2CPP_BINDING("VRCCore-Standalone.dll", "VRC.Core", "APIUser");

        std::vector<std::string> getTags() {
            static const MethodInfo* m = Il2Cpp::resolveMethod(getStaticClass(), "get_tags", 0);
            auto* list                 = Il2Cpp::invoke<Il2CppList<Il2CppString*>*>(m, this);

            std::vector<std::string> result;
            if (list) {
                auto vec = list->toVector();
                for (auto* s : vec) {
                    if (s) {
                        result.push_back(s->toString());
                    }
                }
            }
            return result;
        }
    };
}
