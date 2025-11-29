#pragma once

#include <stdexcept>
#include <type_traits>

#include "Functions.hpp"
#include "Structs.hpp"

namespace Il2Cpp {
    void initialize();

    Il2CppClass* findClass(const char* assemblyName, const char* namespaze, const char* className);

    const MethodInfo* resolveMethod(Il2CppClass* klass, const char* methodName, int argsCount);
    const MethodInfo* resolveMethod(const char* asmName, const char* ns, const char* cls, const char* method, int args);

    Il2CppString* newString(const char* str);

    namespace detail {
        template <typename R, typename... Args>
        R invokeImpl(const MethodInfo* method, void* instance, bool isStatic, Args... args) {
            if (!method) {
                throw std::runtime_error("Method is null");
            }

            if (method->methodPointer) {
                if (isStatic) {
                    using MethodFn = R (*)(Args...);
                    return reinterpret_cast<MethodFn>(method->methodPointer)(args...);
                }
                using MethodFn = R (*)(void*, Args...);
                return reinterpret_cast<MethodFn>(method->methodPointer)(instance, args...);
            }

            void* params[] = { const_cast<void*>(reinterpret_cast<const void*>(&args))..., nullptr };

            Il2CppException* exc = nullptr;
            Il2CppObject* res    = Exports::il2cpp_runtime_invoke(method, instance, params, &exc);

            if (exc) {
                return R{};
            }

            if constexpr (std::is_same_v<R, void>) {
                return;
            } else if constexpr (std::is_pointer_v<R>) {
                return reinterpret_cast<R>(res);
            } else {
                if (!res) {
                    return R{};
                }
                return *static_cast<R*>(Exports::il2cpp_object_unbox(res));
            }
        }
    }

    template <typename R, typename... Args>
    R invoke(const MethodInfo* method, void* instance, Args... args) {
        return detail::invokeImpl<R>(method, instance, false, args...);
    }

    template <typename R, typename... Args>
    R invokeStatic(const MethodInfo* method, Args... args) {
        return detail::invokeImpl<R>(method, nullptr, true, args...);
    }
}
