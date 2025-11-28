#include "MenuManager.hpp"

#include <imgui.h>

#include "Input/CursorManager.hpp"

// Pages
#include "Pages/FeaturesMenu.hpp"
#include "Pages/HUDEditorMenu.hpp"
#include "Pages/MainMenu.hpp"
#include "Pages/SettingsMenu.hpp"

MenuManager& MenuManager::instance() {
    static MenuManager inst;
    return inst;
}

void MenuManager::initialize() {
    m_pages[MenuPageId::Main]      = std::make_shared<MainMenu>();
    m_pages[MenuPageId::Features]  = std::make_shared<FeaturesMenu>();
    m_pages[MenuPageId::Settings]  = std::make_shared<SettingsMenu>();
    m_pages[MenuPageId::HUDEditor] = std::make_shared<HUDEditorMenu>();

    for (auto& [id, page] : m_pages) {
        page->initialize();
    }
}

void MenuManager::navigateTo(MenuPageId pageId) {
    if (m_pages.find(pageId) != m_pages.end()) {
        m_currentPageId = pageId;
        m_pages[pageId]->onOpen();
    }
}

void MenuManager::toggle() {
    m_isOpen = !m_isOpen;

    if (m_isOpen) {
        // Reset to Main menu when opening, unless we were editing HUD
        if (m_currentPageId != MenuPageId::HUDEditor) {
            m_currentPageId = MenuPageId::Main;
        }
        CursorManager::instance().requestUnlock("Menu");
    } else {
        CursorManager::instance().releaseUnlock("Menu");
    }
}

bool MenuManager::isOpen() const { return m_isOpen; }

bool MenuManager::isHUDEditorOpen() const { return m_isOpen && m_currentPageId == MenuPageId::HUDEditor; }

void MenuManager::render() {
    if (!m_isOpen) {
        return;
    }

    auto& currentPage = m_pages[m_currentPageId];
    if (!currentPage) {
        return;
    }

    // Render Fullscreen Background
    if (currentPage->showsBackground()) {
        renderBackground();
    }

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;
    ImVec2 center     = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

    if (currentPage->showsBackground()) {
        ImVec2 windowSize = currentPage->getWindowSize();

        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.95f));

        if (ImGui::Begin("VRToolsMenu", nullptr, flags)) {
            currentPage->render(center, windowSize);
        }
        ImGui::End();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    } else {
        currentPage->render(center, screenSize);
    }
}

void MenuManager::renderBackground() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 size    = ImGui::GetIO().DisplaySize;
    dl->AddRectFilled(ImVec2(0, 0), size, ImColor(0.0f, 0.0f, 0.0f, 0.5f));
}
