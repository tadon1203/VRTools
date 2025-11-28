#include "HUDManager.hpp"

#include <cmath>

#include <imgui.h>

#include "Core/Logger.hpp"

HUDManager& HUDManager::instance() {
    static HUDManager inst;
    return inst;
}

void HUDManager::initializeAll() {
    Logger::instance().info("Initializing HUD components...");
    for (const auto& comp : m_components) {
        comp->initialize();
    }
}
void HUDManager::render() {
    ImDrawList* dl = ImGui::GetForegroundDrawList();

    if (m_editMode) {
        updateInput();
        drawGrid(dl);
        dl->AddText(ImVec2(10, 10), ImColor(0, 255, 0), "HUD EDIT MODE (Drag to move)");
    }

    for (const auto& comp : m_components) {
        comp->render(dl, m_editMode);
    }
}

void HUDManager::drawGrid(ImDrawList* dl) {
    ImVec2 size = ImGui::GetIO().DisplaySize;
    ImColor col = ImColor(255, 255, 255, 40);

    for (float x = 0; x < size.x; x += m_gridSize) {
        dl->AddLine(ImVec2(x, 0), ImVec2(x, size.y), col);
    }

    for (float y = 0; y < size.y; y += m_gridSize) {
        dl->AddLine(ImVec2(0, y), ImVec2(size.x, y), col);
    }
}
void HUDManager::updateInput() {
    ImGuiIO& io     = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    // Reset hover state
    for (auto& comp : m_components) {
        comp->m_isHovered = false;
    }

    // dragging
    if (m_draggedComponent) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_draggedComponent->m_isDragging = false;
            m_draggedComponent               = nullptr;
        } else {
            // snap logic
            float rawX = mousePos.x - m_dragOffset.x;
            float rawY = mousePos.y - m_dragOffset.y;

            int gridCountX = std::round(rawX / m_gridSize);
            int gridCountY = std::round(rawY / m_gridSize);

            float snapX = gridCountX * m_gridSize;
            float snapY = gridCountY * m_gridSize;

            m_draggedComponent->setPos(ImVec2(snapX, snapY));
        }
        return; // don't check new collisions while dragging
    }

    // collision detection
    for (auto it = m_components.rbegin(); it != m_components.rend(); ++it) {
        auto* comp  = it->get();
        ImVec2 pos  = comp->getPos();
        ImVec2 size = comp->getSize();

        if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y
            && mousePos.y <= pos.y + size.y) {

            comp->m_isHovered = true;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_draggedComponent = comp;
                comp->m_isDragging = true;
                m_dragOffset       = ImVec2(mousePos.x - pos.x, mousePos.y - pos.y);
            }
            break;
        }
    }
}
void HUDManager::loadConfig(const nlohmann::json& root) {
    if (!root.contains("HUD")) {
        return;
    }
    auto& j = root["HUD"];

    if (j.contains("EditMode")) {
        m_editMode = j["EditMode"];
    }

    if (j.contains("Components")) {
        for (const auto& comp : m_components) {
            if (j["Components"].contains(comp->getName())) {
                comp->onLoadConfig(j["Components"][comp->getName()]);
            }
        }
    }
}

nlohmann::json HUDManager::saveConfig() const {
    nlohmann::json root;
    root["EditMode"] = m_editMode;

    nlohmann::json comps;
    for (const auto& comp : m_components) {
        nlohmann::json c;
        comp->onSaveConfig(c);
        comps[comp->getName()] = c;
    }
    root["Components"] = comps;
    return root;
}
