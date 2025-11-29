#pragma once

#include <array>
#include <mutex>
#include <string>
#include <vector>

#include <imgui.h>

// this must contain ONLY SAFE data for the rendering thread
struct DrawPlayer {
    std::string name;
    float distance;
    bool isVisible;
    bool isLocal;

    ImVec2 rectMin;
    ImVec2 rectMax;
    std::array<ImVec2, 8> corners3d; // For 3D box
    std::vector<std::pair<ImVec2, ImVec2>> bones;
};

class PlayerManager {
public:
    static PlayerManager& instance();

    // Main Thread: Updates Raw pointers AND calculates Screen logic
    void update();

    // Render Thread: Returns safe data
    [[nodiscard]] std::vector<DrawPlayer> getDrawPlayers();

private:
    PlayerManager()  = default;
    ~PlayerManager() = default;

    std::vector<DrawPlayer> m_drawList;
    std::mutex m_mutex;
};
