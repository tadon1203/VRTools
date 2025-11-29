#include "Il2Cpp.hpp"

#include <Windows.h>

#include <map>
#include <string_view>
#include <tuple>

using ClassCacheKey = std::tuple<std::string_view, std::string_view, std::string_view>;
static std::map<ClassCacheKey, Il2CppClass*> g_classCache;

namespace Il2Cpp::Exports {
#define DO_API_DEF(R, N, A) R(*N) A = nullptr;
    IL2CPP_API_FUNCTIONS(DO_API_DEF)
#undef DO_API_DEF
}

void Il2Cpp::initialize() {
    HMODULE gameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!gameAssembly) {
        throw std::runtime_error("GameAssembly.dll not found.");
    }

    auto load = [&](const char* name) {
        auto addr = GetProcAddress(gameAssembly, name);
        if (!addr) {
            throw std::runtime_error(std::string("Failed to load Il2Cpp function: ") + name);
        }
        return addr;
    };

#define DO_API_LOAD(R, N, A) Exports::N = reinterpret_cast<R(*) A>(load(#N));
    IL2CPP_API_FUNCTIONS(DO_API_LOAD)
#undef DO_API_LOAD
}

Il2CppClass* Il2Cpp::findClass(const char* assemblyName, const char* namespaze, const char* className) {
    const ClassCacheKey cacheKey{ assemblyName, namespaze, className };
    auto it = g_classCache.find(cacheKey);
    if (it != g_classCache.end()) {
        return it->second;
    }

    auto domain = Exports::il2cpp_domain_get();
    size_t assemblyCount;
    const Il2CppAssembly** assemblies = Exports::il2cpp_domain_get_assemblies(domain, &assemblyCount);

    Il2CppClass* foundClass = nullptr;
    for (size_t i = 0; i < assemblyCount; i++) {
        const Il2CppAssembly* assembly = assemblies[i];
        const Il2CppImage* image       = Exports::il2cpp_assembly_get_image(assembly);
        if (std::string_view(image->name) == assemblyName) {
            foundClass = Exports::il2cpp_class_from_name(image, namespaze, className);
            if (foundClass) {
                break;
            }
        }
    }
    g_classCache[cacheKey] = foundClass;
    return foundClass;
}

const MethodInfo* Il2Cpp::resolveMethod(Il2CppClass* klass, const char* methodName, int argsCount) {
    if (!klass) {
        return nullptr;
    }
    const MethodInfo* method = Exports::il2cpp_class_get_method_from_name(klass, methodName, argsCount);
    if (method) {
        return method;
    }

    void* iter = nullptr;
    while ((method = Exports::il2cpp_class_get_methods(klass, &iter))) {
        const char* mName = Exports::il2cpp_method_get_name(method);
        if (mName && strcmp(mName, methodName) == 0) {
            if (Exports::il2cpp_method_get_param_count(method) == argsCount) {
                return method;
            }
        }
    }
    return nullptr;
}

const MethodInfo* Il2Cpp::resolveMethod(
    const char* asmName, const char* ns, const char* cls, const char* method, int args) {
    return resolveMethod(findClass(asmName, ns, cls), method, args);
}

Il2CppString* Il2Cpp::newString(const char* str) { return Exports::il2cpp_string_new(str); }
