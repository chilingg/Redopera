#include <RDebug.h>
#include <RName.h>
#include <RTimer.h>
#include <unordered_map>
#include <RColor.h>

using namespace Redopera;

int main()
{
    size_t num = 1000000;
    size_t count = 0;
    RTimer timer;
    srand(timer.elapsed());

    std::unordered_map<size_t, int> table3;
    std::vector<size_t> names3;
    timer.start();
    while(table3.size() < num)
    {
        size_t n = std::rand();

        if(!table3.count(n))
        {
            names3.push_back(n);
            table3.emplace(n, n);
        }
    }
    rDebug << "Construction size_t table:\t" << timer.elapsed();

    count = 0;
    timer.start();
    for(size_t i = 0; i < num; ++i)
    {
        count += table3.at(names3[i]);
    }
    rDebug << "Access size_t table:\t" << timer.elapsed();
    rDebug << count;

    std::unordered_map<RName, int> table1;
    std::vector<RName> names1;
    timer.start();
    for(size_t i = 0; i < names3.size(); ++i)
    {
        size_t n = names3[i];

        RName name = std::to_string(n);
        if(!table1.count(name))
        {
            names1.push_back(name);
            table1.emplace(names1.back(), n);
        }
    }
    rDebug << "Construction RName table:\t" << timer.elapsed();

    count = 0;
    timer.start();
    for(size_t i = 0; i < num; ++i)
    {
        count += table1.at(names1[i]);
    }
    rDebug << "Access RName table:\t" << timer.elapsed();
    rDebug << count;

    std::unordered_map<std::string, int> table2;
    std::vector<std::string> names2;
    timer.start();
    for(size_t i = 0; i < names3.size(); ++i)
    {
        size_t n = names3[i];

        std::string name = std::to_string(n);
        if(!table2.count(name))
        {
            names2.push_back(name);
            table2.emplace(name, n);
        }
    }
    rDebug << "Construction string table:\t" << timer.elapsed();

    count = 0;
    timer.start();
    for(size_t i = 0; i < num; ++i)
    {
        count += table2.at(names2[i]);
    }
    rDebug << "Access String table:\t" << timer.elapsed();
    rDebug << count;

    return 0;
}
