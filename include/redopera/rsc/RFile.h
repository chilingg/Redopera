#ifndef RFILE_H
#define RFILE_H

#include "../RFormat.h"
#include <fstream>

namespace Redopera {

namespace RFile {

static std::unique_ptr<RData[]> load(const std::string &path, size_t *fsize = nullptr)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(rCheck(!file.is_open(), "Failed to load file in {}!\n", path))
    {
        if(fsize)
            *fsize = 0;
        return nullptr;
    }

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto data = std::make_unique<RData[]>(size);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.read(reinterpret_cast<char*>(data.get()), size);
        file.close();
    }
    catch(std::ifstream::failure &f)
    {
        rError("Wrong access file {}: {}\n", path, f.what());
        return nullptr;
    }

    if(fsize)
        *fsize = size;
    return data;
}

} // ns RFile

} // Redopera ns

#endif
