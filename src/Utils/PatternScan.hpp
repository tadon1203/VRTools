#pragma once

#include <Psapi.h>
#include <vector>

#include <Windows.h>

namespace Utils {
    inline uintptr_t findPattern(const char* moduleName, const char* pattern) {
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule) {
            return 0;
        }

        MODULEINFO moduleInfo;
        if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO))) {
            return 0;
        }

        auto base      = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
        uintptr_t size = moduleInfo.SizeOfImage;

        std::vector<int> patternBytes;

        for (const char* p = pattern; *p;) {
            if (*p == ' ') {
                ++p;
                continue;
            }
            if (*p == '?') {
                patternBytes.push_back(-1);
                ++p;
                if (*p == '?') {
                    ++p;
                }
            } else {
                patternBytes.push_back(strtol(p, const_cast<char**>(&p), 16));
            }
        }

        for (uintptr_t i = 0; i < size - patternBytes.size(); ++i) {
            bool found = true;
            for (size_t j = 0; j < patternBytes.size(); ++j) {
                if (patternBytes[j] != -1 && *reinterpret_cast<uint8_t*>(base + i + j) != patternBytes[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return base + i;
            }
        }

        return 0;
    }
}
