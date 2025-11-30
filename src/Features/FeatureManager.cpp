#include "FeatureManager.hpp"

#include "Core/Logger.hpp"

FeatureManager& FeatureManager::instance() {
    static FeatureManager inst;
    return inst;
}

void FeatureManager::initializeAll() {
    for (const auto& feature : m_features) {
        feature->initialize();
    }
}

void FeatureManager::onUpdate() {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            feature->onUpdate();
        }
    }
}

void FeatureManager::onRender() {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            feature->onRender();
        }
    }
}

bool FeatureManager::onPhotonEvent(Photon::EventData* eventData) {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            if (!feature->onPhotonEvent(eventData)) {
                return false; // Cancelled
            }
        }
    }
    return true;
}

bool FeatureManager::onRaiseEvent(
    uint8_t code, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            if (!feature->onRaiseEvent(code, content, raiseEventOptions, sendOptions)) {
                return false; // Cancelled
            }
        }
    }
    return true;
}
