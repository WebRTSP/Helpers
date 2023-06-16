#include "EventSource.h"

#include <sys/eventfd.h>
#include <unistd.h>


namespace {

struct GEventSource
{
    GSource base;

    int notifyFd;
    gpointer notifyFdTag;
};

gboolean Prepare(GSource* source, gint* timeout)
{
    *timeout = -1;

    return FALSE;
}

gboolean Check(GSource* source)
{
    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);

    eventfd_t value;
    if(0 == eventfd_read(eventSource->notifyFd, &value)) {
        return value != 0;
    }

    return FALSE;
}

gboolean Dispatch(
    GSource* /*source*/,
    GSourceFunc sourceCallback,
    gpointer userData)
{
    sourceCallback(userData);

    return G_SOURCE_CONTINUE;
}

void Finalize(GSource* source)
{
    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);

    g_source_remove_unix_fd(source, eventSource->notifyFdTag);
    eventSource->notifyFdTag = nullptr;

    close(eventSource->notifyFd);
    eventSource->notifyFd = -1;
}

void PostEvent(GSource* source)
{
    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);

    eventfd_write(eventSource->notifyFd, 1);
}

GEventSource* EventSourceAdd(GMainContext* context)
{
    static GSourceFuncs funcs = {
        .prepare = Prepare,
        .check = Check,
        .dispatch = Dispatch,
        .finalize = Finalize,
    };

    GSource* source = g_source_new(&funcs, sizeof(GEventSource));

    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);
    eventSource->notifyFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    eventSource->notifyFdTag = g_source_add_unix_fd(source, eventSource->notifyFd, G_IO_IN);

    g_source_attach(source, context);

    return eventSource;
}

}


struct EventSource::Private
{
    GEventSource* eventSource;
    EventTarget eventTarget;

    void onEvent();
};

void EventSource::Private::onEvent()
{
    if(eventTarget)
        eventTarget();
}

EventSource::EventSource(GMainContext* context) :
    _p(std::make_unique<Private>())
{
    _p->eventSource = EventSourceAdd(context);
    auto callback =
        [] (gpointer user_data) -> gboolean {
            Private* p = static_cast<Private*>(user_data);
            p->onEvent();
            return G_SOURCE_CONTINUE;
        };
    g_source_set_callback(
        reinterpret_cast<GSource*>(_p->eventSource),
        callback,
        _p.get(),
        nullptr);
}

EventSource::~EventSource()
{
    if(_p->eventSource) {
        g_source_unref(reinterpret_cast<GSource*>(_p->eventSource));
        _p->eventSource = nullptr;
    }
}

void EventSource::postEvent()
{
    if(!_p->eventSource) return;

    PostEvent(reinterpret_cast<GSource*>(_p->eventSource));
}

void EventSource::subscribe(const EventTarget& eventTarget)
{
    _p->eventTarget = eventTarget;
}
