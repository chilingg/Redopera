#ifndef RSIGSLOT_H
#define RSIGSLOT_H

#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace Redopera {

/*  目前在多线程中，并没有办法自动阻止在槽函数所在对象析构但未析构到__RSLOT__成员时收到信号，
    所以若不确定槽函数是否在析构时被调用，请手动释放槽 */
class RSlot
{
public:
    template<typename Sloter>
    static void disableSlot(Sloter *sloter)
    {
        while(!sloter->__RSLOT__.flag.unique())
            ;
        sloter->__RSLOT__.flag.reset();
    }

    RSlot():
        flag(std::make_shared<bool>(true))
    {}

    RSlot(RSlot &slot) = delete;
    RSlot& operator=(RSlot &) = delete;

    ~RSlot()
    {
        while(!flag.unique())
            ; // 等待所有槽函数执行完毕才会销毁RSlot，若一直有槽函数执行则死循
        flag.reset();
    }

    std::weak_ptr<bool> clone() const { return std::weak_ptr<bool>(flag); }

private:
    std::shared_ptr<bool> flag; // 存活标志
};


// 在需要使用槽函数的类声明尾部展开宏 _RSLOT_TAIL_
#define _RSLOT_DECLARE_ public: Redopera::RSlot __RSLOT__;

template<typename ... Args>
class RSignal
{
public:
    RSignal() = default;

    RSignal(const RSignal &)  = delete;
    RSignal& operator=(const RSignal &) = delete;

    void operator()(Args ... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = slots_.begin(); it != slots_.end();)
        {
            bool b = it->second(std::forward<Args>(args)...);
            if (!b)
            {
                it = slots_.erase(it);
                continue;
            }
            ++it;
        }
    }

    void emit(Args ... args)
    {
        operator()(std::forward<Args>(args)...);
    }

    template<typename Sloter, typename Sloter2>
    void connect(Sloter *sloter, void (Sloter2::*slot)(Args ... args))
    {
        auto weakptr = sloter->__RSLOT__.clone();
        auto func = std::function<bool(Args ... args)>([weakptr, sloter, slot](Args ... args){
            auto sp = weakptr.lock();
            if(!sp) return false; // 若槽函数所属对象已析构或已主动disableSlot

            (sloter->*slot)(std::forward<Args>(args)...);
            return true;
        });

        std::lock_guard<std::mutex> guard(mutex_);
        slots_.emplace(sloter, func);
    }

    void connect(std::function<bool(Args ...)> func)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        slots_.emplace(nullptr, func);
    }

    void disconnect(void *sloter)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        auto b = slots_.erase(sloter);
    }

    void disconnectAll()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        slots_.clear();
    }

    size_t count()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return slots_.size();
    }

private:
    std::mutex mutex_;
    std::unordered_multimap<void*, std::function<bool(Args ... args)>> slots_;
};

} // Redopera

#endif // RSIGSLOT_H
