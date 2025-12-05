#include "PlayerESP.hpp"

#include <algorithm>

#include <fmt/format.h>
#include <imgui.h>

#include "ESPUtils.hpp"
#include "PlayerESPObject.hpp"
#include "SDK/Game/PlayerManager.hpp"

PlayerESP::PlayerESP()
    : IFeature(FeatureCategory::Visuals, "Player ESP") {
    m_renderOrder = { Element::Box2D, Element::Box3D, Element::Skeleton, Element::Name, Element::Distance };

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

        PlayerESPObject obj = makePlayerESPObject(p);

        for (Element element : m_renderOrder) {
            renderElement(dl, obj, element);
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
        if (ImGui::BeginTabItem("Order")) {
            renderOrderEditor();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

static const char* elementLabel(PlayerESP::Element element) {
    switch (element) {
    case PlayerESP::Element::Box2D:
        return "2D Box";
    case PlayerESP::Element::Box3D:
        return "3D Box";
    case PlayerESP::Element::Skeleton:
        return "Skeleton";
    case PlayerESP::Element::Name:
        return "Name";
    case PlayerESP::Element::Distance:
        return "Distance";
    default:
        return "Unknown";
    }
}

void PlayerESP::renderElement(ImDrawList* dl, const PlayerESPObject& obj, Element element) {
    switch (element) {
    case Element::Box2D:
        if (m_box2DStyle.enabled) {
            ESPUtils::renderBox2D(dl, obj, obj.applyStyle(m_box2DStyle));
        }
        break;
    case Element::Box3D:
        if (m_box3DStyle.enabled) {
            ESPUtils::renderBox3D(dl, obj, obj.applyStyle(m_box3DStyle));
        }
        break;
    case Element::Skeleton:
        if (m_skeletonStyle.enabled) {
            ESPUtils::renderSkeleton(dl, obj, obj.applyStyle(m_skeletonStyle));
        }
        break;
    case Element::Name:
        if (m_nameStyle.enabled) {
            ESPUtils::renderText(dl, obj, obj.name, obj.applyStyle(m_nameStyle), TextAnchor::Top);
        }
        break;
    case Element::Distance:
        if (m_distanceStyle.enabled) {
            std::string distStr = fmt::format("[{:.0f}m]", obj.distance);
            ESPUtils::renderText(dl, obj, distStr, obj.applyStyle(m_distanceStyle), TextAnchor::Bottom);
        }
        break;
    }
}

void PlayerESP::renderOrderEditor() {
    ImGui::TextDisabled("Drag and drop to reorder (Top = Behind, Bottom = Front)");

    ImGui::Separator();

    for (size_t n = 0; n < m_renderOrder.size(); n++) {
        Element item         = m_renderOrder[n];
        const char* itemName = elementLabel(item);

        ImGui::PushID(static_cast<int>(n));

        ImGui::Selectable(itemName, false, ImGuiSelectableFlags_AllowOverlap);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
            ImGui::SetDragDropPayload("ESP_RENDER_ORDER", &n, sizeof(size_t));

            ImGui::Text("Move %s", itemName);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ESP_RENDER_ORDER")) {
                size_t payload_n = *(const size_t*) payload->Data;

                if (payload_n != n) {
                    Element payload_item = m_renderOrder[payload_n];

                    if (payload_n < n) {
                        std::rotate(m_renderOrder.begin() + payload_n, m_renderOrder.begin() + payload_n + 1,
                            m_renderOrder.begin() + n + 1);
                    } else {
                        std::rotate(m_renderOrder.begin() + n, m_renderOrder.begin() + payload_n,
                            m_renderOrder.begin() + payload_n + 1);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
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
