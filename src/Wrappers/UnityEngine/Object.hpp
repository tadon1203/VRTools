#pragma once

#include <string>
#include <vector>

#include "Wrappers/Il2Cpp/Structs.hpp"

namespace UnityEngine {
    class Object : public Il2CppObject {
    public:
        [[nodiscard]] std::string getName() const;
        static std::vector<Object*> findObjectsOfType(Il2CppObject* type, bool includeInactive);
    };
}
