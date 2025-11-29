#include "ESP.hpp"

#include <fmt/format.h>
#include <imgui.h>

#include "SDK/Game/PlayerManager.hpp"

ESP::ESP()
    : IFeature(FeatureCategory::Visuals, "ESP") {
    m_customGradient.addStop(0.0f, Color(0.0f, 0.5f, 1.0f));
    m_customGradient.addStop(1.0f, Color(0.0f, 0.5f, 1.0f));
}

void ESP::onRender() {
    auto players = PlayerManager::instance().getDrawPlayers();
    if (players.empty()) {
        return;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float time     = static_cast<float>(ImGui::GetTime());

    // Prepare Styles...
    VisualsUtils::VisualsStyle style;
    style.colorMode      = m_mode;
    style.primaryColor   = m_mainColor;
    style.gradient       = m_customGradient;
    style.animationSpeed = m_speed;
    style.time           = time;
    style.thickness      = m_thickness;
    style.outline        = m_outline;

    VisualsUtils::VisualsStyle textStyle = style;
    textStyle.colorMode                  = VisualsUtils::ColorMode::Solid;
    textStyle.primaryColor               = m_textColor;

    VisualsUtils::VisualsStyle boneStyle = style;
    boneStyle.colorMode                  = VisualsUtils::ColorMode::Solid;
    boneStyle.primaryColor               = m_boneColor;

    for (const auto& p : players) {
        if (!p.isVisible) {
            continue;
        }

        VisualsUtils::ScreenRect r = { p.rectMin, p.rectMax };

        if (m_box2D) {
            VisualsUtils::drawBox2D(dl, r, style);
        }

        if (m_box3D) {
            VisualsUtils::ScreenCube cube;
            bool valid = true;
            for (int i = 0; i < 8; ++i) {
                if (p.corners3d[i].x <= -9000) {
                    valid = false;
                    break;
                } // Check validity
                cube.corners[i] = p.corners3d[i];
            }
            if (valid) {
                VisualsUtils::drawBox3D(dl, cube, style);
            }
        }

        if (m_bones && !p.bones.empty()) {
            VisualsUtils::drawSkeleton(dl, p.bones, boneStyle);
        }

        if (m_nametags) {
            ImVec2 pos = r.getTopCenter();
            pos.y -= (textStyle.fontSize + 2.0f);
            VisualsUtils::drawText(dl, pos, p.name, textStyle);
        }

        if (m_distance) {
            std::string d = fmt::format("[{:.0f}m]", p.distance);
            ImVec2 pos    = r.getBottomCenter();
            pos.y += 2.0f;
            VisualsUtils::drawText(dl, pos, d, textStyle);
        }
    }
}
