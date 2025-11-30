#include "PlayerESP.hpp"

#include <fmt/format.h>
#include <imgui.h>

#include "SDK/Game/PlayerManager.hpp"
#include "SDK/Game/PlayerRank.hpp"

PlayerESP::PlayerESP()
    : IFeature(FeatureCategory::Visuals, "Player ESP") {

    m_settings.add("DrawBox", &m_drawBox, true);
    m_settings.add("DrawBox3D", &m_drawBox3D, false);
    m_settings.add("FillBox", &m_fillBox, false);
    m_settings.add("DrawName", &m_drawName, true);
    m_settings.add("DrawDistance", &m_drawDistance, true);

    m_settings.add("UseRankColor", &m_useRankColor, true);
    m_settings.add("CustomColor", &m_customColor);
    m_settings.add("FillColor", &m_fillColor);

    m_settings.add("Style", &m_style);
}

void PlayerESP::onRender() {
    auto players = PlayerManager::instance().getDrawPlayers();
    if (players.empty()) {
        return;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float curTime  = static_cast<float>(ImGui::GetTime());

    m_style.time = curTime;

    for (const auto& p : players) {
        if (!p.isVisible) {
            continue;
        }

        VisualsUtils::VisualsStyle pStyle = m_style;

        if (m_useRankColor) {
            pStyle.colorMode    = VisualsUtils::ColorMode::Solid;
            pStyle.primaryColor = VRC::getRankColor(p.rank);
        } else if (pStyle.colorMode == VisualsUtils::ColorMode::Solid) {
            pStyle.primaryColor = m_customColor;
        }

        if (m_drawBox) {
            if (m_drawBox3D) {
                VisualsUtils::ScreenCube cube;
                cube.corners = p.corners3d;
                VisualsUtils::drawBox3D(dl, cube, pStyle, m_fillBox, m_fillColor.toU32());
            } else {
                VisualsUtils::ScreenRect rect = { p.rectMin, p.rectMax };
                VisualsUtils::drawBox2D(dl, rect, pStyle);
            }
        }

        float padding = 2.0f;
        float centerX = p.rectMin.x + (p.rectMax.x - p.rectMin.x) * 0.5f;

        float cursorTop = p.rectMin.y;
        float cursorBot = p.rectMax.y;

        if (m_drawName) {
            std::string text = p.name;
            ImVec2 textSize  = ImGui::GetFont()->CalcTextSizeA(pStyle.fontSize, FLT_MAX, 0.0f, text.c_str());

            cursorTop -= (textSize.y + padding);
            VisualsUtils::drawText(dl, { centerX, cursorTop }, text, pStyle, true);
        }

        if (m_drawDistance) {
            std::string text = fmt::format("[{:.0f}m]", p.distance);
            VisualsUtils::drawText(dl, { centerX, cursorBot + padding }, text, pStyle, true);
        }
    }
}

void PlayerESP::onMenuRender() {
    if (ImGui::BeginTabBar("ESPTabs")) {
        if (ImGui::BeginTabItem("Settings")) {
            ImGui::Checkbox("Draw Box", &m_drawBox);
            ImGui::SameLine();
            ImGui::Checkbox("3D Box", &m_drawBox3D);

            if (m_drawBox && m_drawBox3D) {
                ImGui::Indent();
                ImGui::Checkbox("Fill Box", &m_fillBox);
                if (m_fillBox) {
                    ImGui::SameLine();
                    ImGui::ColorEdit4(
                        "Fill Color", &m_fillColor.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                }
                ImGui::Unindent();
            }

            ImGui::Separator();

            ImGui::Checkbox("Name (Top)", &m_drawName);
            ImGui::Checkbox("Distance (Bottom)", &m_drawDistance);

            ImGui::Separator();
            ImGui::Text("Color Source");
            ImGui::Checkbox("Use Rank Color", &m_useRankColor);

            if (!m_useRankColor) {
                ImGui::TextDisabled("If unchecked, uses Style settings.");
                if (m_style.colorMode == VisualsUtils::ColorMode::Solid) {
                    ImGui::ColorEdit4("Custom Color", &m_customColor.r);
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Style")) {
            ImGui::TextDisabled("Applies when Rank Color is OFF (or for effects)");

            int modeInt = static_cast<int>(m_style.colorMode);
            if (ImGui::Combo("Color Mode", &modeInt, "Solid\0Rainbow\0Gradient")) {
                m_style.colorMode = static_cast<VisualsUtils::ColorMode>(modeInt);
            }

            if (m_style.colorMode != VisualsUtils::ColorMode::Solid) {
                ImGui::SliderFloat("Speed", &m_style.animationSpeed, 0.1f, 5.0f);
            }

            ImGui::Separator();
            ImGui::SliderFloat("Thickness", &m_style.thickness, 1.0f, 5.0f);
            ImGui::SliderFloat("Font Size", &m_style.fontSize, 8.0f, 24.0f);

            ImGui::Checkbox("Outline Box", &m_style.outline);
            ImGui::Checkbox("Outline Text", &m_style.textOutline);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
