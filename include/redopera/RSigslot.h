#ifndef RSIGSLOT_H
#define RSIGSLOT_H

#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace Redopera {

class RSlot
{
public:
    RSlot(size_t typeHash):
        sloterTypeHash(typeHash),
        slotFlag(std::make_shared<bool>(true))
    {}

    RSlot(RSlot &slot):
        sloterTypeHash(slot.sloterTypeHash),
        slotFlag(std::make_shared<bool>(true))
    {}

    RSlot& operator=(RSlot &) { return *this; }

    ~RSlot()
    {
        while(!slotFlag.unique())
            ; // 等待所有槽函数执行完毕才会销毁RSlot，若一直有槽函数执行则死循
        slotFlag.reset();
    }

    std::weak_ptr<bool> clone() const { return std::weak_ptr<bool>(slotFlag); }

    const size_t sloterTypeHash; // 拥有RSlot的类的指针的类型哈希值，用于在connect中确定该类有自己的RSlot而不是继承而来的

private:
    std::shared_ptr<bool> slotFlag; // 存活标志
};

// 在需要使用槽函数的类声明尾部展开宏 _RSLOT_TAIL_
#define _RSLOT_TAIL_ public: const Redopera::RSlot __RSLOT__ { typeid(this).hash_code() };

template<typename ... Args>
class RSignal
{
public:
    RSignal() = default;
    RSignal(RSignal &&sig): mutex_(std::move(sig.mutex_)), slots_(std::move(sig.slots_)) {}
    RSignal& operator=(RSignal &&sig) { mutex_ = std::move(sig.mutex_); slots_ = std::move(sig.slots_); };

    void operator()(Args ... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto it = slots_.begin(); it != slots_.end(); ++it)
        {
            bool b = it->second(std::forward<Args>(args)...);
            if(!b)
            {
                it = slots_.erase(it);
                if(it == slots_.end()) break; //初始单个槽函数返回false的情况下，不加不行
            }
        }
    }

    void emit(Args ... args)
    {
        operator()(std::forward<Args>(args)...);
    }

    template<typename Sloter, typename Sloter2>
    void connect(Sloter *sloter, void (Sloter2::*slot)(Args ... args))
    {
        // 使用槽函数的类需要在声明尾部展开宏_RSLOT_TAIL_
        if(typeid(Sloter*).hash_code() != sloter->__RSLOT__.sloterTypeHash)
            throw std::logic_error(std::string(typeid(Sloter).name())
                                   + ": Using slot functions need to expand the macro _RSLOT_TAIL_ at the tail of the declaration");

        auto weakptr = sloter->__RSLOT__.clone();
        auto func = std::function<bool(Args ... args)>([weakptr, sloter, slot](Args ... args){
            auto sp = weakptr.lock();
            if(!sp) return false; // 若槽函数已析构

            (sloter->*slot)(std::forward<Args>(args)...);
            return true;
        });

        std::lock_guard<std::mutex> guard(mutex_);
        slots_.emplace(sloter, func);
    }

    void connect(const std::function<bool(Args ...)> &func)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        slots_.emplace(nullptr, func);
    }

    template<typename Sloter> // 断开所有关于该类的槽连接 PS:因为不知道如何统一储存成员函数变量
    void disconnect(Sloter *sloter)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        slots_.erase(sloter);
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
