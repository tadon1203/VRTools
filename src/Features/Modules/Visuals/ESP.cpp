#include "ESP.hpp"

#include <imgui.h>

#include "ESPComponents/Elements.hpp"
#include "SDK/Game/PlayerManager.hpp"

ESP::ESP()
    : IFeature(FeatureCategory::Visuals, "ESP") {
    m_customGradient.addStop(0.0f, Color(0.0f, 0.5f, 1.0f));
    m_customGradient.addStop(1.0f, Color(0.0f, 0.5f, 1.0f));

    m_elements.push_back(std::make_unique<BoxElement>());
    m_elements.push_back(std::make_unique<Box3DElement>());
    m_elements.push_back(std::make_unique<SkeletonElement>());
    m_elements.push_back(std::make_unique<NametagElement>());
    m_elements.push_back(std::make_unique<DistanceElement>());
}

void ESP::onRender() {
    auto players = PlayerManager::instance().getDrawPlayers();
    if (players.empty()) {
        return;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    auto time      = static_cast<float>(ImGui::GetTime());

    ESPContext ctx;

    ctx.styleBox2D = { m_mode, m_box2DColor, m_customGradient, m_thickness, m_outline, true, 13.0f, time, m_speed };

    ctx.styleBox3D              = ctx.styleBox2D;
    ctx.styleBox3D.colorMode    = VisualsUtils::ColorMode::Solid;
    ctx.styleBox3D.primaryColor = m_box3DColor;
    ctx.styleBox3D.outline      = false;

    ctx.textStyle              = ctx.styleBox2D;
    ctx.textStyle.colorMode    = VisualsUtils::ColorMode::Solid;
    ctx.textStyle.primaryColor = m_textColor;

    ctx.boneStyle              = ctx.styleBox2D;
    ctx.boneStyle.colorMode    = VisualsUtils::ColorMode::Solid;
    ctx.boneStyle.primaryColor = m_boneColor;

    for (const auto& p : players) {
        if (!p.isVisible) {
            continue;
        }

        float topY    = p.rectMin.y;
        float botY    = p.rectMax.y;
        float leftX   = p.rectMin.x;
        float rightX  = p.rectMax.x;
        float centerX = p.rectMin.x + (p.rectMax.x - p.rectMin.x) * 0.5f;
        float centerY = p.rectMin.y + (p.rectMax.y - p.rectMin.y) * 0.5f;

        for (const auto& el : m_elements) {
            if (!el->isEnabled()) {
                continue;
            }

            ImVec2 size    = el->getSize(p, ctx);
            ImVec2 drawPos = { 0, 0 };

            switch (el->getAnchor()) {
            case ESPAnchor::Top:
                topY -= (size.y + ctx.padding);
                drawPos = { centerX - size.x * 0.5f, topY };
                break;
            case ESPAnchor::Bottom:
                drawPos = { centerX - size.x * 0.5f, botY + ctx.padding };
                botY += (size.y + ctx.padding);
                break;
            case ESPAnchor::Left:
                leftX -= (size.x + ctx.padding);
                drawPos = { leftX, centerY - size.y * 0.5f };
                break;
            case ESPAnchor::Right:
                drawPos = { rightX + ctx.padding, centerY - size.y * 0.5f };
                rightX += (size.x + ctx.padding);
                break;
            case ESPAnchor::Center:
                drawPos = p.rectMin;
                break;
            }

            el->render(dl, p, ctx, drawPos);
        }
    }
}

void ESP::onMenuRender() {
    if (ImGui::BeginTabBar("ESPTabs")) {
        if (ImGui::BeginTabItem("Elements")) {
            ImGui::Spacing();
            ImGui::TextDisabled("Layout Config");
            ImGui::Separator();
            for (const auto& el : m_elements) {
                el->onMenuRender();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Style")) {
            int modeInt = static_cast<int>(m_mode);
            if (ImGui::Combo("Master Mode", &modeInt, "Solid\0Rainbow\0Gradient\0")) {
                m_mode = static_cast<VisualsUtils::ColorMode>(modeInt);
            }

            ImGui::SliderFloat("Thick", &m_thickness, 1.0f, 5.0f);
            ImGui::Checkbox("Global Outline", &m_outline);

            ImGui::Separator();
            ImGui::Text("Colors");

            if (m_mode == VisualsUtils::ColorMode::Solid) {
                ImGui::ColorEdit4("Box 2D", &m_box2DColor.r);
            } else {
                ImGui::SliderFloat("Rainbow Speed", &m_speed, 0.1f, 5.0f);
            }

            ImGui::ColorEdit4("Box 3D (Solid)", &m_box3DColor.r);
            ImGui::ColorEdit4("Skeleton", &m_boneColor.r);
            ImGui::ColorEdit4("Text", &m_textColor.r);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ESP::onLoadConfig(const nlohmann::json& j) {
    IFeature::onLoadConfig(j);

    if (j.contains("Elements")) {
        for (const auto& el : m_elements) {
            el->onLoadConfig(j["Elements"]);
        }
    }

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

    if (j.contains("Box2DColor")) {
        auto c       = j["Box2DColor"];
        m_box2DColor = Color(c[0], c[1], c[2], c[3]);
    }
    if (j.contains("Box3DColor")) {
        auto c       = j["Box3DColor"];
        m_box3DColor = Color(c[0], c[1], c[2], c[3]);
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

    nlohmann::json elems;
    for (const auto& el : m_elements) {
        el->onSaveConfig(elems);
    }
    j["Elements"] = elems;

    j["ColorMode"] = m_mode;
    j["Speed"]     = m_speed;
    j["Thickness"] = m_thickness;
    j["Outline"]   = m_outline;

    j["Box2DColor"] = { m_box2DColor.r, m_box2DColor.g, m_box2DColor.b, m_box2DColor.a };
    j["Box3DColor"] = { m_box3DColor.r, m_box3DColor.g, m_box3DColor.b, m_box3DColor.a };
    j["BoneColor"]  = { m_boneColor.r, m_boneColor.g, m_boneColor.b, m_boneColor.a };
    j["TextColor"]  = { m_textColor.r, m_textColor.g, m_textColor.b, m_textColor.a };
}

bool ESP::isBoneEspEnabled() const {
    for (const auto& el : m_elements) {
        if (el->getName() == "Skeleton") {
            return el->isEnabled();
        }
    }
    return false;
}

bool ESP::isBox3dEnabled() const {
    for (const auto& el : m_elements) {
        if (el->getName() == "Box 3D") {
            return el->isEnabled();
        }
    }
    return false;
}
