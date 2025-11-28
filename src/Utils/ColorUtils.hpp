#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include <imgui.h>

struct Color {
    float r, g, b, a;

    Color()
        : r(1.f)
        , g(1.f)
        , b(1.f)
        , a(1.f) {}
    Color(float _r, float _g, float _b, float _a = 1.f)
        : r(_r)
        , g(_g)
        , b(_b)
        , a(_a) {}
    Color(ImColor c)
        : r(c.Value.x)
        , g(c.Value.y)
        , b(c.Value.z)
        , a(c.Value.w) {}

    [[nodiscard]] ImU32 toU32() const { return ImColor(r, g, b, a); }

    [[nodiscard]] operator ImColor() const { return ImColor(r, g, b, a); }

    [[nodiscard]] operator ImVec4() const { return ImVec4(r, g, b, a); }

    static Color lerp(const Color& a, const Color& b, float t) {
        return Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
    }

    static Color fromHSV(float h, float s, float v, float a = 1.0f) {
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
        return Color(r, g, b, a);
    }

    // Hue Cycle
    static Color rainbow(float time, float speed, float saturation = 1.0f, float value = 1.0f, float alpha = 1.0f) {
        float hue = std::fmod(time * speed, 1.0f);
        if (hue < 0.0f) {
            hue += 1.0f;
        }
        return fromHSV(hue, saturation, value, alpha);
    }
};

class Gradient {
public:
    void addStop(float t, Color color) {
        m_stops.push_back({ t, color });
        std::sort(m_stops.begin(), m_stops.end(), [](const auto& a, const auto& b) { return a.time < b.time; });
    }

    [[nodiscard]] Color get(float t) const {
        if (m_stops.empty()) {
            return Color(1, 1, 1, 1);
        }
        if (m_stops.size() == 1) {
            return m_stops[0].color;
        }

        t = t - std::floor(t);

        if (t <= m_stops.front().time) {
            return interpolate(m_stops.back(), m_stops.front(), t + 1.0f);
        }
        if (t >= m_stops.back().time) {
            return interpolate(m_stops.back(), m_stops.front(), t);
        }

        for (size_t i = 0; i < m_stops.size() - 1; ++i) {
            if (t >= m_stops[i].time && t <= m_stops[i + 1].time) {
                return interpolate(m_stops[i], m_stops[i + 1], t);
            }
        }

        return m_stops.back().color;
    }

private:
    struct Stop {
        float time;
        Color color;
    };

    Color interpolate(const Stop& s1, const Stop& s2, float t) const {
        float t1 = s1.time;
        float t2 = s2.time;
        if (t2 < t1) {
            t2 += 1.0f;
        }

        float localT = (t - t1) / (t2 - t1);
        return Color::lerp(s1.color, s2.color, std::clamp(localT, 0.0f, 1.0f));
    }

    std::vector<Stop> m_stops;
};
