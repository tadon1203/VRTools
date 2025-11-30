#pragma once

#include <memory>
#include <vector>

#include "Core/Settings/ISettingsHandler.hpp"
#include "IFeature.hpp"
#include "SDK/Game/Photon/SendOptions.hpp"

class FeatureManager : public ISettingsHandler {
public:
    static FeatureManager& instance();

    template <typename T>
    void registerFeature() {
        auto feature = std::make_unique<T>();
        m_group.addChild(feature->getName(), &feature->getSettings());
        m_features.push_back(std::move(feature));
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

    // Dispatchers
    void onUpdate();
    void onRender();
    bool onPhotonEvent(Photon::EventData* eventData);
    bool onRaiseEvent(uint8_t code, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions);

    [[nodiscard]] const std::vector<std::unique_ptr<IFeature>>& getFeatures() const { return m_features; }

    [[nodiscard]] std::string getSectionName() const override { return "Features"; }
    SettingsGroup& getSettings() override { return m_group; }

private:
    FeatureManager()           = default;
    ~FeatureManager() override = default;

    std::vector<std::unique_ptr<IFeature>> m_features;
    SettingsGroup m_group;
};
