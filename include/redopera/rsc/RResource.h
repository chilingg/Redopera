#ifndef RRESOURCE_H
#define RRESOURCE_H

#include <string>
#include <memory>

namespace Redopera {

using RData = uint8_t;

struct RFile
{
    size_t size = 0;
    std::unique_ptr<RData[]> data;
};

class RResource
{
public:
    RResource() = delete;

    static RFile loadFile(std::string path)
    {
        RResource::rscPath(path);
        RFile file;

        FILE *fp = fopen(path.c_str(), "rb");
        if (fp)
        {
            fseek(fp, 0L, SEEK_END);
            auto size = ftell(fp);
            fseek(fp, 0L, SEEK_SET);

            file.data = std::make_unique<RData[]>(size);
            fread(file.data.get(), 1, size, fp);
            fclose(fp);
        }

        return file;
    }

    static void rscPath(std::string &path)
    {
        if(path[0] == ':' && path[1] == '/')
            path = rscPath_ + path.substr(2, path.size());
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
