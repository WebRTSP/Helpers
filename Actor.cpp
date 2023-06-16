#include "Actor.h"

#include <thread>

#include "EventSource.h"


namespace {

struct GlibUnref {
    void operator() (GMainContext* context)
        { g_main_context_unref(context); }
    void operator() (GMainLoop* loop)
        { g_main_loop_unref(loop); }
    void operator() (GAsyncQueue* queue)
        { g_async_queue_unref(queue); }
};

typedef
    std::unique_ptr<
        GMainContext,
        GlibUnref> GMainContextPtr;
typedef
    std::unique_ptr<
        GMainLoop,
        GlibUnref> GMainLoopPtr;
typedef
    std::unique_ptr<
        GAsyncQueue,
        GlibUnref> GAsyncQueuePtr;

struct Action {
    Actor::Action action;
};

void OnEvent(GAsyncQueue* queue)
{
    while(gpointer item = g_async_queue_try_pop(queue)) {
        std::unique_ptr<Action>(static_cast<Action*>(item))->action();
    }
}

void ActorMain(
    GMainContext* mainContext,
    GMainLoop* mainLoop,
    GAsyncQueue* queue,
    EventSource* notifier)
{
    g_main_context_push_thread_default(mainContext);

    notifier->subscribe(std::bind(&OnEvent, queue));

    g_main_loop_run(mainLoop);
}

}

struct Actor::Private {
    Private();

    void postQuit();

    GMainContextPtr mainContextPtr;
    GMainLoopPtr mainLoopPtr;
    GAsyncQueuePtr queuePtr;
    EventSource notifier;
    std::thread actorThread;
};

Actor::Private::Private() :
    mainContextPtr(g_main_context_new()),
    mainLoopPtr(g_main_loop_new(mainContextPtr.get(), FALSE)),
    queuePtr(g_async_queue_new()),
    notifier(mainContextPtr.get()),
    actorThread(
        ActorMain,
        mainContextPtr.get(),
        mainLoopPtr.get(),
        queuePtr.get(),
        &notifier)
{
}

void Actor::Private::postQuit()
{
    GMainLoop* loop = mainLoopPtr.get();

    g_async_queue_push(
        queuePtr.get(),
        new Action {
            [loop] () {
                g_main_loop_quit(loop);
            }
        });
}

Actor::Actor() :
    _p(std::make_unique<Private>())
{
}

Actor::~Actor()
{
    if(_p->actorThread.joinable()) {
        _p->postQuit();
        _p->actorThread.join();
    }
}

void Actor::postAction(const Action& action)
{
    g_async_queue_push(
        _p->queuePtr.get(),
        new ::Action { action });
    _p->notifier.postEvent();
}
