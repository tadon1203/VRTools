#pragma once

#include <deque>

#include "Notification.hpp"

class NotificationManager {
public:
    static NotificationManager& instance();

    NotificationManager(const NotificationManager&)            = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;

    void info(const std::string& title, const std::string& message, float duration = 5.0f);
    void warn(const std::string& title, const std::string& message, float duration = 5.0f);
    void error(const std::string& title, const std::string& message, float duration = 5.0f);
    void success(const std::string& title, const std::string& message, float duration = 5.0f);

    void update();
    void render();

private:
    NotificationManager()  = default;
    ~NotificationManager() = default;

    void add(NotificationType type, std::string title, std::string message, float duration = 5.0f);

    std::deque<Notification> m_notifications;
};
