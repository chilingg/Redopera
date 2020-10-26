#ifndef RRESOURCE_H
#define RRESOURCE_H

#include <string>
#include <memory>

namespace Redopera {

using RData = uint8_t;

class RResource
{
public:
    RResource() = delete;

    static void rscPath(std::string &path)
    {
        if(path[0] == ':' && path[1] == '/')
            path = rscPath_ + path.substr(2, path.size());
    }

    static std::string rscPath(std::string &&path)
    {
        if(path[0] == ':' && path[1] == '/')
            path = rscPath_ + path.substr(2, path.size());

        return path;
    }

    static void setResourcePath(const std::string &path)
    {
        rscPath_ = path;
    }

    static const std::string& getResourcePath()
    {
        return rscPath_;
    }

private:
    static std::string rscPath_;
};

} // Redopera

#endif // RRESOURCE_H
