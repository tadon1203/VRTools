#pragma once

#include <deque>
#include <mutex>

#include "../HUDComponent.hpp"
#include "Core/Logger.hpp"

struct LogEntry {
    std::string text;
    ImColor color;
    float timeAlive;
};

class LogComponent : public HUDComponent {
public:
    LogComponent();
    void initialize() override;

    void onRenderContent(ImDrawList* dl) override;
    ImVec2 getSize() override;

private:
    void addLog(LogLevel level, const std::string& msg);

    std::deque<LogEntry> m_logs;
    std::mutex m_mutex;
    float m_lifetime = 10.0f;
    ImVec2 m_cachedSize{ 200, 100 };
};
