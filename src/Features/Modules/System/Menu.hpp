#pragma once

#include <map>
#include <vector>

#include <imgui.h>

#include "Features/Framework/IFeature.hpp"

enum class MenuView {
    Main,
    Features,
    Settings,
};

class Menu final : public IFeature {
public:
    Menu();

    void initialize() override;
    void onRender() override;
    void onMenuRender() override;

    void onEnable() override;
    void onDisable() override;

private:
    void renderBackground();
    void renderMainView(ImVec2 center, ImVec2 size);
    void renderFeaturesView(ImVec2 center, ImVec2 size);
    void renderSettingsView(ImVec2 center, ImVec2 size);

    void renderFeatureSettings(IFeature* feature);

    MenuView m_currentView = MenuView::Main;

    std::map<FeatureCategory, std::vector<IFeature*>> m_featuresByCategory;
    FeatureCategory m_selectedCategory = FeatureCategory::System;
    IFeature* m_selectedFeature        = nullptr;
};
