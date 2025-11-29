#pragma once
#include "Collider.hpp"

namespace UnityEngine {
    class CharacterController : public Collider {
    public:
        IL2CPP_BINDING("UnityEngine.PhysicsModule.dll", "UnityEngine", "CharacterController");
    };
}
