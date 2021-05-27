#ifndef RSIGSLOT_H
#define RSIGSLOT_H

#include <memory>
#include <unordered_map>
#include <functional>

namespace Redopera {

/* 不支持多线程 */

class RSlot
{
public:
    template<typename Sloter>
    static void disableSlot(Sloter *sloter)
    {
        sloter->__RSLOT__.flag.reset();
    }

    template<typename Sloter>
    static void resetSlot(Sloter *sloter)
    {
        sloter->__RSLOT__.flag = std::make_shared<bool>(true);
    }

    RSlot():
        flag(std::make_shared<bool>(true))
    {}

    RSlot(const RSlot &):RSlot() {};
    RSlot& operator=(const RSlot &) { return *this; };

    RSlot(RSlot &&slot):
        flag(std::move(slot.flag))
    {
        slot.flag = nullptr;
    }

    RSlot& operator=(RSlot &&slot)
    {
        flag = std::move(slot.flag);
        slot.flag = nullptr;
        return *this;
    }

    ~RSlot()
    {
        flag.reset();
    }

    std::weak_ptr<bool> clone() const { return std::weak_ptr<bool>(flag); }
    void* flagAddr() const { return flag.get(); }

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

    RSignal(const RSignal &)
    {}
    RSignal& operator=(const RSignal &)
    {}

    RSignal(const RSignal &&slot):
        slots_(std::move(slot.slots_))
    {}
    RSignal& operator=(RSignal &&slot)
    {
        slots_ = std::move(slot.slots_);
        return *this;
    }

    void operator()(Args ... args)
    {
        emit(std::forward<Args>(args)...);
    }

    void emit(Args ... args)
    {
        for(auto it = slots_.begin(); it != slots_.end();)
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

    template<typename Sloter>
    void connect(const Sloter &sloter, const std::function<void(Args ...)> &slot)
    {
        auto weakptr = sloter.__RSLOT__.clone();
        auto func = std::function<bool(Args ... args)>([weakptr, slot](Args ... args){
            auto sp = weakptr.lock();
            if(!sp) return false; // 若槽函数所属对象已析构或已主动disableSlot

            slot(std::forward<Args>(args)...);
            return true;
        });

        slots_.emplace(sloter.__RSLOT__.flagAddr(), func);
    }

    void connect(const RSlot& slot, const std::function<void(Args ...)> &sfunc)
    {
        auto weakptr = slot.clone();
        auto func = std::function<bool(Args ... args)>([weakptr, sfunc](Args ... args){
            auto sp = weakptr.lock();
            if(!sp) return false; // 若槽函数所属对象已析构或已主动disableSlot

            sfunc(std::forward<Args>(args)...);
            return true;
        });

        slots_.emplace(slot.flagAddr(), func);
    }

    void disconnect(RSlot *sloter)
    {
        slots_.erase(sloter->flagAddr());
    }

    template<typename Sloter>
    void disconnect(Sloter *sloter)
    {
        slots_.erase(sloter->__RSLOT__.flagAddr());
    }

    void disconnectAll()
    {
        slots_.clear();
    }

    size_t count()
    {
        return slots_.size();
    }

private:
    std::unordered_multimap<void*, std::function<bool(Args ... args)>> slots_;
};

} // Redopera

#endif // RSIGSLOT_H
