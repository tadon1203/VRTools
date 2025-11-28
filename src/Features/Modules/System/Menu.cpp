#include "Menu.hpp"

#include <Windows.h>

#include "Core/Settings/SettingsManager.hpp"
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
    m_currentView = MenuView::Main;
    CursorManager::instance().requestUnlock("Menu");
}

void Menu::onDisable() {
    IFeature::onDisable();
    CursorManager::instance().releaseUnlock("Menu");
}

void Menu::onMenuRender() {
    // This is for the "System" tab inside the Feature view itself
    ImGui::TextDisabled("This feature controls the main menu UI.");
}

void Menu::onRender() {
    if (!m_enabled) {
        return;
    }

    renderBackground();

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;
    ImVec2 center     = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

    // Common styling for windows
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    // We can vary size based on view
    ImVec2 windowSize;
    if (m_currentView == MenuView::Main) {
        windowSize = ImVec2(300, 350);
    } else {
        windowSize = ImVec2(800, 600);
    }
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.95f));

    if (ImGui::Begin("VRToolsMenu", nullptr, flags)) {
        switch (m_currentView) {
        case MenuView::Main:
            renderMainView(center, windowSize);
            break;
        case MenuView::Features:
            renderFeaturesView(center, windowSize);
            break;
        case MenuView::Settings:
            renderSettingsView(center, windowSize);
            break;
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void Menu::renderBackground() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 size    = ImGui::GetIO().DisplaySize;
    dl->AddRectFilled(ImVec2(0, 0), size, ImColor(0, 0, 0, 128));
}

void Menu::renderMainView(ImVec2 center, ImVec2 size) {
    float contentHeight = 200.0f;
    ImGui::SetCursorPosY((size.y - contentHeight) * 0.5f);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Assuming Font 0 is big/default
    float textWidth = ImGui::CalcTextSize(Version::PROJECT_NAME.data()).x;
    ImGui::SetCursorPosX((size.x - textWidth) * 0.5f);
    ImGui::Text("%s", Version::PROJECT_NAME.data());
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    float btnWidth  = 200.0f;
    float btnHeight = 40.0f;
    float btnX      = (size.x - btnWidth) * 0.5f;

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Features", ImVec2(btnWidth, btnHeight))) {
        m_currentView = MenuView::Features;
    }

    ImGui::Spacing();

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Configuration", ImVec2(btnWidth, btnHeight))) {
        m_currentView = MenuView::Settings;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Close", ImVec2(btnWidth, btnHeight))) {
        toggle(); // Closes menu
    }
}

void Menu::renderFeaturesView(ImVec2 center, ImVec2 size) {
    // Header
    if (ImGui::Button("< Back")) {
        m_currentView = MenuView::Main;
    }
    ImGui::SameLine();
    ImGui::Text("Features");
    ImGui::Separator();

    // Tab Bar Logic
    if (ImGui::BeginTabBar("CategoryTabs")) {
        for (const auto& [category, features] : m_featuresByCategory) {
            if (ImGui::BeginTabItem(toString(category))) {
                m_selectedCategory = category;

                // Ensure selection is valid
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

    ImGui::BeginChild("SplitView", ImVec2(0, 0), true);

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

    ImGui::EndChild();
}

void Menu::renderSettingsView(ImVec2 center, ImVec2 size) {
    if (ImGui::Button("< Back")) {
        m_currentView = MenuView::Main;
    }
    ImGui::SameLine();
    ImGui::Text("Configuration");
    ImGui::Separator();

    ImGui::Spacing();

    if (ImGui::Button("Save Configuration")) {
        auto config = FeatureManager::instance().saveConfig();

        // Add HUD config
        auto hudConfig = HUDManager::instance().saveConfig();
        if (hudConfig.contains("HUD")) {
            config["HUD"] = hudConfig["HUD"];
        }

        SettingsManager::instance().saveToFile(config);
        NotificationManager::instance().success("Config", "Settings saved to disk.");
    }

    ImGui::SameLine();

    if (ImGui::Button("Reload Configuration")) {
        auto config = SettingsManager::instance().loadFromFile();
        FeatureManager::instance().loadConfig(config);
        HUDManager::instance().loadConfig(config);
        NotificationManager::instance().success("Config", "Settings reloaded from disk.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool editMode = HUDManager::instance().isEditMode();
    if (ImGui::Checkbox("HUD Layout Editor", &editMode)) {
        HUDManager::instance().setEditMode(editMode);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows grid and allows moving HUD elements.");
    }
}

void Menu::renderFeatureSettings(IFeature* feature) {
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
