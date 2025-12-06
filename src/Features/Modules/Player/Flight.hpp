#pragma once

#include "Features/IFeature.hpp"

namespace UnityEngine {
    class CharacterController;
}

namespace VRC {
    class VRCPlayerApi;
}

class Flight : public IFeature {
public:
    Flight();

    void onEnable() override;
    void onDisable() override;
    void onUpdate() override;
    void onMenuRender() override;

private:
    UnityEngine::CharacterController* findController(VRC::VRCPlayerApi* player);

    UnityEngine::CharacterController* m_characterController = nullptr;
    float m_speed                                           = 4.0f;
};
