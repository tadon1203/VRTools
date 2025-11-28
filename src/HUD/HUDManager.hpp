#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "HUDComponent.hpp"

class HUDManager {
public:
    static HUDManager& instance();

    void initializeAll();
    void render(bool isEditMode);

    template <typename T>
    void registerComponent() {
        static_assert(std::is_base_of_v<HUDComponent, T>, "T must be derived from HUDComponent");
        m_components.push_back(std::make_unique<T>());
    }

    void loadConfig(const nlohmann::json& root);
    [[nodiscard]] nlohmann::json saveConfig() const;

private:
    HUDManager()  = default;
    ~HUDManager() = default;

    void updateInput();
    void drawGrid(ImDrawList* dl);

    std::vector<std::unique_ptr<HUDComponent>> m_components;

    float m_gridSize = 20.0f;

    HUDComponent* m_draggedComponent = nullptr;
    ImVec2 m_dragOffset{ 0, 0 };
};
