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

void FeatureManager::updateAll() {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            feature->onUpdate();
        }
    }
}

void FeatureManager::renderAll() {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            feature->onRender();
        }
    }
}

bool FeatureManager::onEventAll(Photon::EventData* eventData) {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            if (!feature->onEvent(eventData)) {
                return false;
            }
        }
    }
    return true;
}

bool FeatureManager::onRaiseEventAll(
    uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {
    for (const auto& feature : m_features) {
        if (feature->isEnabled()) {
            if (!feature->onRaiseEvent(eventCode, content, raiseEventOptions, sendOptions)) {
                return false;
            }
        }
    }
    return true;
}

void FeatureManager::loadConfig(const nlohmann::json& root) {
    for (const auto& feature : m_features) {
        if (root.contains(feature->getName())) {
            try {
                feature->onLoadConfig(root[feature->getName()]);
            } catch (const std::exception& e) {
                Logger::instance().error("Error loading config for {}: {}", feature->getName(), e.what());
            }
        }
    }
}

nlohmann::json FeatureManager::saveConfig() const {
    nlohmann::json root;
    for (const auto& feature : m_features) {
        nlohmann::json j;
        feature->onSaveConfig(j);
        root[feature->getName()] = j;
    }
    return root;
}
