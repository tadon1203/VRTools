#include "ESP.hpp"

#include <imgui.h>

#include "ESPComponents/Elements.hpp"
#include "SDK/Game/PlayerManager.hpp"

ESP::ESP()
    : IFeature(FeatureCategory::Visuals, "ESP") {
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
    float curTime  = static_cast<float>(ImGui::GetTime());

    ESPContext ctx;
    ctx.curTime = curTime;
    ctx.padding = 2.0f;

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
    ImGui::Text("ESP Elements");
    ImGui::Separator();


    for (const auto& el : m_elements) {
        el->onMenuRender();
    }
}

void ESP::onLoadConfig(const nlohmann::json& j) {
    IFeature::onLoadConfig(j);
    if (j.contains("Elements")) {
        for (const auto& el : m_elements) {
            el->onLoadConfig(j["Elements"]);
        }
    }
}

void ESP::onSaveConfig(nlohmann::json& j) const {
    IFeature::onSaveConfig(j);
    nlohmann::json elems;
    for (const auto& el : m_elements) {
        el->onSaveConfig(elems);
    }
    j["Elements"] = elems;
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
