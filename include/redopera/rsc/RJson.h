#ifndef RJSON_H
#define RJSON_H

#include <nlohmann/json.hpp>

namespace Redopera {

nlohmann::json loadJSON(const std::string &path, std::string *error);

}

#ifdef REDOPERA_DEFINE_FILE
#include <fstream>
nlohmann::json Redopera::loadJSON(const std::string &path, std::string *error)
{
    std::fstream f(path);
    nlohmann::json j;

    if(f.is_open())
    {
        try
        {
            f >> j;
        }
        catch(nlohmann::json::exception &e)
        {
            if(error)
                *error = e.what();
        }
    }

    return j;
}
#endif

#endif // RJSON_H
