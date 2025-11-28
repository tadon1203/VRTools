#include "Menu.hpp"

#include <Windows.h>

#include <imgui.h>

#include "Core/Version.hpp"
#include "Features/Framework/FeatureManager.hpp"
#include "Features/HUD/HUDManager.hpp"
#include "Input/CursorManager.hpp"

Menu::Menu()
    : IFeature(FeatureCategory::System, "Menu", VK_INSERT) {}

void Menu::initialize() {
    const auto& features = FeatureManager::instance().getFeatures();
    for (const auto& feature : features) {
        m_featuresByCategory[feature->getCategory()].push_back(feature.get());
    }

    if (!m_featuresByCategory.empty()) {
        m_selectedCategory = m_featuresByCategory.begin()->first;
        if (!m_featuresByCategory.begin()->second.empty()) {
            m_selectedFeature = m_featuresByCategory.begin()->second[0];
        }
    }
}

void Menu::onEnable() {
    IFeature::onEnable();
    CursorManager::instance().setUnlock(true);
}

void Menu::onDisable() {
    IFeature::onDisable();
    CursorManager::instance().setUnlock(false);
}

void Menu::onMenuRender() {
    bool editMode = HUDManager::instance().isEditMode();

    if (ImGui::Checkbox("HUD Layout Editor", &editMode)) {
        HUDManager::instance().setEditMode(editMode);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enables a grid overlay. Drag and drop HUD elements to move them.");
    }

    if (editMode) {
        ImGui::Indent();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Edit Mode Active!");
        ImGui::TextDisabled("Close the menu to see the grid better.");
        ImGui::Unindent();
    }
}

void Menu::onRender() {
    if (!m_enabled) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Once);
    if (ImGui::Begin((std::string(Version::PROJECT_NAME) + " Menu").c_str(), nullptr)) {

        if (ImGui::BeginTabBar("CategoryTabs")) {
            for (const auto& [category, features] : m_featuresByCategory) {
                if (ImGui::BeginTabItem(toString(category))) {
                    m_selectedCategory = category;

                    bool found = false;
                    for (auto* f : features) {
                        if (f == m_selectedFeature) {
                            found = true;
                            break;
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

        ImGui::Separator();

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
    ImGui::End();
}

void Menu::renderFeatureSettings(IFeature* feature) {
    ImGui::PushID(feature);
    ImGui::Text("%s", feature->getName().c_str());
    ImGui::Separator();

    bool enabled = feature->isEnabled();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        feature->setEnabled(enabled);
    }

    ImGui::Separator();

    feature->onMenuRender();

    ImGui::PopID();
}
