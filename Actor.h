#pragma once

#include <memory>
#include <functional>


class Actor {
    Actor(Actor&) = delete;

    Actor& operator = (Actor&) = delete;

public:
    Actor();
    ~Actor();

    typedef std::function<void ()> Action;
    void postAction(const Action&);
    void postAction(Action&&);
    void sendAction(const Action&);

private:
    struct Private;
    std::unique_ptr<Private> _p;
};
