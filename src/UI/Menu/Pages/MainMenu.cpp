#include "MainMenu.hpp"

#include "../MenuManager.hpp"
#include "Core/Version.hpp"

void MainMenu::render(ImVec2 center, ImVec2 size) {
    float contentHeight = 250.0f;
    ImGui::SetCursorPosY((size.y - contentHeight) * 0.5f);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    float textWidth = ImGui::CalcTextSize(Version::PROJECT_NAME.data()).x;
    ImGui::SetCursorPosX((size.x - textWidth) * 0.5f);
    ImGui::Text("%s", Version::PROJECT_NAME.data());
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    float btnWidth  = 200.0f;
    float btnHeight = 40.0f;
    float btnX      = (size.x - btnWidth) * 0.5f;

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Features", ImVec2(btnWidth, btnHeight))) {
        MenuManager::instance().navigateTo(MenuPageId::Features);
    }

    ImGui::Spacing();

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("HUD Editor", ImVec2(btnWidth, btnHeight))) {
        MenuManager::instance().navigateTo(MenuPageId::HUDEditor);
    }

    ImGui::Spacing();

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Configuration", ImVec2(btnWidth, btnHeight))) {
        MenuManager::instance().navigateTo(MenuPageId::Settings);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Close", ImVec2(btnWidth, btnHeight))) {
        MenuManager::instance().toggle();
    }
}
