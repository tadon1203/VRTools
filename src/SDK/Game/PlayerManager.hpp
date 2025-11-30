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

// Data structure safe for the Rendering thread
struct DrawPlayer {
    std::string name;
    std::string userID;
    float distance;
    bool isVisible;
    bool isLocal;
    VRC::PlayerRank rank;

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

    struct PlayerEntry {
        VRC::VRCPlayerApi* api     = nullptr;
        VRC::PlayerRank cachedRank = VRC::PlayerRank::Visitor;
        bool rankResolved          = false;

        DrawPlayer drawData;
    };

    void updatePlayer(PlayerEntry& entry, UnityEngine::Camera* cam, const UnityEngine::Vector3& localPos);
    void resolveRank(PlayerEntry& entry);
    void generateBounds(PlayerEntry& entry, UnityEngine::Camera* cam);
    void generateBones(PlayerEntry& entry, UnityEngine::Camera* cam);

    std::unordered_map<VRC::VRCPlayerApi*, PlayerEntry> m_playerMap;

    std::vector<DrawPlayer> m_renderList;
    std::mutex m_mutex;
};
