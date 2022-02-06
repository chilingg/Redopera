#ifndef RENTITY_H
#define RENTITY_H

#include "RName.h"
#include "RDict.h"

namespace Redopera {

template<typename ...Types>
class REntity
{
public:
    using Component = std::variant<Types ...>;

    REntity() = default;
    REntity(const REntity &e) = default;
    REntity(REntity &&e) = default;
    REntity& operator=(const REntity &e) = default;
    REntity& operator=(REntity &&e) = default;

    size_t index(RName name) const { return component_.at(name).index(); }
    const RDict<Component>& componentList() const { return component_; }
    RDict<Component>& componentListR() { return component_; }
    size_t size() const { return component_.size(); }

    bool isComponent(RName name) const { return component_.contains(name); }
    template<typename T>
    bool isComponent(RName name) const { return component_.contains(name) ? std::holds_alternative<T>(component_.at(name)) : false; }

    template<typename T>
    const T& get(RName name) const { return std::get<T>(component_.at(name)); }
    template<typename T>
    T& get(RName name){ return std::get<T>(component_.at(name)); }

    template<typename T>
    T* getIf(RName name) { return std::get_if<T>(&component_.at(name)); }
    template<typename T>
    const T* getIf(RName name) const { return std::get_if<T>(&component_.at(name)); }

    template<typename T>
    T& add(RName name, T value)
    {
        return std::get<T>(component_[name] = std::move(value));
    }
    template<typename T, typename ...Args>
    T& add(RName name, Args&& ... args)
    {
        return std::get<T>(component_[name] = T(std::forward<Args>(args)...));
    }

    void remove(RName name) { component_.erase(name); }
    void clear() { component_.clear(); }

private:
    RDict<Component> component_;
};

} // ns Redopera

#endif // RENTITY_H
