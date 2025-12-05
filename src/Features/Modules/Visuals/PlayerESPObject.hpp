#pragma once

#include <string>

#include "ESPTypes.hpp"
#include "SDK/Game/PlayerRank.hpp"

struct DrawPlayer;

struct PlayerESPObject : public ESPObject {
    VRC::PlayerRank rank = VRC::PlayerRank::Visitor;
    std::string name;

    [[nodiscard]] ESPStyle applyStyle(const ESPStyle& base) const;
};

PlayerESPObject makePlayerESPObject(const DrawPlayer& player);
