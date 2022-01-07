#ifndef RENTITY_H
#define RENTITY_H

#include <variant>
#include <unordered_map>

#include "RName.h"

namespace Redopera {

template<typename ...Types>
class REntity
{
public:
    using Variant = std::variant<Types ...>;
    using ComponentList = std::unordered_map<RName, Variant>;

    REntity() = default;
    REntity(const REntity &e) = default;
    REntity(REntity &&e) = default;
    REntity& operator=(const REntity &e) = default;
    REntity& operator=(REntity &&e) = default;

    size_t index(RName name) const { return component_.at(name).index(); }
    bool isComponent(RName name) const { return component_.contains(name); }
    const ComponentList& componentList() const { return component_; }
    size_t size() const { return component_.size(); }

    template<typename T>
    const T& get(RName name) const { return std::get<T>(component_.at(name)); }
    template<typename T>
    T& get(RName name){ return std::get<T>(component_.at(name)); }

    template<typename T>
    T* getIf(RName name) { return std::get_if<T>(&component_.at(name)); }
    template<typename T>
    const T* getIf(RName name) const { return std::get_if<T>(&component_.at(name)); }

    template<typename T>
    T& add(RName name, T&& value)
    {
        auto it = component_.emplace(name, std::forward<T>(value));
        return std::get<T>(it.first->second);
    }
    template<typename T, typename ...Args>
    T& add(RName name, Args&& ... value)
    {
        auto it = component_.emplace(name, T(std::forward<T>(value)...));
        return std::get<T>(it.first->second);
    }

    void remove(RName name) { component_.erase(name); }

private:
    ComponentList component_;
};

} // ns Redopera

#endif // RENTITY_H
