#include "Actor.h"

#include <thread>
#include <mutex>
#include <condition_variable>

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

void OnEvent(GAsyncQueue* queue) noexcept
{
    while(gpointer item = g_async_queue_try_pop(queue)) {
        std::unique_ptr<Action>(static_cast<Action*>(item))->action();
    }
}

void ActorMain(
    GMainContext* mainContext,
    GMainLoop* mainLoop,
    GAsyncQueue* queue,
    EventSource* notifier,
    const std::shared_ptr<Actor::Context>& context) noexcept
{
    g_main_context_push_thread_default(mainContext);

    if(context)
        context->activate(mainContext, mainLoop);

    notifier->subscribe(std::bind(&OnEvent, queue));

    g_main_loop_run(mainLoop);

    if(context)
        context->deactivate();
}

}

struct Actor::Private {
    Private(const std::shared_ptr<Context>&) noexcept;

    GMainContextPtr mainContextPtr;
    GMainLoopPtr mainLoopPtr;
    GAsyncQueuePtr queuePtr;
    EventSource notifier;
    std::thread actorThread;
};

Actor::Private::Private(const std::shared_ptr<Context>& context) noexcept :
    mainContextPtr(g_main_context_new()),
    mainLoopPtr(g_main_loop_new(mainContextPtr.get(), FALSE)),
    queuePtr(g_async_queue_new()),
    notifier(mainContextPtr.get()),
    actorThread(
        ActorMain,
        mainContextPtr.get(),
        mainLoopPtr.get(),
        queuePtr.get(),
        &notifier,
        context)
{
}

Actor::Actor(const std::shared_ptr<Context>& context) noexcept :
    _p(std::make_unique<Private>(context))
{
}

Actor::~Actor() noexcept
{
    if(_p->actorThread.joinable()) {
        postAction([loop = _p->mainLoopPtr.get()] () {
            g_main_loop_quit(loop);
        });
        _p->actorThread.join();
    }
}

void Actor::postAction(const Action& action) noexcept
{
    g_async_queue_push(
        _p->queuePtr.get(),
        new ::Action { action });
    _p->notifier.postEvent();
}

void Actor::postAction(Action&& action) noexcept
{
    g_async_queue_push(
        _p->queuePtr.get(),
        new ::Action { std::move(action) });
    _p->notifier.postEvent();
}

void Actor::sendAction(const Action& action) noexcept
{
    std::mutex guard;
    std::condition_variable conditional;
    bool handled = false;

    std::unique_lock guardLock(guard);

    g_async_queue_push(
        _p->queuePtr.get(),
        new ::Action {
            [&guard, &conditional, &action, &handled] () {
                std::unique_lock guardLock(guard);
                action();
                handled = true;
                conditional.notify_one();
            }
        });
    _p->notifier.postEvent();

    conditional.wait(guardLock, [&handled] () { return handled; });
}
