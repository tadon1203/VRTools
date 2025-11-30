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
