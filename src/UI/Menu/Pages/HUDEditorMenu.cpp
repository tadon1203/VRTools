#include "HUDEditorMenu.hpp"

#include "../MenuManager.hpp"
#include "Core/Settings/SettingsManager.hpp"
#include "HUD/HUDManager.hpp"

void HUDEditorMenu::render(ImVec2 center, ImVec2 size) {
    // Toolbar
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
            SettingsManager::instance().saveToFile(config);
            MenuManager::instance().navigateTo(MenuPageId::Main);
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
