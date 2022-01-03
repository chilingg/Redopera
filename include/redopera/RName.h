#ifndef RNAME_H
#define RNAME_H

#include <vector>
#include <string>
#include <string_view>
#include <mutex>
#include <unordered_map>

namespace Redopera {

class RName
{
    using NameInfo = std::string;

public:
    RName(const std::string &name):
        index_(nameToIndex(name))
    {}

    RName(std::string_view name):
        index_(nameToIndex(name))
    {}

    RName(const char *name):
        index_(nameToIndex(name))
    {}

    RName(const RName &) = default;
    RName(RName &&) = default;
    RName& operator=(const RName&) = default;
    RName& operator=(RName&&) = default;

    bool operator==(const RName &other) const { return index_ == other.index_; }
    bool operator!=(const RName &other) const { return index_ != other.index_; }

    size_t hash() const { return index_; }

    std::string_view string() const
    {
        std::lock_guard lock(gMutex);
        return gTable()[index_];
    }

private:
    static std::vector<NameInfo>& gTable()
    {
        static std::vector<NameInfo> table;
        return table;
    }

    static std::unordered_multimap<size_t, size_t>& indexTable()
    {
        static std::unordered_multimap<size_t, size_t> table;
        return table;
    }

    static size_t nameToIndex(std::string_view name)
    {
        size_t hash = std::hash<std::string_view>()(name);

        std::lock_guard lock(gMutex);
        auto pair = indexTable().equal_range(hash);
        if(pair.first != indexTable().end())
        {
            for(auto it = pair.first; it != pair.second; ++it)
            {
                if(gTable()[it->second] == name)
                    return it->second;
            }
        }

        gTable().emplace_back(name);
        size_t index = gTable().size() - 1;
        indexTable().emplace(hash, index);
        return index;
    }

    static std::mutex gMutex;

    size_t index_;
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

#ifdef REDOPERA_DEFINE_FILE
Redopera::RName operator "" _rname(const char *str, std::size_t) { return Redopera::RName(str); }
std::mutex Redopera::RName::gMutex;
#endif

#endif // RNAME_H
