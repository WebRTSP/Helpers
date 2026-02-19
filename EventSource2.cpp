#include "EventSource2.h"


namespace {

struct GEventSource
{
    GSource base;
};

gboolean Dispatch(
    GSource* source,
    GSourceFunc sourceCallback,
    gpointer userData)
{
    g_source_set_ready_time(source, -1);

    sourceCallback(userData);

    return G_SOURCE_CONTINUE;
}

void PostEvent(GSource* source)
{
    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);

    g_source_set_ready_time(source, 0);
}

GEventSource* EventSourceAdd(GMainContext* context)
{
    static GSourceFuncs funcs = {
        .dispatch = Dispatch,
    };

    GSource* source = g_source_new(&funcs, sizeof(GEventSource));

    GEventSource* eventSource = reinterpret_cast<GEventSource*>(source);

    g_source_attach(source, context);

    return eventSource;
}

}


struct EventSource2::Private
{
    GEventSource* eventSource;
    EventTarget eventTarget;

    void onEvent();
};

void EventSource2::Private::onEvent()
{
    if(eventTarget)
        eventTarget();
}

EventSource2::EventSource2(GMainContext* context) :
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

EventSource2::~EventSource2()
{
    if(_p->eventSource) {
        g_source_unref(reinterpret_cast<GSource*>(_p->eventSource));
        _p->eventSource = nullptr;
    }
}

void EventSource2::postEvent()
{
    if(!_p->eventSource) return;

    PostEvent(reinterpret_cast<GSource*>(_p->eventSource));
}

void EventSource2::subscribe(const EventTarget& eventTarget)
{
    _p->eventTarget = eventTarget;
}
