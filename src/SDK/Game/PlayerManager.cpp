#include "PlayerManager.hpp"

#include <algorithm>
#include <unordered_set>

#include "Networking.hpp"
#include "SDK/Unity/Bounds.hpp"
#include "SDK/Unity/GameObject.hpp"
#include "SDK/Unity/Transform.hpp"
#include "Utils/Math.hpp"
#include "VRCHelper.hpp"
#include "VRC_Player.hpp"

using namespace UnityEngine;

PlayerManager& PlayerManager::instance() {
    static PlayerManager inst;
    return inst;
}

void PlayerManager::update() {
    if (!VRC::isInWorld()) {
        std::lock_guard lock(m_mutex);
        m_playerMap.clear();
        m_renderList.clear();
        return;
    }

    auto* cam = Camera::get_main();
    if (!cam) {
        return;
    }

    Vector3 localPos = Vector3::Zero();
    if (auto* lp = VRC::Networking::getLocalPlayer()) {
        if (lp->gameObject) {
            auto* t = lp->gameObject->getTransform();
            if (t) {
                localPos = t->getPosition();
            }
        }
    }

    std::vector<VRC::VRCPlayerApi*> gamePlayers = VRC::VRCPlayerApi::getAllPlayers();

    std::unordered_set<VRC::VRCPlayerApi*> currentIds;
    currentIds.reserve(gamePlayers.size());

    for (auto* api : gamePlayers) {
        if (!api || !api->gameObject) {
            continue;
        }

        currentIds.insert(api);

        auto [it, inserted] = m_playerMap.try_emplace(api);
        if (inserted) {
            DrawPlayer& player = it->second;
            player.isLocal     = api->isLocal;
            player.name        = api->displayName ? api->displayName->toString() : "Unknown";
            player.userID      = std::to_string(api->playerId);
            player.rank        = VRC::PlayerRank::Visitor;
            player.bones.reserve(60);
        }
    }

    // Remove disconnected players
    for (auto it = m_playerMap.begin(); it != m_playerMap.end();) {
        if (currentIds.find(it->first) == currentIds.end()) {
            it = m_playerMap.erase(it);
        } else {
            ++it;
        }
    }

    std::vector<DrawPlayer> tempRenderList;
    tempRenderList.reserve(m_playerMap.size());

    for (auto& [api, player] : m_playerMap) {
        // Skip local player for rendering data
        if (player.isLocal) {
            continue;
        }

        updatePlayer(player, api, cam, localPos);
        tempRenderList.push_back(player);
    }

    std::lock_guard lock(m_mutex);
    m_renderList = std::move(tempRenderList);
}

void PlayerManager::updatePlayer(DrawPlayer& player, VRC::VRCPlayerApi* api, Camera* cam, const Vector3& localPos) {
    if (!api || !api->gameObject) {
        player.isVisible = false;
        return;
    }

    auto* tr = api->gameObject->getTransform();
    if (!tr) {
        return;
    }

    Vector3 pos     = tr->getPosition();
    player.distance = pos.distance(localPos);

    resolveRank(player, api);

    generateBounds(player, api, cam);

    generateBones(player, api, cam);
}

void PlayerManager::resolveRank(DrawPlayer& player, VRC::VRCPlayerApi* api) {
    if (player.rankResolved && player.rank != VRC::PlayerRank::Visitor) {
        return;
    }

    auto* vrcPlayer = VRC::VRC_Player::get(api);
    if (!vrcPlayer) {
        return;
    }

    auto* user = vrcPlayer->get_user();
    if (!user) {
        return;
    }

    auto tags   = user->getTags();
    player.rank = VRC::PlayerRank::Visitor;

    bool isModerator     = false;
    bool isTroll         = false;
    bool isProbableTroll = false;

    for (const auto& t : tags) {
        if (t == "admin_moderator") {
            isModerator = true;
        }
        if (t == "system_troll") {
            isTroll = true;
        }
        if (t == "system_probable_troll") {
            isProbableTroll = true;
        }

        if (t == "system_trust_veteran") {
            player.rank = VRC::PlayerRank::Trusted;
        } else if (t == "system_trust_trusted") {
            player.rank = VRC::PlayerRank::Known;
        } else if (t == "system_trust_known") {
            player.rank = VRC::PlayerRank::User;
        } else if (t == "system_trust_basic") {
            player.rank = VRC::PlayerRank::NewUser;
        }
    }

    if (isModerator) {
        player.rank = VRC::PlayerRank::Moderator;
    } else if (isTroll) {
        player.rank = VRC::PlayerRank::Troll;
    } else if (isProbableTroll) {
        player.rank = VRC::PlayerRank::ProbableTroll;
    }

    player.rankResolved = true;
}

void PlayerManager::generateBounds(DrawPlayer& dp, VRC::VRCPlayerApi* api, Camera* cam) {
    auto* p = api;

    auto* head  = p->getBoneTransform(HumanBodyBones::Head);
    auto* footL = p->getBoneTransform(HumanBodyBones::LeftFoot);
    auto* footR = p->getBoneTransform(HumanBodyBones::RightFoot);

    // non humanoid fallback
    if (!head) {
        Vector3 pos = p->gameObject->getTransform()->getPosition();
        Vector3 s   = cam->worldToScreenPoint(pos);
        if (s.z > 0) {
            ImVec2 scr   = Utils::Math::unityToImGui(s);
            dp.rectMin   = { scr.x - 10, scr.y - 20 };
            dp.rectMax   = { scr.x + 10, scr.y + 20 };
            dp.isVisible = true;
        } else {
            dp.isVisible = false;
        }
        return;
    }

    // Calculate center and height based on humanoid bones
    Vector3 headPos = head->getPosition();
    Vector3 footPos = (footL && footR)
                        ? (footL->getPosition() + footR->getPosition()) / 2.0f
                        : (footL ? footL->getPosition()
                                 : (footR ? footR->getPosition() : p->gameObject->getTransform()->getPosition()));

    float height   = std::abs(headPos.y - footPos.y);
    Vector3 center = (headPos + footPos) / 2.0f;
    center.y += 0.1f;

    Vector3 extents(0.4f, (height * 0.5f) + 0.15f, 0.4f);

    // Generate 8 corners
    Vector3 c          = center;
    Vector3 e          = extents;
    Vector3 corners[8] = { { c.x - e.x, c.y - e.y, c.z - e.z }, { c.x + e.x, c.y - e.y, c.z - e.z },
        { c.x + e.x, c.y - e.y, c.z + e.z }, { c.x - e.x, c.y - e.y, c.z + e.z }, { c.x - e.x, c.y + e.y, c.z - e.z },
        { c.x + e.x, c.y + e.y, c.z - e.z }, { c.x + e.x, c.y + e.y, c.z + e.z }, { c.x - e.x, c.y + e.y, c.z + e.z } };

    float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
    bool anyPointOnScreen = false;

    for (int i = 0; i < 8; ++i) {
        Vector3 s = cam->worldToScreenPoint(corners[i]);

        if (s.z > 0) {
            dp.corners3d[i]  = Utils::Math::unityToImGui(s);
            anyPointOnScreen = true;

            minX = std::min(minX, dp.corners3d[i].x);
            maxX = std::max(maxX, dp.corners3d[i].x);
            minY = std::min(minY, dp.corners3d[i].y);
            maxY = std::max(maxY, dp.corners3d[i].y);
        } else {
            dp.corners3d[i] = { -99999, -99999 }; // Invalid
        }
    }

    dp.isVisible = anyPointOnScreen;
    if (anyPointOnScreen) {
        dp.rectMin = { minX, minY };
        dp.rectMax = { maxX, maxY };
    }
}

void PlayerManager::generateBones(DrawPlayer& dp, VRC::VRCPlayerApi* api, Camera* cam) {
    dp.bones.clear();

    static const std::pair<HumanBodyBones, HumanBodyBones> bonePairs[] = {
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

    auto* p = api;

    for (const auto& pair : bonePairs) {
        auto* t1 = p->getBoneTransform(pair.first);
        auto* t2 = p->getBoneTransform(pair.second);

        if (!t1 || !t2) {
            continue;
        }

        Vector3 s1 = cam->worldToScreenPoint(t1->getPosition());
        Vector3 s2 = cam->worldToScreenPoint(t2->getPosition());

        if (s1.z > 0.1f && s2.z > 0.1f) {
            dp.bones.emplace_back(Utils::Math::unityToImGui(s1), Utils::Math::unityToImGui(s2));
        }
    }
}

std::vector<DrawPlayer> PlayerManager::getDrawPlayers() {
    std::lock_guard lock(m_mutex);
    return m_renderList;
}
