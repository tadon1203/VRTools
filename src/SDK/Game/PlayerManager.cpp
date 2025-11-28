#include "PlayerManager.hpp"

#include "../Unity/Camera.hpp"
#include "../Unity/GameObject.hpp"
#include "../Unity/Transform.hpp"
#include "VRCHelper.hpp"
#include "VRCPlayerApi.hpp"

PlayerManager& PlayerManager::instance() {
    static PlayerManager inst;
    return inst;
}

void PlayerManager::update() {
    if (!VRC::isInWorld()) {
        std::lock_guard lock(m_mutex);
        m_players.clear();
        return;
    }

    auto* camera = UnityEngine::Camera::get_main();
    if (!camera) {
        return;
    }

    std::vector<VRC::VRCPlayerApi*> gamePlayers = VRC::VRCPlayerApi::getAllPlayers();
    std::vector<CachedPlayer> tempCache;
    tempCache.reserve(gamePlayers.size());

    for (auto* player : gamePlayers) {
        if (!player || !player->gameObject) {
            continue;
        }

        CachedPlayer p;
        p.id      = player->playerId;
        p.isLocal = player->isLocal;

        // Identity
        if (player->displayName) {
            p.displayName = player->displayName->toString();
        } else {
            p.displayName = "Unknown";
        }

        auto* go        = reinterpret_cast<UnityEngine::GameObject*>(player->gameObject);
        auto* transform = go->getTransform();
        if (!transform) {
            continue;
        }

        p.position = transform->get_position();

        p.headPosition = p.position + UnityEngine::Vector3(0, 1.8f, 0);

        p.rootScreen = camera->WorldToScreenPoint(p.position);
        p.headScreen = camera->WorldToScreenPoint(p.headPosition);

        if (p.rootScreen.z > 0 && p.headScreen.z > 0) {
            p.isVisible = true;
        } else {
            p.isVisible = false;
        }

        tempCache.push_back(p);
    }

    {
        std::lock_guard lock(m_mutex);
        m_players = std::move(tempCache);
    }
}

std::vector<CachedPlayer> PlayerManager::getPlayers() {
    std::lock_guard lock(m_mutex);
    return m_players;
}
