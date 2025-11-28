#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "IHUDComponent.hpp"

class HUDManager {
public:
    static HUDManager& instance();

    void initializeAll();
    void render(); // Called in Present

    template <typename T>
    void registerComponent() {
        static_assert(std::is_base_of_v<IHUDComponent, T>, "T must be derived from IHUDComponent");
        m_components.push_back(std::make_unique<T>());
    }

    void setEditMode(bool enabled) { m_editMode = enabled; }
    bool isEditMode() const { return m_editMode; }

    void loadConfig(const nlohmann::json& root);
    [[nodiscard]] nlohmann::json saveConfig() const;

private:
    HUDManager()  = default;
    ~HUDManager() = default;

    void updateInput();
    void drawGrid(ImDrawList* dl);

    std::vector<std::unique_ptr<IHUDComponent>> m_components;

    bool m_editMode  = false;
    float m_gridSize = 20.0f;

    IHUDComponent* m_draggedComponent = nullptr;
    ImVec2 m_dragOffset{ 0, 0 };
};
