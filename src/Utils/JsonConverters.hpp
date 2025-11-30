#pragma once

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "ColorUtils.hpp"
#include "Features/Modules/Visuals/VisualsUtils.hpp"
#include "HUD/HUDDef.hpp"

// ImVec2
namespace nlohmann {
    template <>
    struct adl_serializer<ImVec2> {
        static void to_json(json& j, const ImVec2& v) { j = json{ { "x", v.x }, { "y", v.y } }; }
        static void from_json(const json& j, ImVec2& v) {
            if (j.contains("x")) {
                j.at("x").get_to(v.x);
            }
            if (j.contains("y")) {
                j.at("y").get_to(v.y);
            }
        }
    };
}

// Color
namespace nlohmann {
    template <>
    struct adl_serializer<Color> {
        static void to_json(json& j, const Color& c) { j = json::array({ c.r, c.g, c.b, c.a }); }
        static void from_json(const json& j, Color& c) {
            if (j.is_array() && j.size() >= 4) {
                c.r = j[0];
                c.g = j[1];
                c.b = j[2];
                c.a = j[3];
            }
        }
    };
}

// Gradient
namespace nlohmann {
    template <>
    struct adl_serializer<Gradient> {
        static void to_json(json& j, const Gradient& g) {
            j = json::array();
            for (const auto& stop : g.getStops()) {
                j.push_back({ { "t", stop.time }, { "c", stop.color } });
            }
        }
        static void from_json(const json& j, Gradient& g) {
            g.clear();
            if (j.is_array()) {
                for (const auto& el : j) {
                    float t = 0.0f;
                    Color c;
                    if (el.contains("t")) {
                        el.at("t").get_to(t);
                    }
                    if (el.contains("c")) {
                        el.at("c").get_to(c);
                    }
                    g.addStop(t, c);
                }
            }
        }
    };
}

// Enums
namespace VisualsUtils {
    // Changed: Removed ColorMode::Wave mapping
    NLOHMANN_JSON_SERIALIZE_ENUM(ColorMode, { { ColorMode::Solid, "Solid" }, { ColorMode::Rainbow, "Rainbow" },
                                                { ColorMode::Gradient, "Gradient" }, { ColorMode::Rank, "Rank" } })
}

NLOHMANN_JSON_SERIALIZE_ENUM(
    HUDAnchor, { { HUDAnchor::TopLeft, "TopLeft" }, { HUDAnchor::TopRight, "TopRight" },
                   { HUDAnchor::BottomLeft, "BottomLeft" }, { HUDAnchor::BottomRight, "BottomRight" } })

// ESPStyle
namespace nlohmann {
    template <>
    struct adl_serializer<VisualsUtils::ESPStyle> {
        static void to_json(json& j, const VisualsUtils::ESPStyle& s) {
            j = json{ { "Enabled", s.enabled }, { "Mode", s.colorMode }, { "Primary", s.primaryColor },
                { "Secondary", s.secondaryColor }, { "Gradient", s.gradient }, { "Speed", s.animationSpeed },
                { "Thickness", s.thickness }, { "Rounding", s.rounding }, { "Outline", s.outline },
                { "Filled", s.filled }, { "FillAlpha", s.fillAlpha }, { "TextOutline", s.textOutline },
                { "FontSize", s.fontSize } };
        }

        static void from_json(const json& j, VisualsUtils::ESPStyle& s) {
            s.enabled        = j.value("Enabled", s.enabled);
            s.colorMode      = j.value("Mode", s.colorMode);
            s.primaryColor   = j.value("Primary", s.primaryColor);
            s.secondaryColor = j.value("Secondary", s.secondaryColor);

            if (j.contains("Gradient")) {
                j.at("Gradient").get_to(s.gradient);
            }

            s.animationSpeed = j.value("Speed", s.animationSpeed);
            s.thickness      = j.value("Thickness", s.thickness);
            s.rounding       = j.value("Rounding", s.rounding);
            s.outline        = j.value("Outline", s.outline);
            s.filled         = j.value("Filled", s.filled);
            s.fillAlpha      = j.value("FillAlpha", s.fillAlpha);

            s.textOutline = j.value("TextOutline", s.textOutline);
            s.fontSize    = j.value("FontSize", s.fontSize);
        }
    };
}
