#pragma once

#include "../IMenu.hpp"

class MainMenu : public IMenu {
public:
    void render(ImVec2 center, ImVec2 size) override;
    [[nodiscard]] ImVec2 getWindowSize() const override { return ImVec2(300, 390); }
};
