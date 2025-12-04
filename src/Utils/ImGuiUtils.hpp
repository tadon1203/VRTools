#pragma once

#include <string>

#include <imgui.h>

namespace ImGuiUtils {

    inline ImVec2 getTextSize(const std::string& text, float fontSize = 0.0f) {
        if (fontSize <= 0.0f) {
            fontSize = ImGui::GetFontSize();
        }
        return ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text.c_str());
    }

    inline ImColor fadeColor(ImColor color, float alphaFactor) {
        if (alphaFactor >= 1.0f) {
            return color;
        }
        if (alphaFactor < 0.0f) {
            alphaFactor = 0.0f;
        }
        color.Value.w *= alphaFactor;
        return color;
    }
}
