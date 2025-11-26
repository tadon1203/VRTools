#pragma once

#include <memory>
#include <vector>

#include "IFeature.hpp"
#include <nlohmann/json.hpp>

class FeatureManager {
public:
    static FeatureManager& instance();

    template <typename T>
    void registerFeature() {
        static_assert(std::is_base_of_v<IFeature, T>, "T must be derived from IFeature");
        m_features.push_back(std::make_unique<T>());
    }

    void initializeAll();
    void updateAll();
    void renderAll();
    bool onEventAll(Photon::EventData* eventData);
    bool onRaiseEventAll(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    void loadConfig(const nlohmann::json& root);
    [[nodiscard]] nlohmann::json saveConfig() const;

    [[nodiscard]] const std::vector<std::unique_ptr<IFeature>>& getFeatures() const { return m_features; }

private:
    FeatureManager()  = default;
    ~FeatureManager() = default;

    std::vector<std::unique_ptr<IFeature>> m_features;
};
