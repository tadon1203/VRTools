#include "MenuManager.hpp"

#include "Core/Settings/SettingsManager.hpp"
#include "Core/Version.hpp"
#include "Features/FeatureManager.hpp"
#include "HUD/HUDManager.hpp"
#include "Input/CursorManager.hpp"
#include "UI/NotificationManager.hpp"

MenuManager& MenuManager::instance() {
    static MenuManager inst;
    return inst;
}

void MenuManager::initialize() {
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
void MenuManager::toggle() {
    if (m_state == MenuState::Closed || m_state == MenuState::Closing) {
        m_state       = MenuState::Opening;
        m_currentView = MenuView::Main;
        CursorManager::instance().requestUnlock("Menu");
    } else {
        m_state = MenuState::Closing;
    }
}

bool MenuManager::isOpen() const { return m_state != MenuState::Closed; }

bool MenuManager::isHUDEditorOpen() const { return isOpen() && m_currentView == MenuView::HUDEditor; }

void MenuManager::render() {
    if (m_state == MenuState::Closed) {
        return;
    }

    // Animation Logic
    float dt          = ImGui::GetIO().DeltaTime;
    const float speed = 8.0f;

    if (m_state == MenuState::Opening) {
        m_alpha += dt * speed;
        if (m_alpha >= 1.0f) {
            m_alpha = 1.0f;
            m_state = MenuState::Open;
        }
    } else if (m_state == MenuState::Closing) {
        m_alpha -= dt * speed;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            m_state = MenuState::Closed;
            CursorManager::instance().releaseUnlock("Menu");
            return;
        }
    }

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

    float scale = 0.95f + (0.05f * m_alpha);
    ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));

    // Do not render full background/main window if in HUD Editor mode
    if (m_currentView == MenuView::HUDEditor) {
        renderHUDEditorToolbar(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 40.0f));
        ImGui::PopStyleVar();
        return;
    }

    renderBackground();

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;
    ImVec2 center     = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    ImVec2 windowSize;
    if (m_currentView == MenuView::Main) {
        windowSize = ImVec2(300, 390);
    } else {
        windowSize = ImVec2(800, 600);
    }

    windowSize.x *= scale;
    windowSize.y *= scale;

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
        case MenuView::HUDEditor:
            break;
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(); // Alpha
}

void MenuManager::renderBackground() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 size    = ImGui::GetIO().DisplaySize;
    dl->AddRectFilled(ImVec2(0, 0), size, ImColor(0.0f, 0.0f, 0.0f, 0.5f * m_alpha));
}

void MenuManager::renderMainView(ImVec2 center, ImVec2 size) {
    float contentHeight = 250.0f; // Adjusted for new button
    ImGui::SetCursorPosY((size.y - contentHeight) * 0.5f);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
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
    if (ImGui::Button("HUD Editor", ImVec2(btnWidth, btnHeight))) {
        m_currentView = MenuView::HUDEditor;
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
        toggle();
    }
}

void MenuManager::renderHUDEditorToolbar(ImVec2 center) {
    ImGui::SetNextWindowPos(ImVec2(center.x, 50), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(300, 80));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.90f));

    if (ImGui::Begin("HUDEditorToolbar", nullptr, flags)) {
        ImGui::Text("HUD Layout Editor");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextDisabled("Drag elements to move them.");

        ImGui::SameLine();
        float avail = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - 100);

        if (ImGui::Button("Save & Exit", ImVec2(100, 0))) {
            auto config = HUDManager::instance().saveConfig();
            SettingsManager::instance().saveToFile(config); // Or just HUD config depending on implementation
            m_currentView = MenuView::Main;
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void MenuManager::renderFeaturesView(ImVec2 center, ImVec2 size) {
    if (ImGui::Button("< Back")) {
        m_currentView = MenuView::Main;
    }
    ImGui::SameLine();
    ImGui::Text("Features");
    ImGui::Separator();

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

void MenuManager::renderSettingsView(ImVec2 center, ImVec2 size) {
    if (ImGui::Button("< Back")) {
        m_currentView = MenuView::Main;
    }
    ImGui::SameLine();
    ImGui::Text("Configuration");
    ImGui::Separator();

    ImGui::Spacing();

    if (ImGui::Button("Save Configuration")) {
        auto config = FeatureManager::instance().saveConfig();

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
}

void MenuManager::renderFeatureSettings(IFeature* feature) {
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
