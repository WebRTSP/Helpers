#pragma once

#include <memory>
#include <functional>

#include <glib.h>


class EventSource {
    EventSource(EventSource&) = delete;

    EventSource& operator = (EventSource&) = delete;

public:
    EventSource(GMainContext* context);
    ~EventSource();

    void postEvent();

    typedef std::function<void ()> EventTarget;
    void subscribe(const EventTarget&);

private:
    struct Private;
    std::unique_ptr<Private> _p;
};
