#pragma once

#include <imgui.h>

class IMenu {
public:
    virtual ~IMenu() = default;

    virtual void initialize() {}

    virtual void render(ImVec2 center, ImVec2 size) = 0;
    virtual void onOpen() {}

    [[nodiscard]] virtual bool showsBackground() const { return true; }

    [[nodiscard]] virtual ImVec2 getWindowSize() const { return ImVec2(800, 600); }
};
