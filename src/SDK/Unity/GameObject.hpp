#pragma once

#include "Component.hpp"
#include "Object.hpp"
#include "Transform.hpp"

namespace UnityEngine {
    class GameObject : public Object {
    public:
        [[nodiscard]] Transform* getTransform() const;

        Component* getComponent(const std::string& typeName) const;
    };
}
