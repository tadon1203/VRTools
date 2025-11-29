#include "CrashHandler.hpp"

#include <DbgHelp.h>
#include <Windows.h>

#include <filesystem>
#include <fstream>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include "Logger.hpp"
#include "Version.hpp"

namespace fs = std::filesystem;

CrashHandler& CrashHandler::instance() {
    static CrashHandler inst;
    return inst;
}

void CrashHandler::initialize() {
    // Register our exception handler
    SetUnhandledExceptionFilter(unhandledExceptionHandler);
    Logger::instance().info("CrashHandler initialized (SetUnhandledExceptionFilter).");
}

const char* CrashHandler::getExceptionCodeString(DWORD code) {
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        return "ACCESS_VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_BREAKPOINT:
        return "BREAKPOINT";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "DATATYPE_MISALIGNMENT";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "FLT_DENORMAL_OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "FLT_DIVIDE_BY_ZERO";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "FLT_INEXACT_RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "FLT_INVALID_OPERATION";
    case EXCEPTION_FLT_OVERFLOW:
        return "FLT_OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:
        return "FLT_STACK_CHECK";
    case EXCEPTION_FLT_UNDERFLOW:
        return "FLT_UNDERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "ILLEGAL_INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:
        return "IN_PAGE_ERROR";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "INT_DIVIDE_BY_ZERO";
    case EXCEPTION_INT_OVERFLOW:
        return "INT_OVERFLOW";
    case EXCEPTION_INVALID_DISPOSITION:
        return "INVALID_DISPOSITION";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "NONCONTINUABLE_EXCEPTION";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "PRIV_INSTRUCTION";
    case EXCEPTION_SINGLE_STEP:
        return "SINGLE_STEP";
    case EXCEPTION_STACK_OVERFLOW:
        return "STACK_OVERFLOW";
    default:
        return "UNKNOWN_EXCEPTION";
    }
}

void CrashHandler::generateStackTrace(PCONTEXT context, std::string& report) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread  = GetCurrentThread();

    // Initialize DbgHelp
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    if (!SymInitialize(process, nullptr, TRUE)) {
        report += "Failed to initialize symbol handler.\n";
        return;
    }

    STACKFRAME64 stackFrame     = {};
    stackFrame.AddrPC.Mode      = AddrModeFlat;
    stackFrame.AddrPC.Offset    = context->Rip;
    stackFrame.AddrStack.Mode   = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrFrame.Mode   = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;

    report += "\nStack Trace:\n";

    int frameCount = 0;
    while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &stackFrame, context, nullptr,
        SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {

        if (stackFrame.AddrPC.Offset == 0) {
            break;
        }
        if (frameCount++ > 50) {
            break; // Limit recursion
        }

        // Resolve Symbol
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol  = (PSYMBOL_INFO) buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen   = MAX_SYM_NAME;

        DWORD64 displacement     = 0;
        std::string functionName = "<Unknown Function>";

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, pSymbol)) {
            functionName = pSymbol->Name;
        }

        // Resolve Line Number
        IMAGEHLP_LINE64 lineInfo = {};
        lineInfo.SizeOfStruct    = sizeof(IMAGEHLP_LINE64);
        DWORD displacementLine   = 0;
        std::string lineStr      = "";

        if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &displacementLine, &lineInfo)) {
            lineStr = fmt::format(" [{}:{}]", fs::path(lineInfo.FileName).filename().string(), lineInfo.LineNumber);
        }

        report +=
            fmt::format("{:02} | 0x{:016X} | {}{}\n", frameCount - 1, stackFrame.AddrPC.Offset, functionName, lineStr);
    }

    SymCleanup(process);
}

LONG WINAPI CrashHandler::unhandledExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) {
    static bool hasCrashed = false;
    if (hasCrashed) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    hasCrashed = true;

    auto now   = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now_t);

    auto timestamp = fmt::format("{:%Y-%m-%d_%H-%M-%S}", timeinfo);

    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    fs::path crashDir = fs::path(exePath).parent_path() / Version::PROJECT_NAME / "Crashes";
    fs::create_directories(crashDir);

    fs::path crashFile = crashDir / ("crash_" + timestamp + ".txt");

    std::string report;
    report += fmt::format("VRTools Crash Report\n");
    report += fmt::format("Timestamp: {}\n", timestamp);
    report += fmt::format("Version: {}\n", Version::PROJECT_VERSION);
    report += fmt::format("Exception Code: 0x{:08X} ({})\n", pExceptionInfo->ExceptionRecord->ExceptionCode,
        getExceptionCodeString(pExceptionInfo->ExceptionRecord->ExceptionCode));
    report +=
        fmt::format("Exception Address: 0x{:016X}\n", (uintptr_t) pExceptionInfo->ExceptionRecord->ExceptionAddress);

    if (pExceptionInfo->ContextRecord) {
        auto& ctx = *pExceptionInfo->ContextRecord;
        report += fmt::format("\nRegisters:\n");
        report += fmt::format("RAX: 0x{:016X}  RBX: 0x{:016X}  RCX: 0x{:016X}\n", ctx.Rax, ctx.Rbx, ctx.Rcx);
        report += fmt::format("RDX: 0x{:016X}  RSI: 0x{:016X}  RDI: 0x{:016X}\n", ctx.Rdx, ctx.Rsi, ctx.Rdi);
        report += fmt::format("R8:  0x{:016X}  R9:  0x{:016X}  R10: 0x{:016X}\n", ctx.R8, ctx.R9, ctx.R10);
        report += fmt::format("R11: 0x{:016X}  R12: 0x{:016X}  R13: 0x{:016X}\n", ctx.R11, ctx.R12, ctx.R13);
        report += fmt::format("RIP: 0x{:016X}  RSP: 0x{:016X}  RBP: 0x{:016X}\n", ctx.Rip, ctx.Rsp, ctx.Rbp);

        generateStackTrace(pExceptionInfo->ContextRecord, report);
    } else {
        report += "\nNo ContextRecord available for register/stack dump.\n";
    }

    std::ofstream file(crashFile);
    if (file.is_open()) {
        file << report;
        file.close();
    }

    Logger::instance().error("CRASH DETECTED! See {} for details.", crashFile.string());
    Logger::instance().shutdown();

    std::string msg = fmt::format("VRTools has crashed!\n\nException: {}\n\nA log has been saved to:\n{}",
        getExceptionCodeString(pExceptionInfo->ExceptionRecord->ExceptionCode), crashFile.string());

    MessageBoxA(nullptr, msg.c_str(), "VRTools Crash Handler", MB_OK | MB_ICONERROR | MB_TOPMOST);

    TerminateProcess(GetCurrentProcess(), pExceptionInfo->ExceptionRecord->ExceptionCode);

    return EXCEPTION_EXECUTE_HANDLER;
}
