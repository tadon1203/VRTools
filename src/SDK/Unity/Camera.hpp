#pragma once

#include "Object.hpp"
#include "Vector3.hpp"

namespace UnityEngine {
    class Camera : public Object {
    public:
        static Camera* get_main();
        Vector3 WorldToScreenPoint(Vector3 position);
    };
}
