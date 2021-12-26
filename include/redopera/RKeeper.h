#ifndef RKEEPER_H
#define RKEEPER_H

#include <functional>

namespace Redopera {

template <typename T, typename Deleter>
class RKeeper
{
    static_assert(std::is_arithmetic<T>::value or std::is_pointer<T>::value, "T is not a arithmetic or pointer type!");
    static_assert(std::is_invocable<Deleter, T>::value, "Bad deleter");

public:
    RKeeper(): valid_(false) {}

    RKeeper(T&& value, Deleter&& func):
        valid_(true),
        free_(std::forward<Deleter>(func)),
        value_(value)
    {}

    RKeeper(const RKeeper &keeper) = delete;
    RKeeper& operator=(const RKeeper &keeper) = delete;

    RKeeper(RKeeper &&keeper):
        valid_(keeper.valid_),
        free_(std::move(keeper.free_)),
        value_(keeper.value_)
    { keeper.valid_ = false; }

    RKeeper& operator=(RKeeper &&keeper)
    {
        valid_ = keeper.valid_;
        free_ = std::move(keeper.free_);
        value_ = keeper.value_;

        keeper.valid_ = false;
        return *this;
    }

    operator T& () { return value_; }
    operator const T& () const { return value_; }

    ~RKeeper() { if(valid_) free_(value_); }

    bool isValid() const { return valid_; }
    T& get() { return value_; }

    void reset(T value, std::function<void(T)> func)
    {
        if(valid_) free_(value_);

        valid_ = true;
        free_ = std::move(func);
        value_ = value;
    }

    T release() { valid_ = false; return value_; }

    void free()
    {
        if(valid_) free_(value_);
    }

private:
    bool valid_;
    T value_;
    Deleter free_;
};

} // Redopera

#endif // RKEEPER_H
