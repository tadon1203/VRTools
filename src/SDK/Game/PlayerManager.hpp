#pragma once

#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../Unity/Vector3.hpp"

struct CachedPlayer {
    int id;
    std::string displayName;
    bool isLocal;

    UnityEngine::Vector3 position; // 3D Feet
    UnityEngine::Vector3 headPosition; // 3D Head

    UnityEngine::Vector3 rootScreen; // 2D Feet
    UnityEngine::Vector3 headScreen; // 2D Head

    bool isVisible; // true if Z > 0

    // bone connections
    std::vector<std::pair<UnityEngine::Vector3, UnityEngine::Vector3>> boneLines;
};

class PlayerManager {
public:
    static PlayerManager& instance();

    void update();

    [[nodiscard]] std::vector<CachedPlayer> getPlayers();

private:
    PlayerManager()  = default;
    ~PlayerManager() = default;

    std::vector<CachedPlayer> m_players;
    std::mutex m_mutex;
};
