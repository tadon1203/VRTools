#pragma once

#include "Object.hpp"
#include "Transform.hpp"

namespace UnityEngine {
    class GameObject : public Object {
    public:
        [[nodiscard]] Transform* getTransform() const;
    };
}
