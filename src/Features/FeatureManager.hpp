#pragma once

#include <memory>
#include <vector>

#include "Core/Settings/ISettingsHandler.hpp"
#include "IFeature.hpp"

class FeatureManager : public ISettingsHandler {
public:
    static FeatureManager& instance();

    template <typename T>
    void registerFeature() {
        m_features.push_back(std::make_unique<T>());
    }

    template <typename T>
    T* getFeature() {
        for (const auto& feature : m_features) {
            if (auto* ptr = dynamic_cast<T*>(feature.get())) {
                return ptr;
            }
        }
        return nullptr;
    }

    void initializeAll();

    [[nodiscard]] const std::vector<std::unique_ptr<IFeature>>& getFeatures() const { return m_features; }

    [[nodiscard]] std::string getSectionName() const override { return "Features"; }
    [[nodiscard]] nlohmann::json onSaveConfig() const override;
    void onLoadConfig(const nlohmann::json& section) override;

private:
    FeatureManager()           = default;
    ~FeatureManager() override = default;

    std::vector<std::unique_ptr<IFeature>> m_features;
};
