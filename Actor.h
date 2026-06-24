#pragma once

#include <memory>
#include <functional>


class Actor final {
public:
    struct Context;

    Actor(const std::shared_ptr<Context>& = {}) noexcept;
    Actor(Actor&) = delete;
    Actor& operator = (Actor&) = delete;
    ~Actor() noexcept;

    typedef std::function<void ()> Action;
    void postAction(const Action&) noexcept;
    void postAction(Action&&) noexcept;
    void sendAction(const Action&) noexcept;

private:
    struct Private;
    std::unique_ptr<Private> _p;
};

struct Actor::Context {
    virtual ~Context() noexcept = default;
    virtual void activate() noexcept {}
    virtual void deactivate() noexcept {}
};
