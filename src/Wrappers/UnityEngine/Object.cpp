#include "Object.hpp"

#include "Wrappers/Il2Cpp/Il2Cpp.hpp"

std::string UnityEngine::Object::getName() const {
    auto result =
        Il2Cpp::invoke<Il2CppString*>(this, "UnityEngine.CoreModule.dll", "UnityEngine", "Object", "get_name");
    return result->toString();
}

std::vector<UnityEngine::Object*> UnityEngine::Object::findObjectsOfType(Il2CppObject* type, bool includeInactive) {
    if (!type) {
        return {};
    }

    auto result = Il2Cpp::invoke<Il2CppArray<Object*>*>(
        "UnityEngine.CoreModule.dll", "UnityEngine", "Object", "FindObjectsOfType", type, includeInactive);
    return result->toVector();
}
