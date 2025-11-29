#pragma once

#include <Windows.h>

#include <string>

class CrashHandler {
public:
    static CrashHandler& instance();

    void initialize();

private:
    CrashHandler()  = default;
    ~CrashHandler() = default;

    static LONG WINAPI unhandledExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo);

    static void generateStackTrace(PCONTEXT context, std::string& outReport);
    static const char* getExceptionCodeString(DWORD code);
};
