#pragma once

#include <array>
#include <string>
#include <vector>

#include <imgui.h>

#include "Utils/ColorUtils.hpp"

namespace VisualsUtils {

    // 1. Removed ColorMode::Wave
    enum class ColorMode { Solid, Rainbow, Gradient, Rank };
    enum class TextAnchor { Top, Bottom, Left, Right, Center };

    struct ESPStyle {
        bool enabled = true;

        ColorMode colorMode  = ColorMode::Solid;
        Color primaryColor   = Color(1.0f, 1.0f, 1.0f, 1.0f);
        Color secondaryColor = Color(0.0f, 0.6f, 1.0f, 1.0f);
        Gradient gradient;

        float animationSpeed = 2.0f;

        float thickness = 1.0f;
        float rounding  = 0.0f;
        bool outline    = true;

        bool filled     = false;
        float fillAlpha = 0.3f;

        bool textOutline = true;
        float fontSize   = 13.0f;

        [[nodiscard]] Color getColor(float offset = 0.0f) const {
            float t = (static_cast<float>(ImGui::GetTime()) * animationSpeed) + offset;

            switch (colorMode) {
            case ColorMode::Rainbow:
                return Color::rainbow(t, 1.0f, 1.0f, 1.0f, primaryColor.a);
            case ColorMode::Gradient:
                return gradient.get(t);
            // 2. Removed ColorMode::Wave case
            case ColorMode::Rank:
                return primaryColor;
            default:
                return primaryColor;
            }
        }
    };

    struct ESPObject {
        ImVec2 rectMin;
        ImVec2 rectMax;

        std::array<ImVec2, 8> corners3d;
        bool is3dValid = false;

        std::vector<std::pair<ImVec2, ImVec2>> bones;

        float distance = 0.0f;
    };

    struct CornerColors {
        ImU32 tl, tr, bl, br;
    };

    inline CornerColors calculateBoxColors(const ESPStyle& style) {
        float time = static_cast<float>(ImGui::GetTime()) * style.animationSpeed;
        CornerColors c{};

        if (style.colorMode == ColorMode::Solid || style.colorMode == ColorMode::Rank) {
            ImU32 col = style.primaryColor.toU32();
            c.tl = c.tr = c.bl = c.br = col;
        } else if (style.colorMode == ColorMode::Rainbow) {
            c.tl = Color::rainbow(time + 0.00f, 1.0f).toU32();
            c.tr = Color::rainbow(time + 0.25f, 1.0f).toU32();
            c.br = Color::rainbow(time + 0.50f, 1.0f).toU32();
            c.bl = Color::rainbow(time + 0.75f, 1.0f).toU32();
        } else if (style.colorMode == ColorMode::Gradient) {
            c.tl = style.gradient.get(time + 0.00f).toU32();
            c.tr = style.gradient.get(time + 0.25f).toU32();
            c.br = style.gradient.get(time + 0.50f).toU32();
            c.bl = style.gradient.get(time + 0.75f).toU32();
        }

        return c;
    }

    inline void renderBox2D(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style) {
        if (!style.enabled) {
            return;
        }

        CornerColors cols = calculateBoxColors(style);

        if (style.outline) {
            dl->AddRect(obj.rectMin, obj.rectMax, ImColor(0, 0, 0, 255), style.rounding, 0, style.thickness + 2.0f);
        }

        if (style.filled) {
            auto fade = [&](ImU32 c) {
                ImColor col(c);
                col.Value.w *= style.fillAlpha;
                return static_cast<ImU32>(col);
            };
            dl->AddRectFilledMultiColor(
                obj.rectMin, obj.rectMax, fade(cols.tl), fade(cols.tr), fade(cols.br), fade(cols.bl));
        }

        float t    = style.thickness;
        ImVec2 min = obj.rectMin;
        ImVec2 max = obj.rectMax;

        dl->AddRectFilledMultiColor(min, ImVec2(max.x, min.y + t), cols.tl, cols.tr, cols.tr, cols.tl);
        dl->AddRectFilledMultiColor(ImVec2(min.x, max.y - t), max, cols.bl, cols.br, cols.br, cols.bl);
        dl->AddRectFilledMultiColor(
            ImVec2(min.x, min.y + t), ImVec2(min.x + t, max.y - t), cols.tl, cols.tl, cols.bl, cols.bl);
        dl->AddRectFilledMultiColor(
            ImVec2(max.x - t, min.y + t), ImVec2(max.x, max.y - t), cols.tr, cols.tr, cols.br, cols.br);
    }

    inline void renderBox3D(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style) {
        if (!style.enabled || !obj.is3dValid) {
            return;
        }

        // 3D Edge Indices
        static constexpr std::pair<int, int> edges[] = {
            { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // Bottom
            { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // Top
            { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } // Sides
        };

        // 3D Face Indices
        static constexpr int faces[6][4] = {
            { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, // Bot, Top
            { 0, 1, 5, 4 }, { 3, 2, 6, 7 }, // Front, Back
            { 0, 3, 7, 4 }, { 1, 2, 6, 5 } // Left, Right
        };

        Color mainCol = style.getColor();

        if (style.filled) {
            ImU32 fillCol = mainCol.toU32();
            ImColor c(fillCol);
            c.Value.w *= style.fillAlpha;
            fillCol = c;

            for (const auto& face : faces) {
                ImVec2 points[4];
                bool valid = true;
                for (int i = 0; i < 4; i++) {
                    points[i] = obj.corners3d[face[i]];
                    if (points[i].x < -10000 || points[i].y < -10000) {
                        valid = false;
                    }
                }
                if (valid) {
                    dl->AddConvexPolyFilled(points, 4, fillCol);
                }
            }
        }

        ImU32 uCol  = mainCol.toU32();
        ImU32 black = ImColor(0, 0, 0, 255);

        for (auto [a, b] : edges) {
            ImVec2 p1 = obj.corners3d[a];
            ImVec2 p2 = obj.corners3d[b];

            if (p1.x < -10000 || p2.x < -10000) {
                continue;
            }

            if (style.outline) {
                dl->AddLine(p1, p2, black, style.thickness + 2.0f);
            }
            dl->AddLine(p1, p2, uCol, style.thickness);
        }
    }

    inline void renderSkeleton(ImDrawList* dl, const ESPObject& obj, const ESPStyle& style) {
        if (!style.enabled || obj.bones.empty()) {
            return;
        }

        Color col   = style.getColor();
        ImU32 uCol  = col.toU32();
        ImU32 black = ImColor(0, 0, 0, 255);

        for (const auto& bone : obj.bones) {
            if (style.outline) {
                dl->AddLine(bone.first, bone.second, black, style.thickness + 2.0f);
            }
            dl->AddLine(bone.first, bone.second, uCol, style.thickness);
        }
    }

    inline void renderText(
        ImDrawList* dl, const ESPObject& obj, const std::string& text, const ESPStyle& style, TextAnchor anchor) {
        if (!style.enabled || text.empty()) {
            return;
        }

        ImVec2 centerTop = { obj.rectMin.x + (obj.rectMax.x - obj.rectMin.x) * 0.5f, obj.rectMin.y };
        ImVec2 centerBot = { obj.rectMin.x + (obj.rectMax.x - obj.rectMin.x) * 0.5f, obj.rectMax.y };

        ImFont* font   = ImGui::GetFont();
        float fSize    = style.fontSize;
        ImVec2 txtSize = font->CalcTextSizeA(fSize, FLT_MAX, 0.0f, text.c_str());

        ImVec2 pos;
        float padding = 2.0f;

        switch (anchor) {
        case TextAnchor::Top:
            pos = { centerTop.x - txtSize.x * 0.5f, centerTop.y - txtSize.y - padding };
            break;
        case TextAnchor::Bottom:
            pos = { centerBot.x - txtSize.x * 0.5f, centerBot.y + padding };
            break;
        case TextAnchor::Center:
            pos = { centerTop.x - txtSize.x * 0.5f,
                centerTop.y + (obj.rectMax.y - obj.rectMin.y) * 0.5f - txtSize.y * 0.5f };
            break;
        default:
            pos = centerTop;
            break;
        }

        ImU32 col = style.getColor().toU32();

        if (style.textOutline) {
            ImU32 black = ImColor(0, 0, 0, 255);
            dl->AddText(font, fSize, { pos.x - 1, pos.y }, black, text.c_str());
            dl->AddText(font, fSize, { pos.x + 1, pos.y }, black, text.c_str());
            dl->AddText(font, fSize, { pos.x, pos.y - 1 }, black, text.c_str());
            dl->AddText(font, fSize, { pos.x, pos.y + 1 }, black, text.c_str());
        }

        dl->AddText(font, fSize, pos, col, text.c_str());
    }
}
