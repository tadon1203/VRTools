#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "HUDComponent.hpp"

class HUDManager {
public:
    static HUDManager& instance();

    void initializeAll();
    void render();

    template <typename T>
    void registerComponent() {
        static_assert(std::is_base_of_v<HUDComponent, T>, "T must be derived from HUDComponent");
        m_components.push_back(std::make_unique<T>());
    }

    void setEditMode(bool enabled);
    [[nodiscard]] bool isEditMode() const { return m_editMode; }

    void loadConfig(const nlohmann::json& root);
    [[nodiscard]] nlohmann::json saveConfig() const;

private:
    HUDManager()  = default;
    ~HUDManager() = default;

    void updateInput();
    void drawGrid(ImDrawList* dl);

    std::vector<std::unique_ptr<HUDComponent>> m_components;

    bool m_editMode  = false;
    float m_gridSize = 20.0f;

    HUDComponent* m_draggedComponent = nullptr;
    ImVec2 m_dragOffset{ 0, 0 };
};
