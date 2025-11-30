#pragma once

#include <string>

#include "SettingsGroup.hpp"

class ISettingsHandler {
public:
    virtual ~ISettingsHandler() = default;

    [[nodiscard]] virtual std::string getSectionName() const = 0;

    virtual SettingsGroup& getSettings() = 0;
};
