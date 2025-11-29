#pragma once

#include "Vector3.hpp"

namespace UnityEngine {
    struct Bounds {
        Vector3 center;
        Vector3 extents;

        Vector3 getMin() const { return center - extents; }
        Vector3 getMax() const { return center + extents; }
    };
}
