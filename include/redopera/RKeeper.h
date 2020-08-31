#ifndef RKEEPER_H
#define RKEEPER_H

#include <functional>

namespace Redopera {

template <typename T>
class RKeeper
{
public:
    RKeeper(T value, std::function<void(T)> func):
        free_(func),
        value_(std::forward<T>(value)) {}

    RKeeper(const RKeeper &keeper) = delete;
    RKeeper& operator=(const RKeeper &keeper) = delete;

    RKeeper(RKeeper &&keeper):
        free_(std::move(keeper.free_)),
        value_(std::move(keeper.value_))
    { keeper.free_ = std::function<void(T)>(); }

    RKeeper& operator=(RKeeper &&keeper)
    {
        free_ = std::move(keeper.free_);
        value_ = std::move(keeper.value_);
        keeper.free_ = std::function<void(T)>();
        return *this;
    }

    RKeeper() {};

    operator T& () { return value_; }

    ~RKeeper() { if(free_) free_(value_); }

    T& get() { return value_; }

    void reset(const T &value, const std::function<void(T)> &func)
    {
        if(free_) free_(value_);
        free_ = func;
        value_ = value;
    }

    void reset()
    {
        if(free_) free_(value_);
        free_ = std::function<void(T)>();
    }

private:
    std::function<void(T)> free_;
    T value_;
};

} // Redopera

#endif // RKEEPER_H
