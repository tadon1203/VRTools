#pragma once
#include "Object.hpp"

namespace UnityEngine {
    class Component : public Object {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Component");
    };
}
