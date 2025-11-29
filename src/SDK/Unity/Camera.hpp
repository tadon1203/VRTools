#pragma once
#include "Component.hpp"
#include "Vector3.hpp"

namespace UnityEngine {
    class Camera : public Component {
    public:
        IL2CPP_BINDING("UnityEngine.CoreModule.dll", "UnityEngine", "Camera");

        static Camera* get_main() { return callStatic<Camera*>("get_main"); }

        Vector3 worldToScreenPoint(Vector3 position) { return this->call<Vector3>("WorldToScreenPoint", position); }
    };
}
