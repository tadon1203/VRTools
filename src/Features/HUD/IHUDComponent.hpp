#pragma once

#include <string>

#include <imgui.h>
#include <nlohmann/json.hpp>

class IHUDComponent {
public:
    virtual ~IHUDComponent() = default;

    explicit IHUDComponent(std::string name, ImVec2 defaultPos)
        : m_name(std::move(name))
        , m_pos(defaultPos) {}

    virtual void initialize() {}

    virtual void onRenderContent(ImDrawList* drawList) = 0;
    virtual ImVec2 getSize()                           = 0;

    void render(ImDrawList* drawList, bool isEditMode) {
        if (!m_enabled && !isEditMode) {
            return;
        }

        if (isEditMode) {
            ImVec2 size = getSize();
            ImVec2 max  = ImVec2(m_pos.x + size.x, m_pos.y + size.y);

            ImColor borderColor = ImColor(255, 255, 255, 100);
            if (m_isHovered || m_isDragging) {
                borderColor = ImColor(0, 255, 0, 255);
                drawList->AddRectFilled(m_pos, max, ImColor(0, 255, 0, 40));
            }
            drawList->AddRect(m_pos, max, borderColor);
        }

        if (m_enabled) {
            onRenderContent(drawList);
        }
    }

    [[nodiscard]] const std::string& getName() const { return m_name; }
    [[nodiscard]] ImVec2 getPos() const { return m_pos; }
    void setPos(ImVec2 p) { m_pos = p; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool e) { m_enabled = e; }

    bool m_isHovered  = false;
    bool m_isDragging = false;

    virtual void onLoadConfig(const nlohmann::json& j) {
        if (j.contains("x")) {
            m_pos.x = j["x"];
        }
        if (j.contains("y")) {
            m_pos.y = j["y"];
        }
        if (j.contains("Enabled")) {
            m_enabled = j["Enabled"];
        }
    }

    virtual void onSaveConfig(nlohmann::json& j) const {
        j["x"]       = m_pos.x;
        j["y"]       = m_pos.y;
        j["Enabled"] = m_enabled;
    }

protected:
    std::string m_name;
    ImVec2 m_pos;
    bool m_enabled = true;
};
