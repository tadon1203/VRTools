#pragma once

class IEvent {
public:
    virtual ~IEvent() = default;
};

class ICancellableEvent : public IEvent {
public:
    void cancel() { m_isCancelled = true; }
    [[nodiscard]] bool isCancelled() const { return m_isCancelled; }

private:
    bool m_isCancelled = false;
};
