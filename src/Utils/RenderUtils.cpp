#include "RenderUtils.hpp"

namespace RenderUtils {

    void renderText(ImDrawList* dl, ImVec2 pos, const std::string& text, ImColor color, bool outline, float fontSize) {
        if (text.empty()) {
            return;
        }

        ImFont* font = ImGui::GetFont();
        if (fontSize <= 0.0f) {
            fontSize = ImGui::GetFontSize();
        }

        if (outline) {
            ImU32 black = ImColor(0, 0, 0, static_cast<int>(color.Value.w * 255));
            dl->AddText(font, fontSize, { pos.x - 1, pos.y }, black, text.c_str());
            dl->AddText(font, fontSize, { pos.x + 1, pos.y }, black, text.c_str());
            dl->AddText(font, fontSize, { pos.x, pos.y - 1 }, black, text.c_str());
            dl->AddText(font, fontSize, { pos.x, pos.y + 1 }, black, text.c_str());
        }

        dl->AddText(font, fontSize, pos, color, text.c_str());
    }

}
