#pragma once

#include <string>

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "../VisualsUtils.hpp"
#include "Core/Settings/SettingsGroup.hpp"
#include "ESPDef.hpp"
#include "SDK/Game/PlayerManager.hpp"

enum class ElementType { Geometry, Text };

class IESPElement {
public:
    virtual ~IESPElement() = default;

    explicit IESPElement(std::string name, ElementType type, ESPAnchor defaultAnchor = ESPAnchor::Top)
        : m_name(std::move(name))
        , m_type(type)
        , m_anchor(defaultAnchor) {
        m_style.gradient.addStop(0.0f, Color(0.0f, 0.5f, 1.0f));
        m_style.gradient.addStop(1.0f, Color(1.0f, 0.5f, 0.0f));

        m_settings.add("Enabled", &m_enabled, true);

        m_settings.add("Anchor", &m_anchor, defaultAnchor);

        m_settings.add("Style", &m_style);
    }

    virtual ImVec2 getSize(const DrawPlayer& player, const ESPContext& ctx)                          = 0;
    virtual void render(ImDrawList* dl, const DrawPlayer& player, const ESPContext& ctx, ImVec2 pos) = 0;

    virtual void onMenuRender() {
        if (ImGui::TreeNode(m_name.c_str())) {

            ImGui::Checkbox("Enabled", &m_enabled);

            if (m_anchor != ESPAnchor::Center) {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                int a = static_cast<int>(m_anchor);
                if (ImGui::Combo("Anchor", &a, "Top\0Bottom\0Left\0Right")) {
                    m_anchor = static_cast<ESPAnchor>(a);
                }
            }

            ImGui::Separator();

            ImGui::TextDisabled("Style Settings");

            int modeInt = static_cast<int>(m_style.colorMode);
            if (ImGui::Combo("Mode", &modeInt, "Solid\0Rainbow\0Gradient\0Rank")) {
                m_style.colorMode = static_cast<VisualsUtils::ColorMode>(modeInt);
            }

            if (m_style.colorMode == VisualsUtils::ColorMode::Solid) {
                ImGui::ColorEdit4("Color", &m_style.primaryColor.r);
            } else if (m_style.colorMode != VisualsUtils::ColorMode::Rank) {
                ImGui::SliderFloat("Anim Speed", &m_style.animationSpeed, 0.1f, 5.0f);
            }

            if (m_type == ElementType::Geometry) {
                ImGui::SliderFloat("Thickness", &m_style.thickness, 1.0f, 5.0f);
                ImGui::Checkbox("Outline", &m_style.outline);
            }

            if (m_type == ElementType::Text) {
                ImGui::SliderFloat("Font Size", &m_style.fontSize, 8.0f, 24.0f);
                ImGui::Checkbox("Text Outline", &m_style.textOutline);
            }

            ImGui::TreePop();
        }
    }

    SettingsGroup& getSettings() { return m_settings; }
    [[nodiscard]] bool isEnabled() const { return m_enabled; }
    [[nodiscard]] ESPAnchor getAnchor() const { return m_anchor; }
    [[nodiscard]] const std::string& getName() const { return m_name; }

    [[nodiscard]] VisualsUtils::VisualsStyle getStyle(const DrawPlayer& p) const {
        auto style = m_style;
        if (style.colorMode == VisualsUtils::ColorMode::Rank) {
            style.colorMode    = VisualsUtils::ColorMode::Solid;
            style.primaryColor = VRC::getRankColor(p.rank);
        }
        return style;
    }

protected:
    std::string m_name;
    ElementType m_type;
    ESPAnchor m_anchor;
    bool m_enabled = true;
    VisualsUtils::VisualsStyle m_style;

    SettingsGroup m_settings;
};
