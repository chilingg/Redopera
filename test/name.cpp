#include <RName.h>
#include <RTimer.h>
#include <fmt/core.h>
#include <unordered_map>
#include <random>
#include <functional>

using namespace Redopera;

class TestName
{
    static constexpr size_t num = 50000;

public:
    TestName():
        rengine(std::time(0)),
        values(num)
    {
        std::fill_n(values.begin(), num, rengine());
    }

    template <typename Key>
    void constructAndAccessTest(size_t &ct, size_t &at, std::function<Key(unsigned)> f)
    {
        std::unordered_map<Key, int> table;

        std::vector<Key> names;
        timer.start();
        while(table.size() < num)
        {
            Key n = f(rengine());
            if(!table.contains(n))
            {
                table.emplace(n, values[table.size()]);
                names.push_back(n);
            }
        }
        ct = timer.elapsed();
        //fmt::print("{1:8d}:Construction {0} table\n", typeid(Key).name(), ct);

        size_t count = 0;
        timer.start();
        for(size_t i = 0; i < num; ++i)
        {
            count += table[names[values[i] % num]];
        }
        at = timer.elapsed();
        //fmt::print("{1:8d}:Access {0} table\n count = {2} \n", typeid(Key).name(), at, count);
    }

    std::default_random_engine rengine;
    std::vector<unsigned> values;
    RStopwatchMS timer;
};

int main()
{
    TestName t;
    size_t sizeC, sizeA, strC, strA, nameC, nameA;

    t.constructAndAccessTest<size_t>(sizeC, sizeA, [](unsigned n){ return n; });
    t.constructAndAccessTest<std::string>(strC, strA, [](unsigned n){ return std::to_string(n*n*n); });
    t.constructAndAccessTest<RName>(nameC, nameA, [](unsigned n){ return std::to_string(n*n*n); });

    t.constructAndAccessTest<RName>(nameC, nameA, [](unsigned n){ return std::to_string(n*n*n); });
    t.constructAndAccessTest<std::string>(strC, strA, [](unsigned n){ return std::to_string(n*n*n); });
    t.constructAndAccessTest<size_t>(sizeC, sizeA, [](unsigned n){ return n; });

    double total = sizeC + strC + nameC;
    fmt::print("Ratio of Construct: {:.3} : {:.3} : {:.3}\n", sizeC / total, strC / total, nameC / total);
    total = sizeA + strA + nameA;
    fmt::print("Ratio of Access: {:.3} : {:.3} : {:.3}\n", sizeA / total, strA / total, nameA / total);

    return 0;
}
