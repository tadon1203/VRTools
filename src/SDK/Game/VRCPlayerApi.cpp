#include "VRCPlayerApi.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

namespace VRC {
    std::vector<VRCPlayerApi*> VRCPlayerApi::getAllPlayers() {
        auto result = Il2Cpp::invoke<Il2CppObject*>("VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi", "get_AllPlayers");

        auto playerList = reinterpret_cast<Il2CppList<VRCPlayerApi*>*>(result);
        return playerList->toVector();
    }

    float VRCPlayerApi::getAvatarEyeHeightAsMeters() {
        return Il2Cpp::invoke<float>(
            this, "VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi", "GetAvatarEyeHeightAsMeters");
    }

    UnityEngine::Transform* VRCPlayerApi::getBoneTransform(UnityEngine::HumanBodyBones bone) {
        auto result = Il2Cpp::invoke<Il2CppObject*>(
            this, "VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi", "GetBoneTransform", bone);
        return reinterpret_cast<UnityEngine::Transform*>(result);
    }
}
