#pragma once

#include <memory>
#include <functional>


class Actor final {
public:
    Actor() noexcept;
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
