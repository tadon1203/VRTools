#pragma once

#include <vector>

#include "SDK/Il2Cpp/Structs.hpp"

namespace VRC {

    class alignas(8) VRCPlayerApi : public Il2CppObject {
    public:
        bool isLocal;
        Il2CppString* displayName;
        int32_t playerId;
        Il2CppObject* gameObject;

        static std::vector<VRCPlayerApi*> getAllPlayers();
    };
}
