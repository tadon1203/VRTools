#include "Networking.hpp"

#include "SDK/Il2Cpp/Il2Cpp.hpp"

VRC::VRCPlayerApi* VRC::Networking::getLocalPlayer() {
    auto result = Il2Cpp::invoke<Il2CppObject*>("VRCSDKBase.dll", "VRC.SDKBase", "Networking", "get_LocalPlayer");
    return reinterpret_cast<VRCPlayerApi*>(result);
}
