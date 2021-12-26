#ifndef RSIGSLOT_H
#define RSIGSLOT_H

#include <memory>
#include <functional>
#include <map>

namespace Redopera {

class RSloter
{
public:
    using SlotFlag = std::shared_ptr<bool>;

    RSloter():
        flags_(std::make_shared<bool>(true))
    {}

    RSloter(const RSloter &) = delete;
    RSloter& operator=(const RSloter &) = delete;

    RSloter(RSloter &&sloter):
        flags_(std::move(sloter.flags_))
    {}

    RSloter& operator=(RSloter &&sloter)
    {
        if(&sloter == this)
            return *this;

        free();
        flags_ = std::move(sloter.flags_);

        return *this;
    }

    ~RSloter()
    {
        free();
    }

    SlotFlag& flag() { return flags_; }

    void free()
    {
        if(flags_)
        {
            (*flags_) = false;
            flags_.reset();
        }
    }

private:
    SlotFlag flags_;
};

// 在需要使用槽函数的类声明尾部展开宏 _RSLOT_TAIL_
#define _RSLOT_DECLARE_ public: Redopera::RSlot __RSLOT__;

template<typename ... Args>
class RSignal
{
public:
    RSignal() = default;

    RSignal(const RSignal &) = delete;
    RSignal& operator=(const RSignal &) = delete;

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
            if(*it->second.first)
                it->second.second(std::forward<Args>(args)...);
            else
            {
                it = slots_.erase(it);
                continue;
            }
            ++it;
        }
    }

    size_t connect(RSloter *sloter, std::function<void(Args ...)> slot)
    {
        static RSloter defaultSloter;

        if(!sloter)
            sloter = &defaultSloter;

        slots_.emplace(idCount_, std::make_pair(sloter->flag(), std::move(slot)));
        return idCount_++;
    }

    bool disconnect(size_t id)
    {
        return slots_.erase(id);
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
    size_t idCount_ = 0;
    std::map<size_t, std::pair<RSloter::SlotFlag, std::function<void(Args ... args)>>> slots_;
};

} // Redopera

#endif // RSIGSLOT_H
