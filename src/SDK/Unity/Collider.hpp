#pragma once
#include "Bounds.hpp"
#include "Component.hpp"

namespace UnityEngine {
    class Collider : public Component {
    public:
        IL2CPP_BINDING("UnityEngine.PhysicsModule.dll", "UnityEngine", "Collider");

        Bounds get_bounds() { return this->call<Bounds>("get_bounds"); }
    };
}
