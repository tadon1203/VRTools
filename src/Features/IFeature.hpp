#pragma once

#include <string>
#include <vector>

#include "Core/Events/EventManager.hpp"
#include "Core/Settings/SettingsGroup.hpp"
#include "FeatureCategory.hpp"
#include "UI/NotificationManager.hpp"

class IFeature {
public:
    virtual ~IFeature() { unsubscribeAll(); }

    explicit IFeature(FeatureCategory category, std::string name, int keybind = 0)
        : m_category(category)
        , m_name(std::move(name))
        , m_keybind(keybind) {
        m_settings.add("Enabled", &m_enabled, false);
        m_settings.add("Keybind", &m_keybind, 0);
    }

    virtual void initialize() {}
    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void onMenuRender() {}
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
            unsubscribeAll();
            onDisable();
            NotificationManager::instance().info(m_name, "Disabled");
        }
    }

    void toggle() { setEnabled(!m_enabled); }

protected:
    template <typename EventType>
    void registerEvent(std::function<void(EventType&)> callback) {
        ListenerID id = EventManager::instance().subscribe<EventType>(callback);
        m_listeners.push_back(id);
    }

    bool m_enabled = false;
    SettingsGroup m_settings;

private:
    void unsubscribeAll() {
        auto& em = EventManager::instance();
        for (auto id : m_listeners) {
            em.unsubscribe(id);
        }
        m_listeners.clear();
    }

    const FeatureCategory m_category;
    const std::string m_name;
    int m_keybind;
    std::vector<ListenerID> m_listeners;
};
