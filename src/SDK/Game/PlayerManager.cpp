#include "PlayerManager.hpp"

#include <algorithm>

#include "../Unity/Camera.hpp"
#include "../Unity/CharacterController.hpp"
#include "../Unity/GameObject.hpp"
#include "../Unity/Transform.hpp"
#include "Core/Logger.hpp"
#include "Features/FeatureManager.hpp"
#include "Features/Modules/Visuals/ESP.hpp"
#include "Networking.hpp"
#include "Utils/Math.hpp"
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
        m_drawList.clear();
        return;
    }

    auto* esp        = FeatureManager::instance().getFeature<ESP>();
    bool calcVisuals = esp && esp->isEnabled();

    if (!calcVisuals) {
        std::lock_guard lock(m_mutex);
        m_drawList.clear();
        return;
    }

    auto* cam = Camera::get_main();
    if (!cam) {
        return;
    }

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

    auto gamePlayers = VRC::VRCPlayerApi::getAllPlayers();
    std::vector<DrawPlayer> tempDraw;
    tempDraw.reserve(gamePlayers.size());

    Vector3 localPos = Vector3::Zero();
    if (auto* lp = VRC::Networking::getLocalPlayer()) {
        if (lp->gameObject) {
            auto* t = lp->gameObject->getTransform();
            if (t) {
                localPos = t->get_position();
            }
        }
    }

    bool needBones = esp->isBoneEspEnabled();
    bool needBox3D = esp->isBox3dEnabled();

    for (auto* p : gamePlayers) {
        if (!p || !p->gameObject) {
            continue;
        }

        auto* go = p->gameObject;
        auto* tr = go->getTransform();
        if (!tr) {
            continue;
        }

        DrawPlayer dp;
        dp.isLocal = p->isLocal;
        if (dp.isLocal) {
            continue; // Skip local for ESP
        }

        dp.isVisible = false;
        Vector3 pos  = tr->get_position();
        dp.distance  = pos.distance(localPos);
        dp.name      = p->displayName ? p->displayName->toString() : "Player";

        // Bounds Logic
        auto* cc = go->getComponent<CharacterController>();
        if (cc) {
            Bounds b  = cc->get_bounds();
            Vector3 c = b.center;
            Vector3 e = b.extents;

            Vector3 corners[8] = { { c.x - e.x, c.y - e.y, c.z - e.z }, { c.x + e.x, c.y - e.y, c.z - e.z },
                { c.x + e.x, c.y - e.y, c.z + e.z }, { c.x - e.x, c.y - e.y, c.z + e.z },
                { c.x - e.x, c.y + e.y, c.z - e.z }, { c.x + e.x, c.y + e.y, c.z - e.z },
                { c.x + e.x, c.y + e.y, c.z + e.z }, { c.x - e.x, c.y + e.y, c.z + e.z } };

            float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
            bool anyVis = false;

            for (int i = 0; i < 8; ++i) {
                Vector3 s = cam->worldToScreenPoint(corners[i]);
                if (needBox3D) {
                    dp.corners3d[i] = Utils::Math::unityToImGui(s);
                } else {
                    dp.corners3d[i] = { -1, -1 };
                }

                if (s.z > 0) {
                    anyVis   = true;
                    ImVec2 g = Utils::Math::unityToImGui(s);
                    minX     = std::min(minX, g.x);
                    maxX     = std::max(maxX, g.x);
                    minY     = std::min(minY, g.y);
                    maxY     = std::max(maxY, g.y);
                } else if (needBox3D) {
                    dp.corners3d[i] = { -9999, -9999 }; // Invalid
                }
            }

            if (anyVis) {
                dp.rectMin   = { minX, minY };
                dp.rectMax   = { maxX, maxY };
                dp.isVisible = true;
            }
        } else {
            // if no CC
            Logger::instance().info("no cc");
            Vector3 s = cam->worldToScreenPoint(pos);
            if (s.z > 0) {
                dp.isVisible = true;
            }
        }

        if (dp.isVisible && needBones) {
            for (const auto& pair : bonePairs) {
                auto* t1 = p->getBoneTransform(pair.first);
                auto* t2 = p->getBoneTransform(pair.second);
                if (t1 && t2) {
                    Vector3 s1 = cam->worldToScreenPoint(t1->get_position());
                    Vector3 s2 = cam->worldToScreenPoint(t2->get_position());
                    if (s1.z > 0 && s2.z > 0) {
                        dp.bones.emplace_back(Utils::Math::unityToImGui(s1), Utils::Math::unityToImGui(s2));
                    }
                }
            }
        }
        tempDraw.push_back(dp);
    }

    std::lock_guard lock(m_mutex);
    m_drawList = std::move(tempDraw);
}

std::vector<DrawPlayer> PlayerManager::getDrawPlayers() {
    std::lock_guard lock(m_mutex);
    return m_drawList;
}
