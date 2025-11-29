#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "Structs.hpp"

namespace Il2Cpp {
    void initialize();

    Il2CppClass* findClass(const char* assemblyName, const char* namespaze, const char* className);

    const MethodInfo* resolveMethod(const char* assemblyName, const char* namespaze, const char* className,
        const char* methodName, int argsCount, const char* firstParamType = nullptr);

    Il2CppObject* getSystemType(Il2CppClass* klass);

    Il2CppString* newString(const char* str);

    namespace detail {
        template <typename R, typename... Args>
        R invokeImpl(const MethodInfo* method, Il2CppObject* instance, bool isStatic, Args... args) {
            if (!method || !method->methodPointer) {
                throw std::runtime_error("MethodInfo or method pointer is null.");
            }

            if (isStatic) {
                using MethodFn = R (*)(Args...);
                auto fn        = reinterpret_cast<MethodFn>(method->methodPointer);
                return fn(args...);
            }

            if (!instance) {
                throw std::runtime_error("Instance is null for a non-static method call.");
            }
            using MethodFn = R (*)(Il2CppObject*, Args...);
            auto fn        = reinterpret_cast<MethodFn>(method->methodPointer);
            return fn(instance, args...);
        }
    }

    template <typename R, typename T, typename... Args>
    R invoke(
        const T* instance, const char* assembly, const char* ns, const char* klass, const char* method, Args... args) {
        static_assert(std::is_base_of_v<Il2CppObject, T>, "Template argument T must be derived from Il2CppObject.");

        const MethodInfo* methodInfo = resolveMethod(assembly, ns, klass, method, sizeof...(args));

        if (!instance) {
            throw std::runtime_error("Attempted to call method '" + std::string(method) + "' on a null instance.");
        }

        auto instancePtr = const_cast<Il2CppObject*>(static_cast<const Il2CppObject*>(instance));
        return detail::invokeImpl<R>(methodInfo, instancePtr, false, args...);
    }

    template <typename R, typename... Args>
    R invoke(const MethodInfo* method, void* instance, Args... args) {
        return detail::invokeImpl<R>(method, static_cast<Il2CppObject*>(instance), false, args...);
    }

    template <typename R, typename... Args>
    R invoke(const char* assembly, const char* ns, const char* klass, const char* method, Args... args) {
        const MethodInfo* methodInfo = resolveMethod(assembly, ns, klass, method, sizeof...(args));
        return detail::invokeImpl<R>(methodInfo, nullptr, true, args...);
    }
}
