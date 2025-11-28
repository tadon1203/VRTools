#pragma once

#include <map>
#include <vector>

#include "../IMenu.hpp"
#include "Features/FeatureCategory.hpp"
#include "Features/IFeature.hpp"

class FeaturesMenu : public IMenu {
public:
    void initialize() override;
    void render(ImVec2 center, ImVec2 size) override;

private:
    void renderFeatureSettings(IFeature* feature);

    std::map<FeatureCategory, std::vector<IFeature*>> m_featuresByCategory;
    FeatureCategory m_selectedCategory = FeatureCategory::System;
    IFeature* m_selectedFeature        = nullptr;
};
