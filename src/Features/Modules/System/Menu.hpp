#pragma once

#include <map>
#include <vector>

#include "Features/Framework/IFeature.hpp"

class Menu final : public IFeature {
public:
    Menu();

    void initialize() override;
    void onRender() override;
    void onMenuRender() override;

private:
    void renderFeatureSettings(IFeature* feature);

    std::map<FeatureCategory, std::vector<IFeature*>> m_featuresByCategory;
    FeatureCategory m_selectedCategory = FeatureCategory::System;
    IFeature* m_selectedFeature        = nullptr;
};
