#include "LogWindow.hpp"

#include <ctime>
#include <imgui.h>

LogWindow::LogWindow()
    : IFeature(FeatureCategory::System, "Log Window") {
    m_enabled = true;
}

void LogWindow::initialize() {
    Logger::instance().addSink([this](LogLevel l, const std::string& m) { this->onLogCallback(l, m); });
}

void LogWindow::onLogCallback(LogLevel level, const std::string& message) {
    auto now   = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now_t);

    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);

    std::string uiMsg = std::string("[") + buf + "] " + message;

    if (m_logs.size() >= m_maxLogs) {
        m_logs.pop_front();
    }
    m_logs.push_back({ level, std::move(uiMsg) });
}

void LogWindow::onLoadConfig(const nlohmann::json& j) {
    IFeature::onLoadConfig(j);
    if (j.contains("AutoScroll")) {
        m_autoScroll = j["AutoScroll"];
    }
}

void LogWindow::onSaveConfig(nlohmann::json& j) const {
    IFeature::onSaveConfig(j);
    j["AutoScroll"] = m_autoScroll;
}

void LogWindow::onMenuRender() {
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically scroll to the bottom when new logs arrive.");
    }
    if (ImGui::Button("Clear Logs")) {
        m_logs.clear();
    }
}

void LogWindow::onRender() {
    if (!m_enabled) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(400, 200);
    float padding = 10.0f;
    ImVec2 windowPos(padding, io.DisplaySize.y - windowSize.y - padding);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.7f);

    ImGui::Begin("Log", &m_enabled, ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Clear")) {
        m_logs.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    ImGui::Separator();

    ImGui::BeginChild("scrolling");

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(m_logs.size()));
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const auto& log = m_logs[i];
            ImVec4 color{};

            if (log.level == LogLevel::Warn) {
                color = { 1, 1, 0.4f, 1 };
            } else if (log.level == LogLevel::Error) {
                color = { 1, 0.4f, 0.4f, 1 };
            } else {
                color = { 0.8f, 0.8f, 0.8f, 1 };
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(log.message.c_str());
            ImGui::PopStyleColor();
        }
    }
    clipper.End();

    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
