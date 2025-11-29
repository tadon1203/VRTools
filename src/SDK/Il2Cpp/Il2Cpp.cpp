#include "Il2Cpp.hpp"

#include <Windows.h>

#include <map>
#include <string_view>
#include <tuple>

using ClassCacheKey = std::tuple<std::string_view, std::string_view, std::string_view>;
static std::map<ClassCacheKey, Il2CppClass*> g_classCache;

namespace Il2Cpp::Exports {
    const Il2CppImage* (*il2cpp_assembly_get_image)(const Il2CppAssembly*)                      = nullptr;
    Il2CppClass* (*il2cpp_class_from_name)(const Il2CppImage*, const char*, const char*)        = nullptr;
    const Il2CppType* (*il2cpp_class_get_type)(Il2CppClass*)                                    = nullptr;
    uint32_t (*il2cpp_method_get_param_count)(const MethodInfo*)                                = nullptr;
    const MethodInfo* (*il2cpp_class_get_method_from_name)(Il2CppClass*, const char*, int)      = nullptr;
    Il2CppObject* (*il2cpp_runtime_invoke)(const MethodInfo*, void*, void**, Il2CppException**) = nullptr;
    Il2CppDomain* (*il2cpp_domain_get)()                                                        = nullptr;
    const Il2CppAssembly** (*il2cpp_domain_get_assemblies)(const Il2CppDomain*, size_t*)        = nullptr;
    Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain*)                                        = nullptr;
    void (*il2cpp_thread_detach)(Il2CppThread*)                                                 = nullptr;
    Il2CppObject* (*il2cpp_type_get_object)(const Il2CppType*)                                  = nullptr;
    Il2CppString* (*il2cpp_string_new)(const char*)                                             = nullptr;
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

    using namespace Exports;
    il2cpp_assembly_get_image =
        reinterpret_cast<decltype(il2cpp_assembly_get_image)>(load("il2cpp_assembly_get_image"));
    il2cpp_class_from_name = reinterpret_cast<decltype(il2cpp_class_from_name)>(load("il2cpp_class_from_name"));
    il2cpp_class_get_type  = reinterpret_cast<decltype(il2cpp_class_get_type)>(load("il2cpp_class_get_type"));
    il2cpp_method_get_param_count =
        reinterpret_cast<decltype(il2cpp_method_get_param_count)>(load("il2cpp_method_get_param_count"));
    il2cpp_class_get_method_from_name =
        reinterpret_cast<decltype(il2cpp_class_get_method_from_name)>(load("il2cpp_class_get_method_from_name"));
    il2cpp_runtime_invoke = reinterpret_cast<decltype(il2cpp_runtime_invoke)>(load("il2cpp_runtime_invoke"));
    il2cpp_domain_get     = reinterpret_cast<decltype(il2cpp_domain_get)>(load("il2cpp_domain_get"));
    il2cpp_domain_get_assemblies =
        reinterpret_cast<decltype(il2cpp_domain_get_assemblies)>(load("il2cpp_domain_get_assemblies"));
    il2cpp_thread_attach   = reinterpret_cast<decltype(il2cpp_thread_attach)>(load("il2cpp_thread_attach"));
    il2cpp_thread_detach   = reinterpret_cast<decltype(il2cpp_thread_detach)>(load("il2cpp_thread_detach"));
    il2cpp_type_get_object = reinterpret_cast<decltype(il2cpp_type_get_object)>(load("il2cpp_type_get_object"));
    il2cpp_string_new      = reinterpret_cast<decltype(il2cpp_string_new)>(load("il2cpp_string_new"));
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

const MethodInfo* Il2Cpp::resolveMethod(
    const char* assemblyName, const char* namespaze, const char* className, const char* methodName, int argsCount) {

    Il2CppClass* klass = findClass(assemblyName, namespaze, className);
    if (!klass) {
        throw std::runtime_error("Could not find class '" + std::string(namespaze) + "." + std::string(className)
                                 + "' in assembly '" + std::string(assemblyName) + "'.");
    }

    const MethodInfo* method = Exports::il2cpp_class_get_method_from_name(klass, methodName, argsCount);
    if (!method) {
        throw std::runtime_error("Failed to resolve method: Could not find method '" + std::string(methodName)
                                 + "' with " + std::to_string(argsCount) + " arguments in class '"
                                 + std::string(namespaze) + "." + std::string(className) + "'.");
    }

    return method;
}

Il2CppObject* Il2Cpp::getSystemType(Il2CppClass* klass) {
    if (!klass) {
        throw std::runtime_error("Il2Cpp::getSystemType: null class");
    }
    const Il2CppType* type = Exports::il2cpp_class_get_type(klass);
    return Exports::il2cpp_type_get_object(type);
}

Il2CppString* Il2Cpp::newString(const char* str) { return Exports::il2cpp_string_new(str); }
