#pragma once

#include "VRCPlayerApi.hpp"

namespace VRC {
    // we dont need inheritance
    class Networking {
    public:
        IL2CPP_BINDING("VRCSDKBase.dll", "VRC.SDKBase", "Networking");

        static VRCPlayerApi* getLocalPlayer() { return callStatic<VRCPlayerApi*>("get_LocalPlayer"); }
    };
}
