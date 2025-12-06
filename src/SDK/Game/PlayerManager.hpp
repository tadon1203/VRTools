#pragma once

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <imgui.h>

#include "PlayerRank.hpp"
#include "SDK/Unity/Camera.hpp"
#include "VRCPlayerApi.hpp"

// this must be safe for the Rendering thread
struct DrawPlayer {
    std::string name;
    std::string userID;
    float distance;
    bool isVisible;
    bool isLocal;
    VRC::PlayerRank rank;
    bool rankResolved = false;

    ImVec2 rectMin;
    ImVec2 rectMax;
    std::array<ImVec2, 8> corners3d;
    std::vector<std::pair<ImVec2, ImVec2>> bones;
};

class PlayerManager {
public:
    static PlayerManager& instance();

    void update();

    [[nodiscard]] std::vector<DrawPlayer> getDrawPlayers();

private:
    PlayerManager()  = default;
    ~PlayerManager() = default;

    void updatePlayer(
        DrawPlayer& player, VRC::VRCPlayerApi* api, UnityEngine::Camera* cam, const UnityEngine::Vector3& localPos);
    void resolveRank(DrawPlayer& player, VRC::VRCPlayerApi* api);
    void generateBounds(DrawPlayer& player, VRC::VRCPlayerApi* api, UnityEngine::Camera* cam);
    void generateBones(DrawPlayer& player, VRC::VRCPlayerApi* api, UnityEngine::Camera* cam);

    std::unordered_map<VRC::VRCPlayerApi*, DrawPlayer> m_playerMap;

    std::vector<DrawPlayer> m_renderList;
    std::mutex m_mutex;
};
