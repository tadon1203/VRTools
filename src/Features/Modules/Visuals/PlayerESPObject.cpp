#include "PlayerESPObject.hpp"

#include "SDK/Game/PlayerManager.hpp"

ESPStyle PlayerESPObject::applyStyle(const ESPStyle& base) const {
    ESPStyle style = base;
    if (style.colorMode == ColorMode::Rank) {
        style.primaryColor = VRC::getRankColor(rank);
    }
    return style;
}

PlayerESPObject makePlayerESPObject(const DrawPlayer& player) {
    PlayerESPObject obj;
    obj.rectMin   = player.rectMin;
    obj.rectMax   = player.rectMax;
    obj.corners3d = player.corners3d;
    obj.is3dValid = true;
    obj.bones     = player.bones;
    obj.distance  = player.distance;
    obj.rank      = player.rank;
    obj.name      = player.name;
    return obj;
}
