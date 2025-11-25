#pragma once

#include <deque>

#include "Core/Logger.hpp"
#include "IFeature.hpp"

struct UILogMessage {
    LogLevel level;
    std::string message;
};

class LogWindow final : public IFeature {
public:
    LogWindow();

    void initialize() override;
    void onRender() override;
    void onMenuRender() override;

    void onLoadConfig(const nlohmann::json& j) override;
    void onSaveConfig(nlohmann::json& j) const override;

private:
    void onLogCallback(LogLevel level, const std::string& message);

    bool m_autoScroll = true;
    std::deque<UILogMessage> m_logs;
    const size_t m_maxLogs = 500;
};
