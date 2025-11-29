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

void ESP::onMenuRender() {
    if (ImGui::BeginTabBar("ESPTabs")) {
        if (ImGui::BeginTabItem("General")) {
            ImGui::Checkbox("Box 2D", &m_box2D);
            ImGui::Checkbox("Box 3D", &m_box3D);
            ImGui::Checkbox("Bones", &m_bones);
            ImGui::Checkbox("Nametags", &m_nametags);
            ImGui::Checkbox("Distance", &m_distance);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors & Style")) {
            int modeInt = static_cast<int>(m_mode);
            if (ImGui::Combo("Main Mode", &modeInt, "Solid\0Rainbow\0Gradient\0")) {
                m_mode = static_cast<VisualsUtils::ColorMode>(modeInt);
            }

            if (m_mode == VisualsUtils::ColorMode::Solid) {
                ImGui::ColorEdit4("Main Color", &m_mainColor.r);
            } else {
                ImGui::SliderFloat("Animation Speed", &m_speed, 0.1f, 5.0f);
            }

            ImGui::SliderFloat("Thickness", &m_thickness, 1.0f, 5.0f);
            ImGui::Checkbox("Outline", &m_outline);

            ImGui::Separator();
            ImGui::Text("Overrides");
            ImGui::ColorEdit4("Bone Color", &m_boneColor.r);
            ImGui::ColorEdit4("Text Color", &m_textColor.r);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ESP::onLoadConfig(const nlohmann::json& j) {
    IFeature::onLoadConfig(j);

    // Features
    if (j.contains("Box2D")) {
        m_box2D = j["Box2D"];
    }
    if (j.contains("Box3D")) {
        m_box3D = j["Box3D"];
    }
    if (j.contains("Bones")) {
        m_bones = j["Bones"];
    }
    if (j.contains("Nametags")) {
        m_nametags = j["Nametags"];
    }
    if (j.contains("Distance")) {
        m_distance = j["Distance"];
    }

    // Style
    if (j.contains("ColorMode")) {
        m_mode = static_cast<VisualsUtils::ColorMode>(j["ColorMode"]);
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

    // Colors
    if (j.contains("MainColor")) {
        auto c      = j["MainColor"];
        m_mainColor = Color(c[0], c[1], c[2], c[3]);
    }
    if (j.contains("BoneColor")) {
        auto c      = j["BoneColor"];
        m_boneColor = Color(c[0], c[1], c[2], c[3]);
    }
    if (j.contains("TextColor")) {
        auto c      = j["TextColor"];
        m_textColor = Color(c[0], c[1], c[2], c[3]);
    }
}

void ESP::onSaveConfig(nlohmann::json& j) const {
    IFeature::onSaveConfig(j);

    j["Box2D"]    = m_box2D;
    j["Box3D"]    = m_box3D;
    j["Bones"]    = m_bones;
    j["Nametags"] = m_nametags;
    j["Distance"] = m_distance;

    j["ColorMode"] = m_mode;
    j["Speed"]     = m_speed;
    j["Thickness"] = m_thickness;
    j["Outline"]   = m_outline;

    j["MainColor"] = { m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a };
    j["BoneColor"] = { m_boneColor.r, m_boneColor.g, m_boneColor.b, m_boneColor.a };
    j["TextColor"] = { m_textColor.r, m_textColor.g, m_textColor.b, m_textColor.a };
}
