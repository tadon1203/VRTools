#include "SettingsMenu.hpp"

#include "../MenuManager.hpp"
#include "Core/Settings/SettingsManager.hpp"
#include "Features/FeatureManager.hpp"
#include "HUD/HUDManager.hpp"
#include "UI/NotificationManager.hpp"

void SettingsMenu::render(ImVec2 center, ImVec2 size) {
    if (ImGui::Button("< Back")) {
        MenuManager::instance().navigateTo(MenuPageId::Main);
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
