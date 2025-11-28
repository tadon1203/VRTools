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
        m_components.push_back(std::make_unique<T>());
    }

    // ISettingsHandler
    [[nodiscard]] std::string getSectionName() const override { return "HUD"; }
    [[nodiscard]] nlohmann::json onSaveConfig() const override;
    void onLoadConfig(const nlohmann::json& section) override;

private:
    HUDManager()  = default;
    ~HUDManager() = default;

    void updateInput();
    void drawGrid(ImDrawList* dl);

    std::vector<std::unique_ptr<HUDComponent>> m_components;
    float m_gridSize                 = 20.0f;
    HUDComponent* m_draggedComponent = nullptr;
    ImVec2 m_dragOffset{ 0, 0 };
};
