#pragma once
#include "Component.hpp"
#include "Vector3.hpp"

namespace UnityEngine {
    class Transform : public Component {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Transform");

        Vector3 get_position() { return this->call<Vector3>("get_position"); }
    };
}
