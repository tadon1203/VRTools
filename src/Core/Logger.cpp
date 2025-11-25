#include "Logger.hpp"

#include <chrono>

#include <Windows.h>

namespace fs = std::filesystem;

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::initialize() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    fs::path logDir = fs::path(exePath).parent_path() / "HitlerHack";
    fs::create_directories(logDir);

    auto now   = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now_t);

    auto now_str     = fmt::format("{:%Y-%m-%d_%H-%M-%S}", timeinfo);
    fs::path logFile = logDir / (now_str + ".txt");

    std::lock_guard lock(m_mutex);
    m_file.open(logFile, std::ios::out | std::ios::app);
}

void Logger::shutdown() {
    info("Logger shutting down.");
    std::lock_guard lock(m_mutex);
    if (m_file.is_open()) {
        m_file.flush();
        m_file.close();
    }
}

void Logger::addSink(LogCallback sink) {
    std::lock_guard lock(m_sinkMutex);
    m_sinks.push_back(std::move(sink));
}

std::string_view Logger::levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    }
    return "UNKNOWN";
}
