#pragma once

#include <string>

#include <imgui.h>

namespace RenderUtils {
    void renderText(
        ImDrawList* dl, ImVec2 pos, const std::string& text, ImColor color, bool outline = true, float fontSize = 0.0f);

}
