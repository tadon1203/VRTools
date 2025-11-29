#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "FeatureCategory.hpp"
#include "SDK/Game/Photon/EventData.hpp"
#include "SDK/Game/Photon/SendOptions.hpp"
#include "SDK/Game/PlayerManager.hpp"
#include "UI/NotificationManager.hpp"

class IFeature {
public:
    virtual ~IFeature() = default;

    explicit IFeature(FeatureCategory category, std::string name, std::optional<int> keybind = std::nullopt)
        : m_category(category)
        , m_name(std::move(name))
        , m_keybind(keybind) {}

    virtual void initialize() {}
    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void onUpdate() {}
    virtual void onKeybind() { toggle(); }

    virtual void onRender() {}
    virtual void onMenuRender() {}

    virtual bool onEvent(Photon::EventData* eventData) { return true; }
    virtual bool onRaiseEvent(
        uint8_t eventCode, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {
        return true;
    }

    virtual void onLoadConfig(const nlohmann::json& j) {
        if (j.contains("Enabled")) {
            m_enabled = j["Enabled"];
        }
    }
    virtual void onSaveConfig(nlohmann::json& j) const { j["Enabled"] = m_enabled; }

    virtual bool shouldShowInMenu() const { return true; }

    [[nodiscard]] bool isEnabled() const { return m_enabled; }
    [[nodiscard]] FeatureCategory getCategory() const { return m_category; }
    [[nodiscard]] const std::string& getName() const { return m_name; }
    [[nodiscard]] std::optional<int> getKeybind() const { return m_keybind; }

    void setEnabled(bool enabled) {
        if (m_enabled == enabled) {
            return;
        }
        m_enabled = enabled;
        if (m_enabled) {
            onEnable();
        } else {
            onDisable();
        }

        if (m_enabled) {
            NotificationManager::instance().success(m_name, "Enabled");
        } else {
            NotificationManager::instance().info(m_name, "Disabled");
        }
    }

    void toggle() { setEnabled(!m_enabled); }

protected:
    bool m_enabled = false;

private:
    const FeatureCategory m_category;
    const std::string m_name;
    const std::optional<int> m_keybind;
};
