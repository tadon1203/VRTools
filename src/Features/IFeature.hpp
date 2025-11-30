#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Core/Events/EventManager.hpp"
#include "FeatureCategory.hpp"
#include "UI/NotificationManager.hpp"

class IFeature {
public:
    virtual ~IFeature() { unsubscribeAll(); }

    explicit IFeature(FeatureCategory category, std::string name, std::optional<int> keybind = std::nullopt)
        : m_category(category)
        , m_name(std::move(name))
        , m_keybind(keybind) {}

    virtual void initialize() {}

    virtual void onEnable() {}
    virtual void onDisable() {}

    virtual void onMenuRender() {}
    virtual void onKeybind() { toggle(); }

    virtual void onLoadConfig(const nlohmann::json& j) {
        if (j.contains("Enabled")) {
            bool shouldEnable = j["Enabled"];
            if (shouldEnable != m_enabled) {
                setEnabled(shouldEnable);
            }
        }
    }

    virtual void onSaveConfig(nlohmann::json& j) const { j["Enabled"] = m_enabled; }
    [[nodiscard]] virtual bool shouldShowInMenu() const { return true; }

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
    const std::optional<int> m_keybind;
    std::vector<ListenerID> m_listeners;
};
