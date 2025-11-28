#include "SettingsMenu.hpp"

#include "../MenuManager.hpp"
#include "Core/Settings/SettingsManager.hpp"
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
        SettingsManager::instance().saveAll();
        NotificationManager::instance().success("Config", "Settings saved to disk.");
    }

    ImGui::SameLine();

    if (ImGui::Button("Reload Configuration")) {
        SettingsManager::instance().loadAll();
        NotificationManager::instance().success("Config", "Settings reloaded from disk.");
    }
}
