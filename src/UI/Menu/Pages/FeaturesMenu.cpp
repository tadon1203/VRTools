#include "FeaturesMenu.hpp"

#include "../MenuManager.hpp"
#include "Features/FeatureManager.hpp"

void FeaturesMenu::initialize() {
    const auto& features = FeatureManager::instance().getFeatures();
    for (const auto& feature : features) {
        if (!feature->shouldShowInMenu()) {
            continue;
        }
        m_featuresByCategory[feature->getCategory()].push_back(feature.get());
    }

    if (!m_featuresByCategory.empty()) {
        m_selectedCategory = m_featuresByCategory.begin()->first;
        if (!m_featuresByCategory.begin()->second.empty()) {
            m_selectedFeature = m_featuresByCategory.begin()->second[0];
        }
    }
}

void FeaturesMenu::render(ImVec2 center, ImVec2 size) {
    if (ImGui::Button("< Back")) {
        MenuManager::instance().navigateTo(MenuPageId::Main);
    }
    ImGui::SameLine();
    ImGui::Text("Features");
    ImGui::Separator();

    if (ImGui::BeginTabBar("CategoryTabs")) {
        for (const auto& [category, features] : m_featuresByCategory) {
            if (ImGui::BeginTabItem(toString(category))) {
                m_selectedCategory = category;

                // Ensure selected feature is in this category
                bool found = false;
                if (m_selectedFeature) {
                    for (auto* f : features) {
                        if (f == m_selectedFeature) {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found && !features.empty()) {
                    m_selectedFeature = features[0];
                }

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::BeginChild("LeftPane", ImVec2(180, 0), true);
    auto& currentFeatures = m_featuresByCategory[m_selectedCategory];
    for (auto* feature : currentFeatures) {
        if (ImGui::Selectable(feature->getName().c_str(), m_selectedFeature == feature)) {
            m_selectedFeature = feature;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
    if (m_selectedFeature) {
        renderFeatureSettings(m_selectedFeature);
    }
    ImGui::EndChild();
}

void FeaturesMenu::renderFeatureSettings(IFeature* feature) {
    ImGui::PushID(feature);
    ImGui::Text("%s", feature->getName().c_str());
    ImGui::Separator();

    bool enabled = feature->isEnabled();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        feature->setEnabled(enabled);
    }

    if (auto k = feature->getKeybind()) {
        ImGui::SameLine();
        ImGui::TextDisabled("(Key: %d)", *k);
    }

    ImGui::Separator();
    feature->onMenuRender();
    ImGui::PopID();
}
