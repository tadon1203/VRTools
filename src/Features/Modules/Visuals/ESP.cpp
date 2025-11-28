#include "ESP.hpp"

#include <imgui.h>

#include "SDK/Game/PlayerManager.hpp"
#include "SDK/Game/VRCHelper.hpp"
#include "Utils/Math.hpp"

ESP::ESP()
    : IFeature(FeatureCategory::Visuals, "ESP") {
    m_customGradient.addStop(0.0f, Color(0.0f, 0.5f, 1.0f));
    m_customGradient.addStop(0.5f, Color(0.8f, 0.0f, 1.0f));
    m_customGradient.addStop(1.0f, Color(0.0f, 0.5f, 1.0f));
}

void ESP::onRender() {
    if (!VRC::isInWorld()) {
        return;
    }

    auto players = PlayerManager::instance().getPlayers();
    if (players.empty()) {
        return;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float time     = static_cast<float>(ImGui::GetTime());

    for (const auto& player : players) {
        if (!player.isVisible) {
            continue;
        }
        if (player.isLocal) {
            continue;
        }

        ImVec2 headPos = Utils::Math::unityToImGui(player.headScreen);
        ImVec2 footPos = Utils::Math::unityToImGui(player.rootScreen);

        // In ImGui (Top-Left 0,0), Head Y is smaller than Foot Y
        float height = footPos.y - headPos.y;

        // Sanity check
        if (height < 1.0f) {
            continue;
        }

        float width = height * 0.5f;
        float x     = footPos.x - (width * 0.5f);
        float y     = headPos.y;

        if (m_box2D) {
            VisualsUtils::BoxSettings settings;
            settings.x = x;
            settings.y = y;
            settings.w = width;
            settings.h = height;

            settings.mode       = m_mode;
            settings.solidColor = m_solidColor;
            settings.gradient   = m_customGradient;

            settings.time      = time;
            settings.speed     = m_speed;
            settings.thickness = m_thickness;
            settings.outline   = m_outline;
            settings.inline_   = m_inline;

            VisualsUtils::drawBox(dl, settings);
        }
    }
}

void ESP::onMenuRender() {
    ImGui::Checkbox("Box 2D", &m_box2D);

    int modeInt = static_cast<int>(m_mode);
    if (ImGui::Combo("Color Mode", &modeInt, "Solid\0Rainbow\0Gradient\0")) {
        m_mode = static_cast<VisualsUtils::ColorMode>(modeInt);
    }

    if (m_mode == VisualsUtils::ColorMode::Solid) {
        ImGui::ColorEdit4("Color", &m_solidColor.r);
    } else {
        ImGui::SliderFloat("Animation Speed", &m_speed, 0.1f, 5.0f);
    }

    ImGui::SliderFloat("Thickness", &m_thickness, 1.0f, 3.0f);
    ImGui::Checkbox("Outline", &m_outline);
    ImGui::SameLine();
    ImGui::Checkbox("Inline", &m_inline);
}

void ESP::onLoadConfig(const nlohmann::json& j) {
    IFeature::onLoadConfig(j);

    if (j.contains("Box2D")) {
        m_box2D = j["Box2D"];
    }
    if (j.contains("ColorMode")) {
        m_mode = static_cast<VisualsUtils::ColorMode>(j["ColorMode"]);
    }

    if (j.contains("SolidColor")) {
        auto c       = j["SolidColor"];
        m_solidColor = Color(c[0], c[1], c[2], c[3]);
    }

    if (j.contains("Speed")) {
        m_speed = j["Speed"];
    }
    if (j.contains("Thickness")) {
        m_thickness = j["Thickness"];
    }
    if (j.contains("Outline")) {
        m_outline = j["Outline"];
    }
    if (j.contains("Inline")) {
        m_inline = j["Inline"];
    }
}

void ESP::onSaveConfig(nlohmann::json& j) const {
    IFeature::onSaveConfig(j);

    j["Box2D"]      = m_box2D;
    j["ColorMode"]  = m_mode;
    j["SolidColor"] = { m_solidColor.r, m_solidColor.g, m_solidColor.b, m_solidColor.a };
    j["Speed"]      = m_speed;
    j["Thickness"]  = m_thickness;
    j["Outline"]    = m_outline;
    j["Inline"]     = m_inline;
}
