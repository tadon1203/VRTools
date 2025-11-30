#pragma once

#include "Structs.hpp"

#define IL2CPP_API_FUNCTIONS(X)                                                                                     \
    X(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly* assembly))                              \
    X(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage* image, const char* namespaze, const char* name))    \
    X(const Il2CppType*, il2cpp_class_get_type, (Il2CppClass * klass))                                              \
    X(uint32_t, il2cpp_method_get_param_count, (const MethodInfo* method))                                          \
    X(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount)) \
    X(Il2CppObject*, il2cpp_runtime_invoke,                                                                         \
        (const MethodInfo* method, void* obj, void** params, Il2CppException** exc))                                \
    X(void*, il2cpp_object_unbox, (Il2CppObject * obj))                                                             \
    X(Il2CppDomain*, il2cpp_domain_get, ())                                                                         \
    X(const Il2CppAssembly**, il2cpp_domain_get_assemblies, (const Il2CppDomain* domain, size_t* size))             \
    X(Il2CppThread*, il2cpp_thread_attach, (Il2CppDomain * domain))                                                 \
    X(void, il2cpp_thread_detach, (Il2CppThread * thread))                                                          \
    X(Il2CppObject*, il2cpp_type_get_object, (const Il2CppType* type))                                              \
    X(Il2CppString*, il2cpp_string_new, (const char* str))                                                          \
    X(const MethodInfo*, il2cpp_class_get_methods, (Il2CppClass * klass, void** iter))                              \
    X(const char*, il2cpp_method_get_name, (const MethodInfo* method))                                              \
    X(const Il2CppType*, il2cpp_method_get_param, (const MethodInfo* method, uint32_t index))                       \
    X(char*, il2cpp_type_get_name, (const Il2CppType* type))                                                        \
    X(size_t, il2cpp_image_get_class_count, (const Il2CppImage* image))                                             \
    X(Il2CppClass*, il2cpp_image_get_class, (const Il2CppImage* image, size_t index))                               \
    X(FieldInfo*, il2cpp_class_get_fields, (Il2CppClass * klass, void** iter))                                      \
    X(const char*, il2cpp_field_get_name, (FieldInfo * field))                                                      \
    X(const Il2CppType*, il2cpp_method_get_return_type, (const MethodInfo* method))                                 \
    X(Il2CppClass*, il2cpp_class_from_type, (const Il2CppType* type))

namespace Il2Cpp::Exports {
#define DO_API_DECL(R, N, A) extern R(*N) A;
    IL2CPP_API_FUNCTIONS(DO_API_DECL)
#undef DO_API_DECL
}
