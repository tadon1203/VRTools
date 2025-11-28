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

    void initializeAll();
    void updateAll();
    void renderAll();
    bool onEventAll(Photon::EventData* eventData);
    bool onRaiseEventAll(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    [[nodiscard]] const std::vector<std::unique_ptr<IFeature>>& getFeatures() const { return m_features; }

    // ISettingsHandler
    [[nodiscard]] std::string getSectionName() const override { return "Features"; }
    [[nodiscard]] nlohmann::json onSaveConfig() const override;
    void onLoadConfig(const nlohmann::json& section) override;

private:
    FeatureManager()  = default;
    ~FeatureManager() = default;

    std::vector<std::unique_ptr<IFeature>> m_features;
};
