#pragma once

#include "../IMenu.hpp"

class SettingsMenu : public IMenu {
public:
    void render(ImVec2 center, ImVec2 size) override;
};
