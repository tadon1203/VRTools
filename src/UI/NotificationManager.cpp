#include "UI/NotificationManager.hpp"

#include <algorithm>
#include <cmath>

#include <imgui.h>
#include <imgui_internal.h>

#include "Utils/Math.hpp"

NotificationManager& NotificationManager::instance() {
    static NotificationManager inst;
    return inst;
}

void NotificationManager::add(NotificationType type, std::string title, std::string message, float duration) {
    Notification n;
    n.title         = std::move(title);
    n.message       = std::move(message);
    n.type          = type;
    n.creationTime  = std::chrono::steady_clock::now();
    n.duration      = duration;
    n.isInitialized = false;

    m_notifications.emplace_back(std::move(n));
}

void NotificationManager::info(const std::string& title, const std::string& message, float duration) {
    add(NotificationType::Info, title, message, duration);
}

void NotificationManager::warn(const std::string& title, const std::string& message, float duration) {
    add(NotificationType::Warning, title, message, duration);
}

void NotificationManager::error(const std::string& title, const std::string& message, float duration) {
    add(NotificationType::Error, title, message, duration);
}

void NotificationManager::success(const std::string& title, const std::string& message, float duration) {
    add(NotificationType::Success, title, message, duration);
}

void NotificationManager::update() {
    if (m_notifications.empty()) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    m_notifications.erase(
        std::remove_if(m_notifications.begin(), m_notifications.end(),
            [&](const Notification& notif) {
                const auto elapsed =
                    std::chrono::duration_cast<std::chrono::duration<float>>(now - notif.creationTime).count();
                return elapsed >= notif.duration;
            }),
        m_notifications.end());
}

void NotificationManager::render() {
    if (m_notifications.empty()) {
        return;
    }

    const auto drawList    = ImGui::GetForegroundDrawList();
    const auto displaySize = ImGui::GetIO().DisplaySize;
    const float dt         = ImGui::GetIO().DeltaTime;

    constexpr float NOTIFY_WIDTH     = 300.0f;
    constexpr float NOTIFY_PADDING   = 12.0f;
    constexpr float NOTIFY_MARGIN_X  = 15.0f;
    constexpr float NOTIFY_MARGIN_Y  = 15.0f;
    constexpr float NOTIFY_ROUNDING  = 6.0f;
    constexpr float ACCENT_THICKNESS = 4.0f;
    constexpr float NOTIFY_SPACING   = 10.0f;

    constexpr float FADE_IN_TIME      = 0.25f;
    constexpr float FADE_OUT_TIME     = 0.5f;
    constexpr float Y_SMOOTHING_SPEED = 15.0f;

    float stackBottomY = displaySize.y - NOTIFY_MARGIN_Y;

    const auto now = std::chrono::steady_clock::now();

    for (auto it = m_notifications.rbegin(); it != m_notifications.rend(); ++it) {
        auto& notif = *it;

        float wrapWidth = NOTIFY_WIDTH - (NOTIFY_PADDING * 2) - ACCENT_THICKNESS;
        ImVec2 titleSize =
            ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, wrapWidth, notif.title.c_str());
        ImVec2 msgSize =
            ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, wrapWidth, notif.message.c_str());

        float contentHeight  = titleSize.y + 5.0f + msgSize.y;
        float totalBoxHeight = NOTIFY_PADDING * 2 + contentHeight;

        const auto elapsedSeconds =
            std::chrono::duration_cast<std::chrono::duration<float>>(now - notif.creationTime).count();

        float alpha       = 1.0f;
        float slideOffset = 0.0f;

        if (elapsedSeconds < FADE_IN_TIME) {
            float t      = elapsedSeconds / FADE_IN_TIME;
            float easedT = Utils::Math::easeOutCubic(t);
            alpha        = easedT;
            slideOffset  = (1.0f - easedT) * (NOTIFY_WIDTH + NOTIFY_MARGIN_X);
        } else if (elapsedSeconds > (notif.duration - FADE_OUT_TIME)) {
            float remaining = notif.duration - elapsedSeconds;
            float t         = 1.0f - (remaining / FADE_OUT_TIME);
            t               = std::clamp(t, 0.0f, 1.0f);

            float easedT = Utils::Math::easeOutCubic(t);

            alpha       = 1.0f - easedT;
            slideOffset = easedT * (NOTIFY_WIDTH + NOTIFY_MARGIN_X);
        }

        float targetY = stackBottomY - totalBoxHeight;

        if (!notif.isInitialized) {
            notif.currentY      = targetY + NOTIFY_SPACING;
            notif.isInitialized = true;
        }

        float t        = 1.0f - std::pow(0.5f, dt * Y_SMOOTHING_SPEED);
        notif.currentY = Utils::Math::lerp(notif.currentY, targetY, t);

        stackBottomY -= (totalBoxHeight + NOTIFY_SPACING);

        float xEnd   = std::floor(displaySize.x - NOTIFY_MARGIN_X + slideOffset);
        float xStart = xEnd - NOTIFY_WIDTH;
        float yStart = std::floor(notif.currentY);
        float yEnd   = std::floor(notif.currentY + totalBoxHeight);

        ImVec2 posMin(xStart, yStart);
        ImVec2 posMax(xEnd, yEnd);


        ImU32 accentColor = 0;
        ImU32 titleColor  = ImColor(1.0f, 1.0f, 1.0f, alpha);
        ImU32 msgColor    = ImColor(0.8f, 0.8f, 0.8f, alpha);
        ImU32 bgColor     = ImColor(0.1f, 0.1f, 0.1f, 0.9f * alpha);

        switch (notif.type) {
        case NotificationType::Info:
            accentColor = ImColor(0.2f, 0.6f, 1.0f, alpha);
            break;
        case NotificationType::Warning:
            accentColor = ImColor(1.0f, 0.8f, 0.2f, alpha);
            break;
        case NotificationType::Error:
            accentColor = ImColor(1.0f, 0.3f, 0.3f, alpha);
            break;
        case NotificationType::Success:
            accentColor = ImColor(0.2f, 0.9f, 0.4f, alpha);
            break;
        }

        drawList->AddRectFilled(posMin, posMax, bgColor, NOTIFY_ROUNDING);

        drawList->AddRectFilled(posMin, ImVec2(posMin.x + ACCENT_THICKNESS, posMax.y), accentColor, NOTIFY_ROUNDING,
            ImDrawFlags_RoundCornersLeft);

        float textX = std::floor(posMin.x + ACCENT_THICKNESS + NOTIFY_PADDING);
        float textY = std::floor(posMin.y + NOTIFY_PADDING);

        drawList->AddText(
            ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(textX, textY), titleColor, notif.title.c_str());

        textY += std::floor(titleSize.y + 5.0f);
        drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(textX, textY), msgColor, notif.message.c_str(),
            nullptr, wrapWidth);
    }
}
