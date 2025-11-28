#pragma once

#include <string>

#include <imgui.h>

namespace HUDUtils {

    inline ImVec2 getTextSize(const std::string& text) {
        return ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, text.c_str());
    }

    inline ImColor fadeColor(ImColor color, float alphaFactor) {
        if (alphaFactor >= 1.0f) {
            return color;
        }

        color.Value.w *= alphaFactor;
        return color;
    }

    inline void drawText(ImDrawList* dl, const std::string& text, ImVec2 pos, ImColor color, bool shadow = true) {
        if (shadow) {
            ImColor shadowColor = ImColor(0.0f, 0.0f, 0.0f, color.Value.w);
            dl->AddText(ImVec2(pos.x + 1.0f, pos.y + 1.0f), shadowColor, text.c_str());
        }
        dl->AddText(pos, color, text.c_str());
    }
}
