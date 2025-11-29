#pragma once

#include "../VisualsUtils.hpp"

enum class ESPAnchor { Top, Bottom, Left, Right, Center };

struct ESPContext {
    VisualsUtils::VisualsStyle styleBox2D;
    VisualsUtils::VisualsStyle styleBox3D;
    VisualsUtils::VisualsStyle textStyle;
    VisualsUtils::VisualsStyle boneStyle;
    float padding = 2.0f;
};
