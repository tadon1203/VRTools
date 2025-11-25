#include <iostream>

#include <Windows.h>

#include "Core/Core.hpp"
#include "Core/Logger.hpp"

DWORD WINAPI threadProc(LPVOID lpParameter) {
    Logger::instance().initialize();

    Core::instance().run();

    Logger::instance().shutdown();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);

        HANDLE hThread = CreateThread(nullptr, 0, &threadProc, hinstDLL, 0, nullptr);
        if (hThread) {
            CloseHandle(hThread);
        }
    }
    if (fdwReason == DLL_PROCESS_DETACH) {
        if (Core::instance().isRunning()) {
            Core::instance().requestShutdown();
        }
    }
    return TRUE;
}
