#ifndef RNAME_H
#define RNAME_H

#include <functional>
#include <array>
#include <string>
#include <cstring>
#include <mutex>
#include <unordered_map>

namespace Redopera {

class RName
{
    using NameInfo = std::string;

public:
    RName():
        index_(0)
    {}

    RName(const std::string &name):
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

    size_t hash() const { return index_; }
    size_t index() const { return index_; }

    std::string toString() const
    {
        std::lock_guard lock(gMutex);
        return gTable[index_];
    }

private:
    static size_t nameToIndex(const std::string &name)
    {
        static const size_t P = 19249, MOD = 3221225473ul;

        NameInfo info(name);

        size_t hash = 0;
        std::for_each_n(info.data(), info.size(), [&hash](char& c)
        {
            size_t value;
            if('0' <= c && c <= '9')
            {
                value = c - 48 + 27;
            }
            else if('A' <= c && c <= 'Z')
            {
                c += 32;
                value = c - 96;
            }
            else if('a' <= c && c <= 'z')
            {
                value = c - 96;
            }
            else
            {
                c = '_';
                value = 37;
            }

            hash = (hash * P + value) % MOD;
        });

        std::lock_guard lock(gMutex);
        auto pair = indexTable.equal_range(hash);
        if(pair.first != indexTable.end())
        {
            for(auto it = pair.first; it != pair.second; ++it)
            {
                if(gTable[it->second] == info)
                    return it->second;
            }
        }

        gTable.push_back(std::move(info));
        size_t index = gTable.size() - 1;
        indexTable.emplace(hash, index);
        return index;
    }

    static std::vector<NameInfo> gTable;
    static std::unordered_multimap<size_t, size_t> indexTable;
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

#endif // RNAME_H
