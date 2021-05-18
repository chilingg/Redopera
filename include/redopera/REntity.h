#ifndef RENTITY_H
#define RENTITY_H

#include <RNode.h>
#include <RMath.h>
#include <RSigslot.h>

#include <any>
#include <unordered_map>
#include <cassert>
#include <variant>

namespace Redopera {

class REntity
{
public:
    using ComponentList = std::unordered_map<RName, std::any>;
    using FuncList = ComponentList;
    using SignalList = std::unordered_map<RName, std::any>;
    using EntityList = std::unordered_map<RName, std::unique_ptr<REntity>>;

    explicit REntity(const RName &name):
        node(name, this)
    {}

    REntity(const REntity&) = delete;

    const std::type_info& compenentType(const RName &name) const { return comps_.at(name).type(); }

    bool isComponent(const RName &name) const { return comps_.count(name); }
    bool isFunc(const RName &name) const { return funcs_.count(name); }
    bool isSignal(const RName &name) const { return signals_.count(name); }
    bool isEntity(const RName &name) const { return entitys_.count(name); }

    const ComponentList& compenentList() const { return comps_; }
    const FuncList& funcList() const { return funcs_; }
    const SignalList& signalList() const { return signals_; }
    const EntityList& entityList() const { return entitys_; }

    size_t compenentSize() const { return comps_.size(); }
    size_t funcSize() const { return funcs_.size(); }
    size_t signalSize() const { return signals_.size(); }
    size_t entitySize() const { return entitys_.size(); }

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
        return *std::any_cast<std::shared_ptr<RSignal<Args ...>>&>(signals_.at(name));
    }

    REntity& entity(const RName &name)
    {
        return *entitys_.at(name);
    }

    void addNumber(const RName &name, RNumber comp)
    {
        auto it = comps_.emplace(name, comp);
        assert(it.second);
    }

    template<typename T>
    void addComponent(const RName &name, T&& comp)
    {
        auto it = comps_.emplace(name, std::forward<T>(comp));
        assert(it.second);
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
        auto it = signals_.emplace(name, std::make_shared<RSignal<Args ...>>());
        assert(it.second);
    }

    REntity& addEntity(const RName &name)
    {
        auto it = entitys_.emplace(name, std::make_unique<REntity>(name));
        assert(it.second);
        it.first->second->node.changeParent(&node);

        return *it.first->second;
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

    void removeEntity(const RName &name)
    {
        size_t s = entitys_.erase(name);
        assert(s);
    }

    RNode node;

private:
    ComponentList comps_;
    FuncList funcs_;
    SignalList signals_;
    EntityList entitys_;

    _RSLOT_DECLARE_
};

} // ns Redopera

#endif // RENTITY_H
