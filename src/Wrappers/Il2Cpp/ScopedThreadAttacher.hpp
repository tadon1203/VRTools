#pragma once

#include "Il2Cpp.hpp"

namespace Il2Cpp {
    class ScopedThreadAttacher {
    public:
        ScopedThreadAttacher() {
            m_thread = il2cpp_thread_attach(il2cpp_domain_get());
            if (!m_thread) {
                throw std::runtime_error("Failed to attach IL2CPP thread.");
            }
        }

        ~ScopedThreadAttacher() {
            if (m_thread) {
                il2cpp_thread_detach(m_thread);
            }
        }

        ScopedThreadAttacher(const ScopedThreadAttacher&)            = delete;
        ScopedThreadAttacher& operator=(const ScopedThreadAttacher&) = delete;
        ScopedThreadAttacher(ScopedThreadAttacher&&)                 = delete;
        ScopedThreadAttacher& operator=(ScopedThreadAttacher&&)      = delete;

    private:
        Il2CppThread* m_thread = nullptr;
    };
}
