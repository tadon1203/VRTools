#include "LogComponent.hpp"

#include <algorithm>

#include "Features/HUD/HUDUtils.hpp"

LogComponent::LogComponent()
    : IHUDComponent("LogOverlay", ImVec2(20, 300)) {
    m_enabled = true;
}

void LogComponent::initialize() {
    Logger::instance().addSink([this](LogLevel l, const std::string& m) { this->addLog(l, m); });
}

void LogComponent::addLog(LogLevel level, const std::string& msg) {
    ImColor col = ImColor(255, 255, 255);
    switch (level) {
    case LogLevel::Warn:
        col = ImColor(255, 255, 0);
        break;
    case LogLevel::Error:
        col = ImColor(255, 100, 100);
        break;
    }

    std::lock_guard lock(m_mutex);
    m_logs.push_back({ msg, col, m_lifetime });
    if (m_logs.size() > 50) {
        m_logs.pop_front();
    }
}

void LogComponent::onRenderContent(ImDrawList* dl) {
    std::lock_guard lock(m_mutex);
    float dt = ImGui::GetIO().DeltaTime;

    float yOffset    = 0.0f;
    float maxWidth   = 100.0f;
    float lineHeight = ImGui::GetFontSize() + 2.0f;

    auto it = m_logs.begin();
    while (it != m_logs.end()) {
        it->timeAlive -= dt;
        if (it->timeAlive <= 0) {
            it = m_logs.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& log : m_logs) {
        float alpha = std::clamp(log.timeAlive, 0.0f, 1.0f);

        ImColor finalColor = HUDUtils::fadeColor(log.color, alpha);

        ImVec2 linePos = ImVec2(m_pos.x, m_pos.y + yOffset);
        HUDUtils::drawText(dl, log.text, linePos, finalColor, true);

        float w = HUDUtils::getTextSize(log.text).x;
        if (w > maxWidth) {
            maxWidth = w;
        }

        yOffset += lineHeight;
    }

    m_cachedSize = ImVec2(maxWidth, yOffset > 20 ? yOffset : 20);
}

ImVec2 LogComponent::getSize() { return m_cachedSize; }
