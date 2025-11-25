#pragma once

#include <atomic>

class Core {
public:
    static Core& instance();

    void run();
    void requestShutdown();
    [[nodiscard]] bool isRunning() const;

private:
    Core()  = default;
    ~Core() = default;

    void initialize();
    void shutdown();

    std::atomic<bool> m_isRunning = false;
};
