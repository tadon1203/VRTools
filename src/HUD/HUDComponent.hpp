#pragma once

#include <string>

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "Core/Settings/SettingsGroup.hpp"
#include "HUDDef.hpp"

class HUDComponent {
public:
    virtual ~HUDComponent() = default;

    explicit HUDComponent(std::string name, ImVec2 defaultPos)
        : m_name(std::move(name))
        , m_pos(defaultPos)
        , m_anchorOffset(defaultPos) {
        m_settings.add("Enabled", &m_enabled, true);
        m_settings.add("Anchor", reinterpret_cast<int*>(&m_anchor), 0);
        m_settings.add("Offset", &m_anchorOffset);
    }

    virtual void initialize() {}

    virtual void onRenderContent(ImDrawList* drawList) = 0;
    virtual ImVec2 getSize()                           = 0;

    void updatePosition(ImVec2 screenSize);
    void calculateAnchor(ImVec2 screenSize);
    void render(ImDrawList* drawList, bool isEditMode);

    [[nodiscard]] const std::string& getName() const { return m_name; }
    [[nodiscard]] ImVec2 getPos() const { return m_pos; }
    void setPos(ImVec2 p) { m_pos = p; }

    [[nodiscard]] bool isEnabled() const { return m_enabled; }
    void setEnabled(bool e) { m_enabled = e; }

    bool m_isHovered  = false;
    bool m_isDragging = false;

    SettingsGroup& getSettings() { return m_settings; }

protected:
    std::string m_name;
    ImVec2 m_pos;
    HUDAnchor m_anchor = HUDAnchor::TopLeft;
    ImVec2 m_anchorOffset;
    bool m_enabled = true;

    SettingsGroup m_settings;
};
