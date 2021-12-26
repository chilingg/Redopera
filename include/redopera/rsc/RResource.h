#ifndef RRESOURCE_H
#define RRESOURCE_H

#include <memory>

namespace Redopera {

namespace basic {

template<typename Data, typename Deleter>
class ResourceBasic
{
public:
    ResourceBasic() = default;

    ResourceBasic(Data *p, Deleter d):
        p_(p, std::move(d))
    {}

    ResourceBasic(const ResourceBasic &) = default;
    ResourceBasic& operator=(const ResourceBasic &) = default;

    ResourceBasic(ResourceBasic &&rsc): p_(std::move(rsc.p_)) {}
    ResourceBasic& operator=(ResourceBasic &&rsc) { p_ = std::move(rsc.p_); return *this; }

    bool isValid() const { return p_.operator bool(); };
    Data* data()const { return p_.get(); }
    void free() { p_.reset(); }

protected:
    void swap(ResourceBasic &rsc) { p_.swap(rsc.p_); };

    void reset(Data *p, Deleter d) { p_.reset(p, std::move(d)); }
    void reset(const std::shared_ptr<Data> &p) { p_ = p; }

    std::shared_ptr<Data> p_;
};

} // basic

} // Redopera


#endif // RRESOURCE_H
