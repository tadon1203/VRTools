#include "VRCHelper.hpp"

#include "Networking.hpp"

bool VRC::isInWorld() { return VRC::Networking::getLocalPlayer() != nullptr; }
