#pragma once

#include "Wrappers/Il2Cpp/Structs.hpp"

namespace Photon {
    struct ParameterDictionary;

    struct alignas(8) EventData : Il2CppObject {
        uint8_t code;
        ParameterDictionary* parameters;
        uint8_t senderKey;
        int32_t sender;
        uint8_t customDataKey;
        Il2CppObject* customData;
    };
}
