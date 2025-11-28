#pragma once

#include <cmath>

#include "SDK/Unity/Vector3.hpp"

namespace Utils::Math {

    inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

    inline float easeOutCubic(float t) { return 1.0f - std::pow(1.0f - t, 3.0f); }

    inline float easeOutExpo(float t) { return (t == 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }

    inline ImVec2 unityToImGui(const UnityEngine::Vector3& unityScreenPos) {
        return ImVec2(unityScreenPos.x, ImGui::GetIO().DisplaySize.y - unityScreenPos.y);
    }
}
