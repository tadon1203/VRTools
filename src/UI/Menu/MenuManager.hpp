#pragma once

#include <memory>
#include <unordered_map>

#include "IMenu.hpp"

enum class MenuPageId { Main, Features, Settings, HUDEditor };

enum class MenuState { Closed, Opening, Open, Closing };

class MenuManager {
public:
    static MenuManager& instance();

    void initialize();
    void render();
    void toggle();

    void navigateTo(MenuPageId pageId);

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] bool isHUDEditorOpen() const;

private:
    MenuManager()  = default;
    ~MenuManager() = default;

    void updateAnimation();
    void renderBackground();

    // Animation State
    MenuState m_state = MenuState::Closed;
    float m_animTime  = 0.0f;
    float m_alpha     = 0.0f;

    // Page Management
    MenuPageId m_currentPageId = MenuPageId::Main;
    std::unordered_map<MenuPageId, std::shared_ptr<IMenu>> m_pages;
};
