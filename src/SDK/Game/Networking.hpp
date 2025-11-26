#pragma once

#include "VRCPlayerApi.hpp"

namespace VRC {
    class Networking {
    public:
        static VRCPlayerApi* getLocalPlayer();
    };
}
