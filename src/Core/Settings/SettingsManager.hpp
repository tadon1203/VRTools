#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "ISettingsHandler.hpp"

class SettingsManager {
public:
    static SettingsManager& instance();

    void registerHandler(ISettingsHandler* handler);

    void saveAll();

    void loadAll();

private:
    SettingsManager()  = default;
    ~SettingsManager() = default;

    // I/O helpers
    nlohmann::json loadFromFile();
    void saveToFile(const nlohmann::json& root);

    std::vector<ISettingsHandler*> m_handlers;
    const std::string m_filename = "settings.json";
};
