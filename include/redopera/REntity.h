#ifndef RENTITY_H
#define RENTITY_H

#include <RName.h>
#include <RSigslot.h>
#include <RDict.h>
#include <RMath.h>

#include <any>
#include <cassert>

namespace Redopera {

class REntity
{
    enum class Status
    {
        Normal,
        Action,
        Stop,
        Error,
    };

public:
    using ComponentList = RDict<>;
    using FuncList = ComponentList;
    using SignalList = ComponentList;
    using ChildrenList = std::vector<REntity>;

    REntity(const RName &name, REntity *parent);
    REntity(const REntity& entity);
    REntity(REntity&& entity);

    REntity& operator=(const REntity& entity);
    REntity& operator=(REntity&& entity);

    const std::type_info& compenentType(const RName &name) const { return comps_.at(name).type(); }

    bool isComponent(const RName &name) const { return comps_.count(name); }
    bool isFunc(const RName &name) const { return funcs_.count(name); }
    bool isSignal(const RName &name) const { return signals_.count(name); }
    bool isChildren(const RName &name) const;

    RName name() const { return name_; }
    REntity* parent() const { return parent_; }

    const ComponentList& compenentList() const { return comps_; }
    const FuncList& funcList() const { return funcs_; }
    const SignalList& signalList() const { return signals_; }
    const ChildrenList& childrenList() const { return children_; }

    size_t compenentSize() const { return comps_.size(); }
    size_t funcSize() const { return funcs_.size(); }
    size_t signalSize() const { return signals_.size(); }
    size_t childrenSize() const { return children_.size(); }

    template<typename T>
    const T& get(const RName &name) const
    {
        return std::any_cast<const T&>(comps_.at(name));
    }

    template<typename T>
    T& getR(const RName &name)
    {
        return std::any_cast<T&>(comps_.at(name));
    }

    template<typename Result, typename ... Args>
    Result func(const RName &name, Args&& ... args)
    {
        return std::any_cast<std::function<Result(Args ...)>>(funcs_.at(name))(std::forward<Args>(args)...);
    }

    template<typename ... Args>
    RSignal<Args ...>& sigal(const RName &name)
    {
        return std::any_cast<RSignal<Args ...>&>(signals_.at(name));
    }

    REntity& child(const RName &name)
    {
        auto it = std::find_if(children_.begin(), children_.end(), [name](const REntity &e){ return e.name_ == name; });
        assert(it != children_.end());

        return *it;
    }

    RNumber& addNumber(const RName &name, RNumber comp)
    {
        auto it = comps_.emplace(name, comp);
        assert(it.second);

        return std::any_cast<RNumber&>(it.first->second);
    }

    template<typename T>
    T& addComponent(const RName &name, T&& comp)
    {
        auto it = comps_.emplace(name, std::forward<T>(comp));
        assert(it.second);

        return std::any_cast<T&>(it.first->second);
    }

    template<typename Result, typename ... Args>
    void addFunc(const RName &name, const std::function<Result(Args ...)> &func)
    {
        auto it = funcs_.emplace(name, func);
        assert(it.second);
    }

    template<typename Result, typename ... Args>
    void addFunc(const RName &name, Result(*func)(Args ...))
    {
        addFunc(name, std::function<Result(Args ...)>(func));
    }

    template<typename ... Args, typename FuncType>
    void addFunc(const RName &name, FuncType func)
    {
        addFunc(name, std::function<typename std::result_of<FuncType(Args ...)>::type(Args ...)>(func));
    }

    template<typename ... Args>
    void addSignal(const RName &name)
    {
        auto it = signals_.emplace(name, RSignal<Args ...>());
        assert(it.second);
    }

    REntity& addChild(const RName &name)
    {
        children_.emplace_back(name, this);
        children_.back().status_ = status_;

        return children_.back();
    }

    void removeComponent(const RName &name)
    {
        size_t s = comps_.erase(name);
        assert(s);
    }

    void removeFunc(const RName &name)
    {
        size_t s = funcs_.erase(name);
        assert(s);
    }

    void removeSignal(const RName &name)
    {
        size_t s = signals_.erase(name);
        assert(s);
    }

    void removeChild(const RName &name)
    {
        auto it = std::find_if(children_.begin(), children_.end(), [name](const REntity &e){ return e.name_ == name; });
        assert(it != children_.end());

        children_.erase(it);
    }

    template<typename Result, typename ... Args>
    void callFuncToAll(RName name, Args&& ... args)
    {
        if(isFunc(name) && funcs_[name].type() == typeid(std::function<Result(Args ...)>))
            func<Result, Args ...>(name, std::forward<Args>(args) ...);

        for(REntity &e : children_)
            e.callFuncToAll<Result, Args ...>(name, std::forward<Args>(args) ...);
    }

    template<typename Result, typename ... Args>
    void callFuncToAllFromThis(RName name, Args&& ... args)
    {
        if(isFunc(name) && funcs_[name].type() == typeid(std::function<Result(REntity&, Args ...)>))
            func<Result, REntity&, Args ...>(name, *this, std::forward<Args>(args) ...);

        for(REntity &e : children_)
            e.callFuncToAll<Result, Args ...>(name, std::forward<Args>(args) ...);
    }

    void rename(RName name) { name_ = name; }
    void setStatus(Status status) { status_ = status; }
    void moveAttrTo(REntity &entity);

private:
    RName name_;
    Status status_ = Status::Normal;
    REntity *parent_ = nullptr;

    ComponentList comps_;
    FuncList funcs_;
    SignalList signals_;
    ChildrenList children_;
};

} // ns Redopera

#endif // RENTITY_H
