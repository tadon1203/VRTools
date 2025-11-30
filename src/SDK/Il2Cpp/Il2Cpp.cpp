#include "Il2Cpp.hpp"

#include <Windows.h>

#include <map>
#include <string_view>
#include <tuple>

#include "Core/Logger.hpp"

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
        throw std::runtime_error("Critical Error: GameAssembly.dll not found in process.");
    }

    auto load = [&](const char* name) {
        auto addr = GetProcAddress(gameAssembly, name);
        if (!addr) {
            throw std::runtime_error(fmt::format("Il2Cpp Import Error: Failed to load function '{}'", name));
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

    if (!foundClass) {
        throw std::runtime_error(fmt::format(
            "Il2Cpp Error: Failed to find class '{}::{}' in assembly '{}'", namespaze, className, assemblyName));
    }

    g_classCache[cacheKey] = foundClass;
    return foundClass;
}

Il2CppClass* Il2Cpp::findClassByField(const char* assemblyName, const char* fieldName) {
    auto domain = Exports::il2cpp_domain_get();
    size_t assemblyCount;
    const Il2CppAssembly** assemblies = Exports::il2cpp_domain_get_assemblies(domain, &assemblyCount);

    for (size_t i = 0; i < assemblyCount; i++) {
        const Il2CppAssembly* assembly = assemblies[i];
        const Il2CppImage* image       = Exports::il2cpp_assembly_get_image(assembly);

        std::string_view imgName(image->name);
        if (imgName.find(assemblyName) == std::string_view::npos) {
            continue;
        }

        size_t classCount = Exports::il2cpp_image_get_class_count(image);
        for (size_t c = 0; c < classCount; c++) {
            Il2CppClass* klass = Exports::il2cpp_image_get_class(image, c);
            if (!klass) {
                continue;
            }

            void* iter = nullptr;
            while (FieldInfo* field = Exports::il2cpp_class_get_fields(klass, &iter)) {
                const char* fName = Exports::il2cpp_field_get_name(field);
                if (fName && std::string_view(fName) == fieldName) {
                    return klass;
                }
            }
        }
    }

    throw std::runtime_error(
        fmt::format("Il2Cpp Error: Failed to find class by field '{}' in assembly '{}'", fieldName, assemblyName));
}

const MethodInfo* Il2Cpp::resolveMethod(Il2CppClass* klass, const char* methodName, int argsCount) {
    if (!klass) {
        // Should catch logic errors where a previous findClass might have been bypassed (unlikely with new exceptions)
        throw std::runtime_error(
            fmt::format("Il2Cpp Error: Attempted to resolve method '{}' on a nullptr class.", methodName));
    }

    const MethodInfo* method = Exports::il2cpp_class_get_method_from_name(klass, methodName, argsCount);
    if (method) {
        return method;
    }

    // Fallback search
    void* iter = nullptr;
    while ((method = Exports::il2cpp_class_get_methods(klass, &iter))) {
        const char* mName = Exports::il2cpp_method_get_name(method);
        if (mName && strcmp(mName, methodName) == 0) {
            if (Exports::il2cpp_method_get_param_count(method) == argsCount) {
                return method;
            }
        }
    }

    throw std::runtime_error(
        fmt::format("Il2Cpp Error: Method '{}' (args: {}) not found in class '{}'", methodName, argsCount,
            Exports::il2cpp_class_get_name(klass) ? Exports::il2cpp_type_get_name(Exports::il2cpp_class_get_type(klass))
                                                  : "Unknown"));
}

const MethodInfo* Il2Cpp::resolveMethod(
    const char* asmName, const char* ns, const char* cls, const char* method, int args) {
    return resolveMethod(findClass(asmName, ns, cls), method, args);
}

const MethodInfo* Il2Cpp::resolveMethodByReturnType(Il2CppClass* klass, Il2CppClass* returnType, int argsCount) {
    if (!klass || !returnType) {
        throw std::runtime_error("Il2Cpp Error: resolveMethodByReturnType called with nullptr class or returnType.");
    }

    const Il2CppType* targetType = Exports::il2cpp_class_get_type(returnType);
    if (!targetType) {
        throw std::runtime_error("Il2Cpp Error: Failed to get type from returnType class.");
    }

    void* iter = nullptr;
    while (const MethodInfo* method = Exports::il2cpp_class_get_methods(klass, &iter)) {
        if (Exports::il2cpp_method_get_param_count(method) != static_cast<uint32_t>(argsCount)) {
            continue;
        }

        const Il2CppType* retType = Exports::il2cpp_method_get_return_type(method);
        if (!retType) {
            continue;
        }

        Il2CppClass* retClass = Exports::il2cpp_class_from_type(retType);
        if (retClass == returnType) {
            return method;
        }
    }

    throw std::runtime_error("Il2Cpp Error: Failed to find method by return type matching.");
}

Il2CppString* Il2Cpp::newString(const char* str) {
    auto* s = Exports::il2cpp_string_new(str);
    if (!s) {
        throw std::runtime_error("Il2Cpp Error: Failed to create new string.");
    }
    return s;
}
