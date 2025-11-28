#pragma once

#include <map>
#include <vector>

#include <imgui.h>

#include "Features/FeatureCategory.hpp"
#include "Features/IFeature.hpp"

enum class MenuView {
    Main,
    Features,
    Settings,
};

enum class MenuState { Closed, Opening, Open, Closing };

class MenuManager {
public:
    static MenuManager& instance();

    void initialize();
    void render(); // Called on Render Thread

    void toggle();
    [[nodiscard]] bool isOpen() const;

private:
    MenuManager()  = default;
    ~MenuManager() = default;

    void renderBackground();
    void renderMainView(ImVec2 center, ImVec2 size);
    void renderFeaturesView(ImVec2 center, ImVec2 size);
    void renderSettingsView(ImVec2 center, ImVec2 size);

    void renderFeatureSettings(IFeature* feature);

    // anim
    MenuState m_state = MenuState::Closed;
    float m_alpha     = 0.0f;

    // UI
    MenuView m_currentView = MenuView::Main;
    std::map<FeatureCategory, std::vector<IFeature*>> m_featuresByCategory;
    FeatureCategory m_selectedCategory = FeatureCategory::System;
    IFeature* m_selectedFeature        = nullptr;
};
