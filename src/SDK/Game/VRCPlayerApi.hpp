#pragma once

#include <vector>

#include "../Unity/HumanBodyBones.hpp"
#include "../Unity/Transform.hpp"
#include "SDK/Il2Cpp/Structs.hpp"

namespace VRC {

    class alignas(8) VRCPlayerApi : public Il2CppObject {
    public:
        bool isLocal;
        Il2CppString* displayName;
        int32_t playerId;
        Il2CppObject* gameObject;

        static std::vector<VRCPlayerApi*> getAllPlayers();

        float getAvatarEyeHeightAsMeters();
        UnityEngine::Transform* getBoneTransform(UnityEngine::HumanBodyBones bone);
    };
}
