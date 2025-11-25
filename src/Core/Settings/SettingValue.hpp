#pragma once

#include <string>
#include <variant>
#include <vector>

using SettingValue = std::variant<bool, int, float>;

struct EnumMetadata {
    std::vector<std::string> items;
};
