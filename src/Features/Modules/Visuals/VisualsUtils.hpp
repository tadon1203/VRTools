#pragma once

#include <imgui.h>

#include "Utils/ColorUtils.hpp"

namespace VisualsUtils {

    enum class ColorMode { Solid, Rainbow, Gradient };

    struct BoxSettings {
        // Positioning
        float x, y, w, h;

        // Appearance
        ColorMode mode   = ColorMode::Solid;
        Color solidColor = Color(1, 1, 1, 1);
        Gradient gradient;

        // Animation
        float time       = 0.0f;
        float speed      = 1.0f;
        float rainbowSat = 1.0f;
        float rainbowVal = 1.0f;

        // Styling
        bool outline    = true;
        bool inline_    = true; // 'inline' is reserved
        float thickness = 1.0f;
    };

    inline Color getColorAtOffset(const BoxSettings& s, float offset) {
        // (GlobalTime * Speed) + SpatialOffset
        float t = (s.time * s.speed) + offset;

        if (s.mode == ColorMode::Rainbow) {
            return Color::rainbow(t, 1.0f, s.rainbowSat, s.rainbowVal);
        }
        if (s.mode == ColorMode::Gradient) {
            return s.gradient.get(t);
        }

        return s.solidColor;
    }

    inline void drawBox(ImDrawList* dl, const BoxSettings& s) {
        // Draw Outline
        if (s.outline) {
            dl->AddRect(ImVec2(s.x - 1, s.y - 1), ImVec2(s.x + s.w + 1, s.y + s.h + 1), ImColor(0, 0, 0, 255), 0.0f, 0,
                s.thickness + 2.0f);
        }

        // Draw Inline
        if (s.inline_) {
            dl->AddRect(ImVec2(s.x + 1, s.y + 1), ImVec2(s.x + s.w - 1, s.y + s.h - 1), ImColor(0, 0, 0, 255), 0.0f, 0,
                s.thickness);
        }

        // Draw The Gradient/Rainbow Stroke
        float tTopLeft  = 0.00f;
        float tTopRight = 0.25f;
        float tBotRight = 0.50f;
        float tBotLeft  = 0.75f;
        float tEnd      = 1.00f;

        ImU32 colorTL = getColorAtOffset(s, tTopLeft).toU32();
        ImU32 cTR     = getColorAtOffset(s, tTopRight).toU32();
        ImU32 cBR     = getColorAtOffset(s, tBotRight).toU32();
        ImU32 cBL     = getColorAtOffset(s, tBotLeft).toU32();
        ImU32 cEnd    = getColorAtOffset(s, tEnd).toU32(); // Should match cTL mathematically if looped

        // Top Bar
        dl->AddRectFilledMultiColor(ImVec2(s.x, s.y), ImVec2(s.x + s.w, s.y + s.thickness), colorTL, cTR, cTR, colorTL);

        // Right Bar
        dl->AddRectFilledMultiColor(
            ImVec2(s.x + s.w - s.thickness, s.y), ImVec2(s.x + s.w, s.y + s.h), cTR, cTR, cBR, cBR);

        // Bottom Bar
        dl->AddRectFilledMultiColor(
            ImVec2(s.x, s.y + s.h - s.thickness), ImVec2(s.x + s.w, s.y + s.h), cBL, cBR, cBR, cBL);

        // Left Bar
        dl->AddRectFilledMultiColor(ImVec2(s.x, s.y), ImVec2(s.x + s.thickness, s.y + s.h), colorTL, colorTL, cBL, cBL);
    }
}
