#ifndef RTIMER_H
#define RTIMER_H

#include <chrono>

namespace Redopera {

using Clock = std::chrono::high_resolution_clock;

/*
template<typename durationType>
class RStopwatchBasic;

template <typename Rep, typename Peiod>
class RStopwatchBasic<std::chrono::duration<Rep, Peiod>>
{
    using rep = Rep;
    RStopwatchBasic():
        point_(std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count())
    {}

    rep elapsed() const
    {
        std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count() - point_;
    }

    void start() const
    {
        point_ = std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count();
    }

private:
    rep point_;
};

using RStopwatch = RStopwatchBasic<std::chrono::milliseconds>;      // 毫秒计时器
using RStopwatchMS = RStopwatchBasic<std::chrono::microseconds>;    // 微妙计时器
using RStopwatchNS = RStopwatchBasic<std::chrono::nanoseconds>;     // 纳秒计时器

template<typename durationType>
class RTimerBasic;

template <typename Rep, typename Peiod>
class RTimerBasic<std::chrono::duration<Rep, Peiod>>
{
    using rep = Rep;
    RTimerBasic() {}

    bool isStarted() const { return !paused_; }
    bool isPaused() const { return paused_; }

    rep elapsed() const
    {
        paused_ ? point_ :
        std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count() - point_;
    }

    void start()
    {
        paused_ = false;
        point_ = std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count();
    }

    void pause()
    {
        if(!paused_)
        {
            point_ = std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count() - point_;
            paused_ = false;
        }
    }

private:
    bool paused_ = true;
    rep point_ = 0;
};

using RTimer = RTimerBasic<std::chrono::seconds>;
using RTimerMS = RTimerBasic<std::chrono::milliseconds>;
*/

class RTimer // 毫秒级计时器
{
public:
    RTimer():
        time_(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count())
    {}

    int64_t elapsed() const
    {
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

    int64_t elapsed() const
    {
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
