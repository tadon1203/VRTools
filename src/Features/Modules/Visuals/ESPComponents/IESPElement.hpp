#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "ESPDef.hpp"
#include "SDK/Game/PlayerManager.hpp"

class IESPElement {
public:
    virtual ~IESPElement() = default;

    explicit IESPElement(std::string name, ESPAnchor defaultAnchor = ESPAnchor::Top)
        : m_name(std::move(name))
        , m_anchor(defaultAnchor) {}

    virtual ImVec2 getSize(const DrawPlayer& player, const ESPContext& ctx) = 0;

    virtual void render(ImDrawList* dl, const DrawPlayer& player, const ESPContext& ctx, ImVec2 pos) = 0;

    virtual void onMenuRender() {
        ImGui::Checkbox(m_name.c_str(), &m_enabled);
        if (m_anchor != ESPAnchor::Center) {
            ImGui::SameLine();
            ImGui::PushID(m_name.c_str());
            int a = static_cast<int>(m_anchor);
            ImGui::SetNextItemWidth(80);
            if (ImGui::Combo("", &a, "Top\0Bottom\0Left\0Right")) {
                m_anchor = static_cast<ESPAnchor>(a);
            }
            ImGui::PopID();
        }
    }

    virtual void onLoadConfig(const nlohmann::json& j) {
        if (j.contains(m_name)) {
            const auto& s = j[m_name];
            if (s.contains("Enabled")) {
                m_enabled = s["Enabled"];
            }
            if (s.contains("Anchor")) {
                m_anchor = static_cast<ESPAnchor>(s["Anchor"]);
            }
        }
    }

    virtual void onSaveConfig(nlohmann::json& j) const {
        j[m_name] = { { "Enabled", m_enabled }, { "Anchor", m_anchor } };
    }

    [[nodiscard]] bool isEnabled() const { return m_enabled; }
    [[nodiscard]] ESPAnchor getAnchor() const { return m_anchor; }
    [[nodiscard]] const std::string& getName() const { return m_name; }

protected:
    std::string m_name;
    ESPAnchor m_anchor;
    bool m_enabled = true;
};
