#include "Flight.hpp"

#include <imgui.h>

#include "Input/InputManager.hpp"
#include "SDK/Game/Networking.hpp"
#include "SDK/Game/VRCHelper.hpp"
#include "SDK/Game/VRCPlayerApi.hpp"
#include "SDK/Unity/Camera.hpp"
#include "SDK/Unity/CharacterController.hpp"
#include "SDK/Unity/GameObject.hpp"
#include "SDK/Unity/Vector3.hpp"

Flight::Flight()
    : IFeature(FeatureCategory::Player, "Flight") {
    m_settings.add("Speed", &m_speed, 4.0f);
}

void Flight::onMenuRender() { ImGui::SliderFloat("Speed", &m_speed, 0.1f, 20.0f, "%.1f"); }

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

    auto* trans = local->gameObject->getTransform();

    UnityEngine::Vector3 forward = trans->getForward();
    UnityEngine::Vector3 right   = trans->getRight();
    UnityEngine::Vector3 up      = UnityEngine::Vector3::Up();

    UnityEngine::Vector3 moveDir = UnityEngine::Vector3::Zero();
    auto& input                  = InputManager::instance();

    if (input.isKeyDown('W')) {
        moveDir = moveDir + forward;
    }
    if (input.isKeyDown('S')) {
        moveDir = moveDir - forward;
    }
    if (input.isKeyDown('D')) {
        moveDir = moveDir + right;
    }
    if (input.isKeyDown('A')) {
        moveDir = moveDir - right;
    }
    if (input.isKeyDown(VK_SPACE)) {
        moveDir = moveDir + up;
    }
    if (input.isKeyDown(VK_LCONTROL) || input.isKeyDown(VK_CONTROL)) {
        moveDir = moveDir - up;
    }

    if (moveDir.magnitude() > 0.001f) {
        float currentSpeed = m_speed;
        if (input.isKeyDown(VK_SHIFT) || input.isKeyDown(VK_LSHIFT)) {
            currentSpeed *= 2.0f;
        }

        // We prevent faster diagonal movement
        moveDir = moveDir.normalized();

        float dt = ImGui::GetIO().DeltaTime;

        UnityEngine::Vector3 pos = trans->getPosition();
        trans->setPosition(pos + (moveDir * currentSpeed * dt));
    }
}

UnityEngine::CharacterController* Flight::findController(VRC::VRCPlayerApi* player) {
    if (!player) {
        return nullptr;
    }

    return player->gameObject->getComponent<UnityEngine::CharacterController>();
}
