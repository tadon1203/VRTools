#pragma once

#include <array>
#include <vector>

#include <imgui.h>

#include "Utils/ColorUtils.hpp"

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
        case ColorMode::Rank:
            return primaryColor;
        default:
            return primaryColor;
        }
    }
};

struct ESPObject {
    virtual ~ESPObject() = default;

    ImVec2 rectMin{};
    ImVec2 rectMax{};

    std::array<ImVec2, 8> corners3d{};
    bool is3dValid = false;

    std::vector<std::pair<ImVec2, ImVec2>> bones;

    float distance = 0.0f;
};
