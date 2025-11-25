#pragma once

#include "Structs.hpp"

namespace Il2Cpp::Exports {
    extern const Il2CppImage* (*il2cpp_assembly_get_image)(const Il2CppAssembly* assembly);
    extern Il2CppClass* (*il2cpp_class_from_name)(const Il2CppImage* image, const char* namespaze, const char* name);
    extern const Il2CppType* (*il2cpp_class_get_type)(Il2CppClass* klass);
    extern uint32_t (*il2cpp_method_get_param_count)(const MethodInfo* method);
    extern const MethodInfo* (*il2cpp_class_get_method_from_name)(Il2CppClass* klass, const char* name, int argsCount);
    extern Il2CppObject* (*il2cpp_runtime_invoke)(
        const MethodInfo* method, void* obj, void** params, Il2CppException** exc);
    extern Il2CppDomain* (*il2cpp_domain_get)();
    extern const Il2CppAssembly** (*il2cpp_domain_get_assemblies)(const Il2CppDomain* domain, size_t* size);
    extern Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain* domain);
    extern void (*il2cpp_thread_detach)(Il2CppThread* thread);
    extern Il2CppObject* (*il2cpp_type_get_object)(const Il2CppType* type);
}
