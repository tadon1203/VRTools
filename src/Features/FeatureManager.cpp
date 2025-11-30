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
