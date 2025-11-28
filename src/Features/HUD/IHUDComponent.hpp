#pragma once

#include <string>

#include <imgui.h>
#include <nlohmann/json.hpp>

enum class HUDAnchor { TopLeft, TopRight, BottomLeft, BottomRight };

class IHUDComponent {
public:
    virtual ~IHUDComponent() = default;

    explicit IHUDComponent(std::string name, ImVec2 defaultPos);

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

    virtual void onLoadConfig(const nlohmann::json& j);
    virtual void onSaveConfig(nlohmann::json& j) const;

protected:
    std::string m_name;
    ImVec2 m_pos;
    HUDAnchor m_anchor = HUDAnchor::TopLeft;
    ImVec2 m_anchorOffset;
    bool m_enabled = true;
};
