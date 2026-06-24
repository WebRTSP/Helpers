#pragma once

#include <memory>
#include <functional>


class Actor {
    Actor(Actor&) = delete;

    Actor& operator = (Actor&) = delete;

public:
    Actor() noexcept;
    ~Actor() noexcept;

    typedef std::function<void ()> Action;
    void postAction(const Action&) noexcept;
    void postAction(Action&&) noexcept;
    void sendAction(const Action&) noexcept;

private:
    struct Private;
    std::unique_ptr<Private> _p;
};
