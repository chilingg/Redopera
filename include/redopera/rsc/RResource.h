#ifndef RRESOURCE_H
#define RRESOURCE_H

#include <string>

namespace Redopera {

using RData = uint8_t;

class RResource
{
public:
    RResource() = delete;

    static void rscpath(std::string &path)
    {
        if(path[0] == ':' && path[1] == '/')
            path = rscPath + path.substr(2, path.size());
    }

    static void setResourcePath(const std::string &path)
    {
        rscPath = path;
    }

    static const std::string& getResourcePath()
    {
        return rscPath;
    }

private:
    static std::string rscPath;
};

} // Redopera

#endif // RRESOURCE_H
