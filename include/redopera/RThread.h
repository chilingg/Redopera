#ifndef RTHREAD_H
#define RTHREAD_H

#include <thread>

namespace Redopera {

class RThread
{
public:
    RThread() noexcept = default;

    template<typename Callable, typename ... Args>
    explicit RThread(Callable&& func, Args&& ... args):
        t_(std::forward<Callable>(func), std::forward<Args>(args)...) {}

    explicit RThread(std::thread &&t) noexcept:
        t_(std::move(t)) {}

    RThread(RThread &&other) noexcept:
        t_(std::move(other.t_)) {}

    RThread& operator=(RThread &&other) noexcept
    {
        if(joinable()) join();
        t_ = std::move(other.t_);
        return *this;
    }

    ~RThread() noexcept
    {
        if(joinable()) join();
    }

    void swap(RThread& other) noexcept
    {
        t_.swap(other.t_);
    }

    std::thread::id threadID() const noexcept { return t_.get_id(); }
    bool joinable() const noexcept { return t_.joinable(); }
    void join() { t_.join(); }
    void detach() { t_.detach(); }

private:
    std::thread t_;
};

} // Redopera

#endif // RTHREAD_H
