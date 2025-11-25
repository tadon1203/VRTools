#pragma once

#include <filesystem>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <mutex>
#include <vector>

enum class LogLevel { Info, Warn, Error };

using LogCallback = std::function<void(LogLevel, const std::string&)>;

class Logger {
public:
    static Logger& instance();

    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

    void initialize();
    void shutdown();

    void addSink(LogCallback sink);

    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Info, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Warn, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    }

private:
    Logger()  = default;
    ~Logger() = default;

    template <typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) {
        auto now = std::chrono::system_clock::now();
        std::string payload;

        try {
            payload = fmt::format(fmt, std::forward<Args>(args)...);
        } catch (const fmt::format_error& e) {
            payload = std::string("Format error: ") + e.what();
        }

        {
            std::lock_guard lock(m_mutex);
            if (m_file.is_open()) {
                auto now_t = std::chrono::system_clock::to_time_t(now);
                std::tm timeinfo;
                localtime_s(&timeinfo, &now_t);
                m_file << fmt::format("[{:%Y-%m-%d %H:%M:%S}] [{}] {}\n", timeinfo, levelToString(level), payload)
                       << std::flush;
            }
        }

        {
            std::lock_guard lock(m_sinkMutex);
            for (const auto& sink : m_sinks) {
                sink(level, payload);
            }
        }
    }

    [[nodiscard]] std::string_view levelToString(LogLevel level);

    std::mutex m_mutex;
    std::ofstream m_file;

    std::mutex m_sinkMutex;
    std::vector<LogCallback> m_sinks;
};
