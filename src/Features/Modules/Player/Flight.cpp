#include "Flight.hpp"

#include "SDK/Game/Networking.hpp"
#include "SDK/Game/VRCHelper.hpp"
#include "SDK/Game/VRCPlayerApi.hpp"
#include "SDK/Unity/CharacterController.hpp"
#include "SDK/Unity/GameObject.hpp"
#include "SDK/Unity/Vector3.hpp"

Flight::Flight()
    : IFeature(FeatureCategory::Player, "Flight") {}

void Flight::onEnable() {
    auto* local           = VRC::Networking::getLocalPlayer();
    m_characterController = findController(local);
    if (m_characterController) {
        m_characterController->setEnabled(false);
    }
}

void Flight::onDisable() {
    if (m_characterController) {
        m_characterController->setEnabled(true);
    }
    m_characterController = nullptr;
}

void Flight::onUpdate() {
    if (!VRC::isInWorld()) {
        return;
    }

    auto* local = VRC::Networking::getLocalPlayer();
    if (!local) {
        return;
    }

    local->setVelocity(UnityEngine::Vector3::Zero());

    if (!m_characterController) {
        m_characterController = findController(local);
    }

    if (m_characterController && m_characterController->getEnabled()) {
        m_characterController->setEnabled(false);
    }
}

UnityEngine::CharacterController* Flight::findController(VRC::VRCPlayerApi* player) {
    if (!player || !player->gameObject) {
        return nullptr;
    }

    return player->gameObject->getComponent<UnityEngine::CharacterController>();
}
