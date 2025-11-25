#pragma once

#include <chrono>
#include <string>

enum class NotificationType {
    Info,
    Warning,
    Error,
    Success,
};

struct Notification {
    std::string title;
    std::string message;
    NotificationType type;

    std::chrono::steady_clock::time_point creationTime;
    float duration;

    float currentY     = 0.0f;
    bool isInitialized = false;
};
