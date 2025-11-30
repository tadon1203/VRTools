#include "PlayerESP.hpp"

#include <fmt/format.h>
#include <imgui.h>

#include "SDK/Game/PlayerManager.hpp"
#include "SDK/Game/PlayerRank.hpp"

using namespace VisualsUtils;

PlayerESP::PlayerESP()
    : IFeature(FeatureCategory::Visuals, "Player ESP") {

    m_box2DStyle.enabled   = true;
    m_box2DStyle.colorMode = ColorMode::Rank;

    m_box3DStyle.enabled   = false;
    m_box3DStyle.colorMode = ColorMode::Rank;
    m_box3DStyle.filled    = true;
    m_box3DStyle.fillAlpha = 0.2f;

    m_skeletonStyle.enabled   = false;
    m_skeletonStyle.colorMode = ColorMode::Solid;

    m_nameStyle.enabled     = true;
    m_nameStyle.colorMode   = ColorMode::Solid;
    m_nameStyle.textOutline = true;

    m_distanceStyle.enabled      = true;
    m_distanceStyle.fontSize     = 11.0f;
    m_distanceStyle.primaryColor = Color(0.8f, 0.8f, 0.8f, 1.0f);
    m_distanceStyle.textOutline  = true;

    m_settings.add("Box2D", &m_box2DStyle);
    m_settings.add("Box3D", &m_box3DStyle);
    m_settings.add("Skeleton", &m_skeletonStyle);
    m_settings.add("Name", &m_nameStyle);
    m_settings.add("Distance", &m_distanceStyle);
}

void PlayerESP::onRender() {
    auto players = PlayerManager::instance().getDrawPlayers();
    if (players.empty()) {
        return;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    for (const auto& p : players) {
        if (!p.isVisible) {
            continue;
        }

        ESPObject obj;
        obj.rectMin   = p.rectMin;
        obj.rectMax   = p.rectMax;
        obj.corners3d = p.corners3d;
        obj.is3dValid = true;
        obj.bones     = p.bones;
        obj.distance  = p.distance;

        auto applyRank = [&](ESPStyle style) -> ESPStyle {
            if (style.colorMode == ColorMode::Rank) {
                style.primaryColor = VRC::getRankColor(p.rank);
            }
            return style;
        };

        renderBox2D(dl, obj, applyRank(m_box2DStyle));
        renderBox3D(dl, obj, applyRank(m_box3DStyle));
        renderSkeleton(dl, obj, applyRank(m_skeletonStyle));

        if (m_nameStyle.enabled) {
            renderText(dl, obj, p.name, applyRank(m_nameStyle), TextAnchor::Top);
        }

        if (m_distanceStyle.enabled) {
            std::string distStr = fmt::format("[{:.0f}m]", p.distance);
            renderText(dl, obj, distStr, applyRank(m_distanceStyle), TextAnchor::Bottom);
        }
    }
}

void PlayerESP::onMenuRender() {
    if (ImGui::BeginTabBar("ESPTabs")) {
        if (ImGui::BeginTabItem("2D Box")) {
            renderStyleEditor("##Box2D", m_box2DStyle);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("3D Box")) {
            renderStyleEditor("##Box3D", m_box3DStyle);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skeleton")) {
            renderStyleEditor("##Skel", m_skeletonStyle);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Name")) {
            renderStyleEditor("##Name", m_nameStyle, true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Distance")) {
            renderStyleEditor("##Dist", m_distanceStyle, true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void PlayerESP::renderStyleEditor(const char* label, ESPStyle& style, bool isText) {
    ImGui::PushID(label);

    ImGui::Checkbox("Enabled", &style.enabled);

    if (style.enabled) {
        ImGui::Separator();
        ImGui::Text("Appearance");

        int mode = static_cast<int>(style.colorMode);
        if (ImGui::Combo("Color Mode", &mode, "Solid\0Rainbow\0Gradient\0Rank")) {
            style.colorMode = static_cast<ColorMode>(mode);
        }

        if (style.colorMode != ColorMode::Rank) {
            ImGui::ColorEdit4("Primary", &style.primaryColor.r, ImGuiColorEditFlags_AlphaBar);
        }

        if (style.colorMode == ColorMode::Gradient) {
            ImGui::ColorEdit4("Secondary", &style.secondaryColor.r, ImGuiColorEditFlags_AlphaBar);
        }

        if (style.colorMode == ColorMode::Rainbow || style.colorMode == ColorMode::Gradient) {
            ImGui::SliderFloat("Speed", &style.animationSpeed, 0.1f, 10.0f);
        }

        ImGui::Separator();

        if (isText) {
            ImGui::SliderFloat("Font Size", &style.fontSize, 8.0f, 32.0f);
            ImGui::Checkbox("Outline", &style.textOutline);
        } else {
            ImGui::SliderFloat("Thickness", &style.thickness, 1.0f, 5.0f);
            ImGui::SliderFloat("Rounding", &style.rounding, 0.0f, 10.0f);
            ImGui::Checkbox("Outline", &style.outline);

            ImGui::Spacing();
            ImGui::Checkbox("Filled", &style.filled);
            if (style.filled) {
                ImGui::SliderFloat("Fill Alpha", &style.fillAlpha, 0.0f, 1.0f);
            }
        }
    }

    ImGui::PopID();
}
