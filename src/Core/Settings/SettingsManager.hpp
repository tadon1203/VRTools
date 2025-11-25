#pragma once

#include <string>

#include <nlohmann/json.hpp>

class SettingsManager {
public:
    static SettingsManager& instance();

    nlohmann::json loadFromFile(const std::string& filename = "settings.json");
    void saveToFile(const nlohmann::json& root, const std::string& filename = "settings.json");

private:
    SettingsManager()  = default;
    ~SettingsManager() = default;
};
