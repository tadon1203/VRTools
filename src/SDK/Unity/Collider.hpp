#pragma once

#include "Bounds.hpp"
#include "Component.hpp"

namespace UnityEngine {
    class Collider : public Component {
    public:
        Bounds get_bounds();
    };
}
