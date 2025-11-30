#pragma once

#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Utils/JsonConverters.hpp"

class ISettingEntry {
public:
    virtual ~ISettingEntry()                   = default;
    virtual void save(nlohmann::json& j) const = 0;
    virtual void load(const nlohmann::json& j) = 0;
};

template <typename T>
class SettingEntry : public ISettingEntry {
public:
    SettingEntry(std::string name, T* ptr, T defaultValue)
        : m_name(std::move(name))
        , m_ptr(ptr)
        , m_default(defaultValue) {
        *m_ptr = m_default;
    }

    void save(nlohmann::json& j) const override { j[m_name] = *m_ptr; }

    void load(const nlohmann::json& j) override {
        if (j.contains(m_name)) {
            try {
                *m_ptr = j[m_name].get<T>();
            } catch (...) {
            }
        }
    }

private:
    std::string m_name;
    T* m_ptr;
    T m_default;
};

class SettingsGroup;

class GroupEntry : public ISettingEntry {
public:
    GroupEntry(std::string name, SettingsGroup* group);
    void save(nlohmann::json& j) const override;
    void load(const nlohmann::json& j) override;

private:
    std::string m_name;
    SettingsGroup* m_group;
};

class SettingsGroup {
public:
    template <typename T>
    void add(const std::string& name, T* ptr, T defaultValue = T()) {
        m_entries.push_back(std::make_unique<SettingEntry<T>>(name, ptr, defaultValue));
    }

    void addChild(const std::string& name, SettingsGroup* child) {
        m_entries.push_back(std::make_unique<GroupEntry>(name, child));
    }

    void save(nlohmann::json& j) const {
        for (const auto& entry : m_entries) {
            entry->save(j);
        }
    }

    void load(const nlohmann::json& j) {
        for (const auto& entry : m_entries) {
            entry->load(j);
        }
    }

private:
    std::vector<std::unique_ptr<ISettingEntry>> m_entries;
};

inline GroupEntry::GroupEntry(std::string name, SettingsGroup* group)
    : m_name(std::move(name))
    , m_group(group) {}

inline void GroupEntry::save(nlohmann::json& j) const {
    nlohmann::json childJ;
    m_group->save(childJ);
    j[m_name] = childJ;
}

inline void GroupEntry::load(const nlohmann::json& j) {
    if (j.contains(m_name)) {
        m_group->load(j[m_name]);
    }
}
