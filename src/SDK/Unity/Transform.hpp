#pragma once
#include "Component.hpp"
#include "Vector3.hpp"

namespace UnityEngine {
    class Transform : public Component {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Transform");

        Vector3 getPosition() { return this->call<Vector3>("get_position"); }
        void setPosition(Vector3 value) { this->call("set_position", value); }

        Vector3 getForward() { return this->call<Vector3>("get_forward"); }
        Vector3 getRight() { return this->call<Vector3>("get_right"); }
        Vector3 getUp() { return this->call<Vector3>("get_up"); }
    };
}
