#ifndef RTIMER_H
#define RTIMER_H

#include <thread>
#include <chrono>

namespace Redopera {

using Clock = std::chrono::high_resolution_clock;

class RTimer // 毫秒级计时器
{
public:
    RTimer():
        time_(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count())
    {}

    int64_t elapsed()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count() - time_;
    }

    int64_t elapsed(int64_t time)
    {
        int64_t elp = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count() - time_;
        if(elp <= time) std::this_thread::sleep_for(std::chrono::milliseconds(time - elp));
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count() - time_;
    }

    void start()
    {
        time_ = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
    }

private:
    int64_t time_ = 0;
};

class RTimerNS // 纳秒级计时器
{
public:
    RTimerNS():
        time_(std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count())
    {}

    int64_t elapsed()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count() - time_;
    }

    int64_t elapsed(int64_t time)
    {
        int64_t elp = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count() - time_;
        if(elp <= time) std::this_thread::sleep_for(std::chrono::microseconds(time - elp));
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count() - time_;
    }

    void start()
    {
        time_ = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count();
    }

private:
    int64_t time_ = 0;
};

} // Redopera

#endif // RTIMER_H
