#pragma once

#include <memory>
#include <vector>

#include "Core/Settings/ISettingsHandler.hpp"
#include "HUDComponent.hpp"

class HUDManager : public ISettingsHandler {
public:
    static HUDManager& instance();

    void initializeAll();
    void render(bool isEditMode);

    template <typename T>
    void registerComponent() {
        auto comp = std::make_unique<T>();
        m_group.addChild(comp->getName(), &comp->getSettings());
        m_components.push_back(std::move(comp));
    }

    [[nodiscard]] std::string getSectionName() const override { return "HUD"; }
    SettingsGroup& getSettings() override { return m_group; }

private:
    HUDManager()           = default;
    ~HUDManager() override = default;

    void updateInput();
    void drawGrid(ImDrawList* dl);

    std::vector<std::unique_ptr<HUDComponent>> m_components;
    float m_gridSize                 = 20.0f;
    HUDComponent* m_draggedComponent = nullptr;
    ImVec2 m_dragOffset{ 0, 0 };

    SettingsGroup m_group;
};
