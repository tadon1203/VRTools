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

void FeatureManager::onLoadConfig(const nlohmann::json& section) {
    for (const auto& feature : m_features) {
        if (section.contains(feature->getName())) {
            try {
                feature->onLoadConfig(section[feature->getName()]);
            } catch (const std::exception& e) {
                Logger::instance().error("Error loading config for {}: {}", feature->getName(), e.what());
            }
        }
    }
}

nlohmann::json FeatureManager::onSaveConfig() const {
    nlohmann::json j = nlohmann::json::object();
    for (const auto& feature : m_features) {
        nlohmann::json f = nlohmann::json::object();
        feature->onSaveConfig(f);
        j[feature->getName()] = f;
    }
    return j;
}
