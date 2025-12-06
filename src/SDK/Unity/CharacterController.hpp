#pragma once
#include "Component.hpp"

namespace UnityEngine {
    class CharacterController : public Component {
    public:
        IL2CPP_BINDING("UnityEngine.PhysicsModule.dll", "UnityEngine", "CharacterController");

        void setEnabled(bool value) { this->call("set_enabled", value); }
        bool getEnabled() { return this->call<bool>("get_enabled"); }
    };
}
