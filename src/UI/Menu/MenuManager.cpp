#include "MenuManager.hpp"

#include <imgui.h>

#include "Input/CursorManager.hpp"
#include "Utils/Math.hpp"

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
    if (m_state == MenuState::Closed || m_state == MenuState::Closing) {
        m_state = MenuState::Opening;

        // Reset to Main menu when opening, unless we were editing HUD
        if (m_currentPageId != MenuPageId::HUDEditor) {
            m_currentPageId = MenuPageId::Main;
        }

        CursorManager::instance().requestUnlock("Menu");
    } else {
        m_state = MenuState::Closing;
    }
}

bool MenuManager::isOpen() const { return m_state != MenuState::Closed; }

bool MenuManager::isHUDEditorOpen() const { return isOpen() && m_currentPageId == MenuPageId::HUDEditor; }

void MenuManager::updateAnimation() {
    float dt             = ImGui::GetIO().DeltaTime;
    const float duration = 0.25f; // in seconds

    if (m_state == MenuState::Opening) {
        m_animTime += dt / duration;
        if (m_animTime >= 1.0f) {
            m_animTime = 1.0f;
            m_state    = MenuState::Open;
        }
    } else if (m_state == MenuState::Closing) {
        m_animTime -= dt / duration;
        if (m_animTime <= 0.0f) {
            m_animTime = 0.0f;
            m_state    = MenuState::Closed;
            CursorManager::instance().releaseUnlock("Menu");
        }
    }

    // Apply Easing
    m_alpha = Utils::Math::easeOutCubic(m_animTime);
}

void MenuManager::render() {
    if (m_state == MenuState::Closed) {
        return;
    }

    updateAnimation();

    auto& currentPage = m_pages[m_currentPageId];
    if (!currentPage) {
        return;
    }

    // Global Alpha
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

    // Scale animation (pop effect)
    float scale = 0.90f + (0.10f * m_alpha);

    if (currentPage->showsBackground()) {
        renderBackground();
    }

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;
    ImVec2 center     = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

    if (currentPage->showsBackground()) {
        ImVec2 windowSize = currentPage->getWindowSize();
        windowSize.x *= scale;
        windowSize.y *= scale;

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

    ImGui::PopStyleVar();
}

void MenuManager::renderBackground() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 size    = ImGui::GetIO().DisplaySize;
    dl->AddRectFilled(ImVec2(0, 0), size, ImColor(0.0f, 0.0f, 0.0f, 0.5f * m_alpha));
}
