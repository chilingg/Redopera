#ifndef RTIMER_H
#define RTIMER_H

#include <chrono>

namespace Redopera {

using Clock = std::chrono::high_resolution_clock;

namespace basic {

template<typename durationType>
class RStopwatchBasic;

template <typename Rep, typename Peiod>
class RStopwatchBasic<std::chrono::duration<Rep, Peiod>>
{
public:
    using rep = Rep;

    RStopwatchBasic():
        point_(std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count())
    {}

    rep elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count() - point_;
    }

    void start()
    {
        point_ = std::chrono::duration_cast<std::chrono::duration<Rep, Peiod>>(Clock::now().time_since_epoch()).count();
    }

private:
    rep point_;
};

template<typename durationType>
class RTimerBasic;

template <typename Rep, typename Peiod>
class RTimerBasic<std::chrono::duration<Rep, Peiod>>
{
public:
    using rep = Rep;

    RTimerBasic(bool started = false):paused_(!started) { if(started) start(); }

    bool isStarted() const { return !paused_; }
    bool isPaused() const { return paused_; }

    rep elapsed() const
    {
        return paused_ ? point_ :
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
    bool paused_;
    rep point_ = 0;
};

extern template class basic::RStopwatchBasic<std::chrono::milliseconds>;
extern template class basic::RStopwatchBasic<std::chrono::microseconds>;
extern template class basic::RStopwatchBasic<std::chrono::nanoseconds>;

extern template class basic::RTimerBasic<std::chrono::seconds>;
extern template class basic::RTimerBasic<std::chrono::milliseconds>;

}

using RStopwatch = basic::RStopwatchBasic<std::chrono::milliseconds>;      // 毫秒计时器
using RStopwatchMS = basic::RStopwatchBasic<std::chrono::microseconds>;    // 微妙计时器
using RStopwatchNS = basic::RStopwatchBasic<std::chrono::nanoseconds>;     // 纳秒计时器

using RTimer = basic::RTimerBasic<std::chrono::seconds>;
using RTimerMS = basic::RTimerBasic<std::chrono::milliseconds>;

} // Redopera

#ifdef REDOPERA_DEFINE_FILE
template class Redopera::basic::RStopwatchBasic<std::chrono::milliseconds>;
template class Redopera::basic::RStopwatchBasic<std::chrono::microseconds>;
template class Redopera::basic::RStopwatchBasic<std::chrono::nanoseconds>;

template class Redopera::basic::RTimerBasic<std::chrono::seconds>;
template class Redopera::basic::RTimerBasic<std::chrono::milliseconds>;
#endif

#endif // RTIMER_H
