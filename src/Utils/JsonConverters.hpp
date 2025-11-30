#pragma once

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "ColorUtils.hpp"
#include "Features/Modules/Visuals/ESPComponents/ESPDef.hpp"
#include "Features/Modules/Visuals/VisualsUtils.hpp"
#include "HUD/HUDDef.hpp"

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

namespace VisualsUtils {
    NLOHMANN_JSON_SERIALIZE_ENUM(ColorMode, { { ColorMode::Solid, "Solid" }, { ColorMode::Rainbow, "Rainbow" },
                                                { ColorMode::Gradient, "Gradient" }, { ColorMode::Rank, "Rank" } })
}

NLOHMANN_JSON_SERIALIZE_ENUM(
    ESPAnchor, { { ESPAnchor::Top, "Top" }, { ESPAnchor::Bottom, "Bottom" }, { ESPAnchor::Left, "Left" },
                   { ESPAnchor::Right, "Right" }, { ESPAnchor::Center, "Center" } })

NLOHMANN_JSON_SERIALIZE_ENUM(
    HUDAnchor, { { HUDAnchor::TopLeft, "TopLeft" }, { HUDAnchor::TopRight, "TopRight" },
                   { HUDAnchor::BottomLeft, "BottomLeft" }, { HUDAnchor::BottomRight, "BottomRight" } })

namespace nlohmann {
    template <>
    struct adl_serializer<VisualsUtils::VisualsStyle> {
        static void to_json(json& j, const VisualsUtils::VisualsStyle& s) {
            j = json{ { "Mode", s.colorMode }, { "Color", s.primaryColor }, { "Gradient", s.gradient },
                { "Thickness", s.thickness }, { "Outline", s.outline }, { "TextOutline", s.textOutline },
                { "FontSize", s.fontSize }, { "Speed", s.animationSpeed } };
        }

        static void from_json(const json& j, VisualsUtils::VisualsStyle& s) {
            s.colorMode    = j.value("Mode", s.colorMode);
            s.primaryColor = j.value("Color", s.primaryColor);

            if (j.contains("Gradient")) {
                j.at("Gradient").get_to(s.gradient);
            }

            s.thickness      = j.value("Thickness", s.thickness);
            s.outline        = j.value("Outline", s.outline);
            s.textOutline    = j.value("TextOutline", s.textOutline);
            s.fontSize       = j.value("FontSize", s.fontSize);
            s.animationSpeed = j.value("Speed", s.animationSpeed);
        }
    };
}
