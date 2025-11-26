#pragma once

#include <string>
#include <string_view>

namespace Version {
    constexpr std::string_view PROJECT_NAME    = "VRTools";
    constexpr std::string_view PROJECT_VERSION = "1.0.0";

#ifdef _DEBUG
    constexpr bool IS_DEBUG = true;
#else
    constexpr bool IS_DEBUG = false;
#endif
}
