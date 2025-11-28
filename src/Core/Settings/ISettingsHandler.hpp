#pragma once

#include <string>

#include <nlohmann/json.hpp>

class ISettingsHandler {
public:
    virtual ~ISettingsHandler() = default;

    [[nodiscard]] virtual std::string getSectionName() const = 0;

    [[nodiscard]] virtual nlohmann::json onSaveConfig() const = 0;

    virtual void onLoadConfig(const nlohmann::json& section) = 0;
};
