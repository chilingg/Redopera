#ifndef RFILE_H
#define RFILE_H

#include "RResource.h"

namespace Redopera {

struct RFile
{
    enum class Mode
    {
        Write,
        Append
    };

    static RFile load(const std::string &path)
    {
        auto rpath = RResource::rscPath(path);
        RFile file;

        FILE *fp = fopen(rpath.c_str(), "rb");
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

    static bool save(const std::string &path, const RFile& file, Mode mode = Mode::Write)
    {
        auto rpath = RResource::rscPath(path);

        FILE *fp = fopen(rpath.c_str(), mode == Mode::Write ? "wb" : "ab");
        if (fp)
        {
            bool b = fwrite(file.data.get(), 1, file.size, fp) == file.size;
            fclose(fp);

            return b;
        }

        return false;
    }

    size_t size = 0;
    std::unique_ptr<RData[]> data;
};

} // Redopera ns

#endif
