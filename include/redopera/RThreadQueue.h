#ifndef RTHREADQUEUE_H
#define RTHREADQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace Redopera {

template <typename T>
class RThreadQueue
{
public:
    RThreadQueue() = default;
    RThreadQueue(const RThreadQueue&) = delete;
    RThreadQueue& operator=(const RThreadQueue&) = delete;

    void push(T value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(std::move(value));
        cond_.notify_one();
    }

    bool tryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(data_.empty()) return false;
        value = std::move(data_.front());
        data_.pop();
        return true;
    }

    void waitAndPop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]{ return !data_.empty(); });
        value = std::move(data_.front());
        data_.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

private:
    mutable std::mutex mutex_;
    std::queue<T> data_;
    std::condition_variable cond_;
};

} // Redopera

#endif // RTHREADQUEUE_H
