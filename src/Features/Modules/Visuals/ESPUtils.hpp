#pragma once

#include <string>

#include <imgui.h>

#include "ESPDef.hpp"

namespace ESPUtils {

    void renderBox2D(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style);

    void renderBox3D(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style);

    void renderSkeleton(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style);

    void renderText(
        ImDrawList* dl, const ESPObject& obj, const std::string& text, const ESPStyle& style, TextAnchor anchor);

}
