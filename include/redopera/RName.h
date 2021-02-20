#ifndef RNAME_H
#define RNAME_H

#include <memory>
#include <functional>
#include <string>

namespace Redopera {

class RName
{
public:
    RName(const std::string &name):
        length_(name.size()),
        hash_(std::hash<std::string>{}(name)),
        name_(std::make_shared<const std::string>(name))
    {}

    RName(const RName &) = default;
    RName(RName &&) = default;
    RName& operator=(const RName&) = default;
    RName& operator=(RName&&) = default;

    bool operator==(const RName &other) const { return hash_ == other.hash_ && length_ == other.length_; }

    size_t hash() const { return hash_; }
    size_t length() const { return length_; }
    const std::string& toString() const { return *name_; }

private:
    size_t length_;
    size_t hash_;
    std::shared_ptr<const std::string> name_;
};

} // ns Redopera

namespace std
{
    template<> struct hash<Redopera::RName>
    {
        typedef Redopera::RName argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type &name) const
        {
            result_type const hash = name.hash();
            return hash;
        }
    };
}

#endif // RNAME_H
