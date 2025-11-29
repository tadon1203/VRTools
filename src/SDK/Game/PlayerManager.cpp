#include "PlayerManager.hpp"

#include <map>

#include "../Unity/Camera.hpp"
#include "../Unity/GameObject.hpp"
#include "../Unity/HumanBodyBones.hpp"
#include "../Unity/Transform.hpp"
#include "Features/FeatureManager.hpp"
#include "Features/Modules/Visuals/ESP.hpp"
#include "VRCHelper.hpp"
#include "VRCPlayerApi.hpp"

using namespace UnityEngine;

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

    auto* camera = Camera::get_main();
    if (!camera) {
        return;
    }

    bool calcBones = false;
    if (auto* esp = FeatureManager::instance().getFeature<ESP>()) {
        calcBones = esp->isBoneEspEnabled();
    }

    std::vector<VRC::VRCPlayerApi*> gamePlayers = VRC::VRCPlayerApi::getAllPlayers();
    std::vector<CachedPlayer> tempCache;
    tempCache.reserve(gamePlayers.size());

    static const std::vector<std::pair<HumanBodyBones, HumanBodyBones>> bonePairs = {
        { HumanBodyBones::Head, HumanBodyBones::Neck },
        { HumanBodyBones::Neck, HumanBodyBones::Chest },
        { HumanBodyBones::Chest, HumanBodyBones::Spine },
        { HumanBodyBones::Spine, HumanBodyBones::Hips },

        { HumanBodyBones::Chest, HumanBodyBones::LeftShoulder },
        { HumanBodyBones::LeftShoulder, HumanBodyBones::LeftUpperArm },
        { HumanBodyBones::LeftUpperArm, HumanBodyBones::LeftLowerArm },
        { HumanBodyBones::LeftLowerArm, HumanBodyBones::LeftHand },

        { HumanBodyBones::Chest, HumanBodyBones::RightShoulder },
        { HumanBodyBones::RightShoulder, HumanBodyBones::RightUpperArm },
        { HumanBodyBones::RightUpperArm, HumanBodyBones::RightLowerArm },
        { HumanBodyBones::RightLowerArm, HumanBodyBones::RightHand },

        { HumanBodyBones::Hips, HumanBodyBones::LeftUpperLeg },
        { HumanBodyBones::LeftUpperLeg, HumanBodyBones::LeftLowerLeg },
        { HumanBodyBones::LeftLowerLeg, HumanBodyBones::LeftFoot },

        { HumanBodyBones::Hips, HumanBodyBones::RightUpperLeg },
        { HumanBodyBones::RightUpperLeg, HumanBodyBones::RightLowerLeg },
        { HumanBodyBones::RightLowerLeg, HumanBodyBones::RightFoot },
    };

    for (auto* player : gamePlayers) {
        if (!player || !player->gameObject) {
            continue;
        }

        CachedPlayer p;
        p.id      = player->playerId;
        p.isLocal = player->isLocal;

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

        float eyeHeight = player->getAvatarEyeHeightAsMeters();
        if (eyeHeight < 0.2f) {
            eyeHeight = 1.6f;
        }
        p.headPosition = p.position + Vector3(0, eyeHeight, 0);

        p.rootScreen = camera->WorldToScreenPoint(p.position);
        p.headScreen = camera->WorldToScreenPoint(p.headPosition);

        p.isVisible = (p.rootScreen.z > 0 && p.headScreen.z > 0);

        if (calcBones && p.isVisible && !p.isLocal) {
            std::map<HumanBodyBones, Transform*> boneTransforms;

            auto getBone = [&](HumanBodyBones b) -> Transform* {
                if (boneTransforms.count(b)) {
                    return boneTransforms[b];
                }
                Transform* t      = player->getBoneTransform(b);
                boneTransforms[b] = t;
                return t;
            };

            for (const auto& pair : bonePairs) {
                Transform* t1 = getBone(pair.first);
                Transform* t2 = getBone(pair.second);

                if (t1 && t2) {
                    Vector3 pos1 = t1->get_position();
                    Vector3 pos2 = t2->get_position();

                    Vector3 s1 = camera->WorldToScreenPoint(pos1);
                    Vector3 s2 = camera->WorldToScreenPoint(pos2);

                    if (s1.z > 0 && s2.z > 0) {
                        p.boneLines.emplace_back(s1, s2);
                    }
                }
            }
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
