#pragma once

#include <cstdint>

namespace Photon {
    enum class DeliveryMode : int32_t { Unreliable, Reliable, UnreliableUnsequenced, ReliableUnsequenced };

    struct SendOptions {
        DeliveryMode deliveryMode;
        bool encrypt;
        uint8_t channel;
    };
}
