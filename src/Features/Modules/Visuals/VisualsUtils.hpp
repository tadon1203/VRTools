#pragma once

#include <array>
#include <string>
#include <vector>

#include <imgui.h>

#include "Utils/ColorUtils.hpp"

namespace VisualsUtils {

    enum class ColorMode { Solid, Rainbow, Gradient };

    struct VisualsStyle {
        ColorMode colorMode = ColorMode::Solid;
        Color primaryColor  = Color(1, 1, 1, 1);
        Gradient gradient;

        float thickness = 1.0f;
        bool outline    = true;

        bool textShadow = true;
        float fontSize  = 13.0f;

        float time           = 0.0f;
        float animationSpeed = 1.0f;

        [[nodiscard]] Color getColor(float offset = 0.0f) const {
            float t = (time * animationSpeed) + offset;
            if (colorMode == ColorMode::Rainbow) {
                return Color::rainbow(t, 1.0f, 1.0f, 1.0f, primaryColor.a);
            }
            if (colorMode == ColorMode::Gradient) {
                return gradient.get(t);
            }
            return primaryColor;
        }
    };

    struct ScreenRect {
        ImVec2 min; // Top Left
        ImVec2 max; // Bottom Right

        [[nodiscard]] ImVec2 getTopCenter() const { return { min.x + (max.x - min.x) * 0.5f, min.y }; }
        [[nodiscard]] ImVec2 getBottomCenter() const { return { min.x + (max.x - min.x) * 0.5f, max.y }; }
    };

    struct ScreenCube {
        // 0-3: Bottom Face (FL, FR, BR, BL)
        // 4-7: Top Face    (FL, FR, BR, BL)
        std::array<ImVec2, 8> corners;
    };

    inline void drawBox2D(ImDrawList* dl, const ScreenRect& rect, const VisualsStyle& style) {
        ImU32 color = style.getColor().toU32();

        if (style.outline) {
            dl->AddRect(rect.min, rect.max, ImColor(0, 0, 0, 255), 0.0f, 0, style.thickness + 2.0f);
        }

        dl->AddRect(rect.min, rect.max, color, 0.0f, 0, style.thickness);
    }

    inline void drawBox3D(ImDrawList* dl, const ScreenCube& cube, const VisualsStyle& style) {
        static constexpr std::pair<int, int> lines[] = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 },
            { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } };

        ImU32 color = style.getColor().toU32();

        for (auto [a, b] : lines) {
            if (style.outline) {
                dl->AddLine(cube.corners[a], cube.corners[b], ImColor(0, 0, 0, 255), style.thickness + 2.0f);
            }
            dl->AddLine(cube.corners[a], cube.corners[b], color, style.thickness);
        }
    }

    inline void drawSkeleton(
        ImDrawList* dl, const std::vector<std::pair<ImVec2, ImVec2>>& bones, const VisualsStyle& style) {
        ImU32 color = style.getColor().toU32();
        for (const auto& bone : bones) {
            if (style.outline) {
                dl->AddLine(bone.first, bone.second, ImColor(0, 0, 0, 200), style.thickness + 2.0f);
            }
            dl->AddLine(bone.first, bone.second, color, style.thickness);
        }
    }

    inline void drawText(
        ImDrawList* dl, ImVec2 pos, const std::string& text, const VisualsStyle& style, bool center = true) {
        if (text.empty()) {
            return;
        }

        ImU32 color = style.getColor().toU32();

        if (center) {
            ImVec2 size = ImGui::GetFont()->CalcTextSizeA(style.fontSize, FLT_MAX, 0.0f, text.c_str());
            pos.x -= size.x * 0.5f;
        }

        if (style.textShadow) {
            dl->AddText(
                ImGui::GetFont(), style.fontSize, { pos.x + 1, pos.y + 1 }, ImColor(0, 0, 0, 255), text.c_str());
        }
        dl->AddText(ImGui::GetFont(), style.fontSize, pos, color, text.c_str());
    }
}
