#pragma once

#include <memory>
#include <unordered_map>

#include "IMenu.hpp"

enum class MenuPageId { Main, Features, Settings, HUDEditor };

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

    void renderBackground();

    bool m_isOpen = false;

    // Page Management
    MenuPageId m_currentPageId = MenuPageId::Main;
    std::unordered_map<MenuPageId, std::shared_ptr<IMenu>> m_pages;
};
