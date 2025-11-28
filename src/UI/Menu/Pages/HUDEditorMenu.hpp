#pragma once
#include "../IMenu.hpp"

class HUDEditorMenu : public IMenu {
public:
    void render(ImVec2 center, ImVec2 size) override;

    [[nodiscard]] bool showsBackground() const override { return false; }
};
