#pragma once

#include <memory>
#include <functional>

#include <glib.h>


class EventSource2 {
    EventSource2(EventSource2&) = delete;

    EventSource2& operator = (EventSource2&) = delete;

public:
    EventSource2(GMainContext* context);
    ~EventSource2();

    void postEvent();

    typedef std::function<void ()> EventTarget;
    void subscribe(const EventTarget&);

private:
    struct Private;
    std::unique_ptr<Private> _p;
};
