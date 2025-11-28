#pragma once

#include "Object.hpp"
#include "Vector3.hpp"

namespace UnityEngine {
    class Transform : public Object {
    public:
        [[nodiscard]] Vector3 get_position() const;
    };
}
