#pragma once

#include <array>
#include <string>
#include <vector>

#include <imgui.h>

#include "Utils/ColorUtils.hpp"

namespace VisualsUtils {

    enum class ColorMode { Solid, Rainbow, Gradient, Rank };

    struct VisualsStyle {
        ColorMode colorMode = ColorMode::Solid;
        Color primaryColor  = Color(1, 1, 1, 1);
        Gradient gradient;

        float thickness = 1.0f;
        bool outline    = true;

        bool textOutline = true;
        float fontSize   = 13.0f;

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
        ImVec2 min;
        ImVec2 max;
        [[nodiscard]] ImVec2 getTopCenter() const { return { min.x + (max.x - min.x) * 0.5f, min.y }; }
        [[nodiscard]] ImVec2 getBottomCenter() const { return { min.x + (max.x - min.x) * 0.5f, max.y }; }
    };

    struct ScreenCube {
        std::array<ImVec2, 8> corners;
    };

    inline void drawBox2D(ImDrawList* dl, const ScreenRect& rect, const VisualsStyle& style) {
        if (style.outline) {
            // Draw slightly larger black box behind
            dl->AddRect(rect.min, rect.max, ImColor(0, 0, 0, 255), 0.0f, 0, style.thickness + 2.0f);
        }

        if (style.colorMode == ColorMode::Solid) {
            dl->AddRect(rect.min, rect.max, style.getColor().toU32(), 0.0f, 0, style.thickness);
        } else {
            // waving color

            float t   = style.thickness;
            ImVec2 tl = rect.min;
            ImVec2 br = rect.max;
            ImVec2 tr = { br.x, tl.y };
            ImVec2 bl = { tl.x, br.y };

            ImU32 c_tl = style.getColor(0.00f).toU32();
            ImU32 c_tr = style.getColor(0.25f).toU32();
            ImU32 c_br = style.getColor(0.50f).toU32();
            ImU32 c_bl = style.getColor(0.75f).toU32();

            dl->AddRectFilledMultiColor(tl, ImVec2(tr.x, tr.y + t), c_tl, c_tr, c_tr, c_tl);

            dl->AddRectFilledMultiColor(ImVec2(tr.x - t, tr.y), br, c_tr, c_tr, c_br, c_br);

            dl->AddRectFilledMultiColor(ImVec2(bl.x, bl.y - t), br, c_bl, c_br, c_br, c_bl);

            dl->AddRectFilledMultiColor(tl, ImVec2(bl.x + t, bl.y), c_tl, c_tl, c_bl, c_bl);
        }
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

        float offset = 0.0f;
        float step   = (style.colorMode == ColorMode::Solid) ? 0.0f : (0.5f / (float) bones.size());

        for (const auto& bone : bones) {
            ImU32 color = style.getColor(offset).toU32();

            if (style.outline) {
                dl->AddLine(bone.first, bone.second, ImColor(0, 0, 0, 200), style.thickness + 2.0f);
            }
            dl->AddLine(bone.first, bone.second, color, style.thickness);

            offset += step;
        }
    }

    inline void drawText(
        ImDrawList* dl, ImVec2 pos, const std::string& text, const VisualsStyle& style, bool center = true) {
        if (text.empty()) {
            return;
        }

        ImU32 color = style.getColor().toU32();
        ImU32 black = ImColor(0, 0, 0, 255);

        if (center) {
            ImVec2 size = ImGui::GetFont()->CalcTextSizeA(style.fontSize, FLT_MAX, 0.0f, text.c_str());
            pos.x -= size.x * 0.5f;
        }

        if (style.textOutline) {
            dl->AddText(ImGui::GetFont(), style.fontSize, { pos.x - 1, pos.y - 1 }, black, text.c_str());
            dl->AddText(ImGui::GetFont(), style.fontSize, { pos.x + 1, pos.y - 1 }, black, text.c_str());
            dl->AddText(ImGui::GetFont(), style.fontSize, { pos.x - 1, pos.y + 1 }, black, text.c_str());
            dl->AddText(ImGui::GetFont(), style.fontSize, { pos.x + 1, pos.y + 1 }, black, text.c_str());
        }

        dl->AddText(ImGui::GetFont(), style.fontSize, pos, color, text.c_str());
    }
}
