#include "IHUDComponent.hpp"

IHUDComponent::IHUDComponent(std::string name, ImVec2 defaultPos)
    : m_name(std::move(name))
    , m_pos(defaultPos)
    , m_anchorOffset(defaultPos) {}

void IHUDComponent::updatePosition(ImVec2 screenSize) {
    if (m_isDragging) {
        return;
    }

    ImVec2 size = getSize();
    switch (m_anchor) {
    case HUDAnchor::TopLeft:
        m_pos = m_anchorOffset;
        break;
    case HUDAnchor::TopRight:
        m_pos.x = screenSize.x - m_anchorOffset.x - size.x;
        m_pos.y = m_anchorOffset.y;
        break;
    case HUDAnchor::BottomLeft:
        m_pos.x = m_anchorOffset.x;
        m_pos.y = screenSize.y - m_anchorOffset.y - size.y;
        break;
    case HUDAnchor::BottomRight:
        m_pos.x = screenSize.x - m_anchorOffset.x - size.x;
        m_pos.y = screenSize.y - m_anchorOffset.y - size.y;
        break;
    }
}

void IHUDComponent::calculateAnchor(ImVec2 screenSize) {
    ImVec2 size   = getSize();
    ImVec2 center = ImVec2(m_pos.x + size.x * 0.5f, m_pos.y + size.y * 0.5f);

    bool isLeft = center.x < screenSize.x * 0.5f;
    bool isTop  = center.y < screenSize.y * 0.5f;

    if (isLeft && isTop) {
        m_anchor = HUDAnchor::TopLeft;
    } else if (!isLeft && isTop) {
        m_anchor = HUDAnchor::TopRight;
    } else if (isLeft && !isTop) {
        m_anchor = HUDAnchor::BottomLeft;
    } else {
        m_anchor = HUDAnchor::BottomRight;
    }

    switch (m_anchor) {
    case HUDAnchor::TopLeft:
        m_anchorOffset = m_pos;
        break;
    case HUDAnchor::TopRight:
        m_anchorOffset.x = screenSize.x - m_pos.x - size.x;
        m_anchorOffset.y = m_pos.y;
        break;
    case HUDAnchor::BottomLeft:
        m_anchorOffset.x = m_pos.x;
        m_anchorOffset.y = screenSize.y - m_pos.y - size.y;
        break;
    case HUDAnchor::BottomRight:
        m_anchorOffset.x = screenSize.x - m_pos.x - size.x;
        m_anchorOffset.y = screenSize.y - m_pos.y - size.y;
        break;
    }
}

void IHUDComponent::render(ImDrawList* drawList, bool isEditMode) {
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

        ImVec2 anchorPos = m_pos;
        switch (m_anchor) {
        case HUDAnchor::TopLeft:
            anchorPos = m_pos;
            break;
        case HUDAnchor::TopRight:
            anchorPos = ImVec2(max.x, m_pos.y);
            break;
        case HUDAnchor::BottomLeft:
            anchorPos = ImVec2(m_pos.x, max.y);
            break;
        case HUDAnchor::BottomRight:
            anchorPos = max;
            break;
        }
        drawList->AddCircleFilled(anchorPos, 4.0f, ImColor(255, 0, 0, 200));
    }

    if (m_enabled) {
        onRenderContent(drawList);
    }
}

void IHUDComponent::onLoadConfig(const nlohmann::json& j) {
    if (j.contains("Anchor")) {
        m_anchor = static_cast<HUDAnchor>(j["Anchor"]);
    }

    if (j.contains("OffsetX")) {
        m_anchorOffset.x = j["OffsetX"];
    } else if (j.contains("x")) {
        m_anchorOffset.x = j["x"];
    }

    if (j.contains("OffsetY")) {
        m_anchorOffset.y = j["OffsetY"];
    } else if (j.contains("y")) {
        m_anchorOffset.y = j["y"];
    }

    if (j.contains("Enabled")) {
        m_enabled = j["Enabled"];
    }

    if (m_anchor == HUDAnchor::TopLeft) {
        m_pos = m_anchorOffset;
    }
}

void IHUDComponent::onSaveConfig(nlohmann::json& j) const {
    j["Anchor"]  = static_cast<int>(m_anchor);
    j["OffsetX"] = m_anchorOffset.x;
    j["OffsetY"] = m_anchorOffset.y;
    j["Enabled"] = m_enabled;
}
