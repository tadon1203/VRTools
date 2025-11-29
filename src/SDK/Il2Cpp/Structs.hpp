#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Il2CppAssembly {};
struct Il2CppClass {};
struct Il2CppDomain {};
struct Il2CppThread {};
struct Il2CppType {};

struct MethodInfo {
    void* methodPointer;
};

struct Il2CppObject {
    Il2CppClass* klass;
    void* monitor;
};

struct Il2CppImage {
    const char* name;
};

struct Il2CppString : Il2CppObject {
    int32_t length;
    wchar_t chars[1];

    [[nodiscard]] std::string toString() const;
};

struct Il2CppArrayBounds {
    uintptr_t length;
    uint32_t lowerBound;
};

template <typename T = void*>
struct Il2CppArray : Il2CppObject {
    Il2CppArrayBounds* bounds;
    uintptr_t maxLength;
    alignas(8) T data[0];

    [[nodiscard]] std::vector<T> toVector() const {
        if (!this) {
            return {};
        }
        return std::vector<T>(data, data + maxLength);
    }
};

template <typename T>
struct Il2CppList : Il2CppObject {
    Il2CppArray<T>* array;

    [[nodiscard]] std::vector<T> toVector() const {
        if (!this || !array) {
            return {};
        }
        return array->toVector();
    }
};

struct Il2CppException {
    Il2CppString* className;
    Il2CppString* message;
    Il2CppObject* data;
    Il2CppException* innerEx;
    Il2CppString* helpUrl;
    Il2CppArray<>* traceIps;
    Il2CppString* stackTrace;
};
