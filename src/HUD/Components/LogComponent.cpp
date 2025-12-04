#include "LogComponent.hpp"

#include <algorithm>
#include <vector>

#include "Utils/ImGuiUtils.hpp"

#include "Utils/RenderUtils.hpp"

LogComponent::LogComponent()
    : HUDComponent("LogOverlay", ImVec2(20, 300)) {
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
    float maxWidth   = 0.0f;
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

    std::vector<float> widths;
    widths.reserve(m_logs.size());
    for (const auto& log : m_logs) {
        float w = ImGuiUtils::getTextSize(log.text).x;
        widths.push_back(w);
        if (w > maxWidth) {
            maxWidth = w;
        }
    }

    if (maxWidth < 100.0f) {
        maxWidth = 100.0f;
    }

    bool rightAlign = (m_anchor == HUDAnchor::TopRight || m_anchor == HUDAnchor::BottomRight);

    bool newestAtAnchor = true;
    bool invertOrder    = newestAtAnchor && (m_anchor == HUDAnchor::TopLeft || m_anchor == HUDAnchor::TopRight);

    auto drawLog = [&](const LogEntry& log, float w) {
        float alpha        = std::clamp(log.timeAlive, 0.0f, 1.0f);
        ImColor finalColor = ImGuiUtils::fadeColor(log.color, alpha);

        float x = m_pos.x;
        if (rightAlign) {
            x += (maxWidth - w);
        }

        ImVec2 linePos = ImVec2(x, m_pos.y + yOffset);
        RenderUtils::renderText(dl, linePos, log.text, finalColor, true);

        yOffset += lineHeight;
    };

    if (invertOrder) {
        for (size_t i = 0; i < m_logs.size(); ++i) {
            size_t idx = m_logs.size() - 1 - i;
            drawLog(m_logs[idx], widths[idx]);
        }
    } else {
        for (size_t i = 0; i < m_logs.size(); ++i) {
            drawLog(m_logs[i], widths[i]);
        }
    }

    m_cachedSize = ImVec2(maxWidth, yOffset > 20 ? yOffset : 20);
}

ImVec2 LogComponent::getSize() { return m_cachedSize; }
