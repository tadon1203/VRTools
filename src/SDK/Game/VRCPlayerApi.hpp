#pragma once

#include "SDK/Il2Cpp/Binding.hpp"
#include "SDK/Il2Cpp/Structs.hpp"
#include "SDK/Unity/GameObject.hpp"
#include "SDK/Unity/HumanBodyBones.hpp"
#include "SDK/Unity/Transform.hpp"
#include "SDK/Unity/Vector3.hpp"

namespace VRC {
    class VRCPlayerApi : public Il2CppObject {
    public:
        IL2CPP_BINDING("VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi");

        bool isLocal;
        Il2CppString* displayName;
        int32_t playerId;
        UnityEngine::GameObject* gameObject;

        static std::vector<VRCPlayerApi*> getAllPlayers() {
            auto* list = callStatic<Il2CppList<VRCPlayerApi*>*>("get_AllPlayers");
            return list ? list->toVector() : std::vector<VRCPlayerApi*>{};
        }

        UnityEngine::Transform* getBoneTransform(UnityEngine::HumanBodyBones bone) {
            return reinterpret_cast<UnityEngine::Transform*>(this->call<Il2CppObject*>("GetBoneTransform", bone));
        }

        void setVelocity(UnityEngine::Vector3 velocity) { this->call("SetVelocity", velocity); }
    };
}
