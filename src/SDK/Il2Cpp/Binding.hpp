#pragma once
#include "Il2Cpp.hpp"

#define IL2CPP_BINDING(Asm, Ns, Cls)                                                                       \
public:                                                                                                    \
    static Il2CppClass* getStaticClass() {                                                                 \
        static Il2CppClass* klass = Il2Cpp::findClass(Asm, Ns, Cls);                                       \
        return klass;                                                                                      \
    }                                                                                                      \
    static Il2CppObject* getStaticType() {                                                                 \
        static Il2CppObject* type = nullptr;                                                               \
        if (!type) {                                                                                       \
            auto k = getStaticClass();                                                                     \
            if (k)                                                                                         \
                type = Il2Cpp::Exports::il2cpp_type_get_object(Il2Cpp::Exports::il2cpp_class_get_type(k)); \
        }                                                                                                  \
        return type;                                                                                       \
    }                                                                                                      \
    template <typename R = void, typename... Args>                                                         \
    static R callStatic(const char* name, Args... args) {                                                  \
        static const MethodInfo* m = nullptr;                                                              \
        if (!m)                                                                                            \
            m = Il2Cpp::resolveMethod(getStaticClass(), name, sizeof...(args));                            \
        return Il2Cpp::invokeStatic<R>(m, args...);                                                        \
    }                                                                                                      \
    template <typename R = void, typename... Args>                                                         \
    R call(const char* name, Args... args) {                                                               \
        static const MethodInfo* m = nullptr;                                                              \
        if (!m)                                                                                            \
            m = Il2Cpp::resolveMethod(getStaticClass(), name, sizeof...(args));                            \
        return Il2Cpp::invoke<R>(m, this, args...);                                                        \
    }
