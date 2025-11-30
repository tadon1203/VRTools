#pragma once

#include <string>

#include "Core/Settings/SettingsGroup.hpp"
#include "FeatureCategory.hpp"
#include "SDK/Game/Photon/SendOptions.hpp"
#include "UI/NotificationManager.hpp"

namespace Photon {
    struct EventData;
    struct SendOptions;
}
struct Il2CppObject;

class IFeature {
public:
    virtual ~IFeature() = default;

    explicit IFeature(FeatureCategory category, std::string name, int keybind = 0)
        : m_category(category)
        , m_name(std::move(name))
        , m_keybind(keybind) {
        m_settings.add("Enabled", &m_enabled, false);
        m_settings.add("Keybind", &m_keybind, 0);
    }

    virtual void initialize() {}

    // Lifecycle
    virtual void onEnable() {}
    virtual void onDisable() {}

    // Loop
    virtual void onUpdate() {}
    virtual void onRender() {}
    virtual void onMenuRender() {}

    // Network
    virtual bool onPhotonEvent(Photon::EventData* eventData) { return true; }
    virtual bool onRaiseEvent(
        uint8_t code, Il2CppObject* content, void* raiseEventOptions, Photon::SendOptions sendOptions) {
        return true;
    }

    virtual void onKeybind() { toggle(); }

    [[nodiscard]] virtual bool shouldShowInMenu() const { return true; }
    [[nodiscard]] bool isEnabled() const { return m_enabled; }
    [[nodiscard]] FeatureCategory getCategory() const { return m_category; }
    [[nodiscard]] const std::string& getName() const { return m_name; }

    [[nodiscard]] int getKeybind() const { return m_keybind; }

    SettingsGroup& getSettings() { return m_settings; }

    void setEnabled(bool enabled) {
        if (m_enabled == enabled) {
            return;
        }
        m_enabled = enabled;
        if (m_enabled) {
            onEnable();
            NotificationManager::instance().success(m_name, "Enabled");
        } else {
            onDisable();
            NotificationManager::instance().info(m_name, "Disabled");
        }
    }

    void toggle() { setEnabled(!m_enabled); }

protected:
    bool m_enabled = false;
    SettingsGroup m_settings;

private:
    const FeatureCategory m_category;
    const std::string m_name;
    int m_keybind;
};
