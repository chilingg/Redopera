#include <iostream>
#include <thread/RThreadPool.h>
#include <RTimer.h>

using namespace Redopera;

bool primeNum(unsigned n)
{
    for(unsigned i = 2; i < n; ++i)
        if(n % i == 0)
            return false;
    return true;
}

std::atomic_int count;

int main()
{
    RStopwatch timer;
    unsigned tNum = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() : 1;

    std::cout << "\n======== Multithread Test ========\n\n"
              "Count prime number in 0~50000: \n";

    std::cout << "Using single thread...\n";

    timer.start();
    count = 0;
    for(unsigned i = 0; i < 50000; ++i)
        if(primeNum(i)) ++count;

    std::cout << "Result: " + std::to_string(count)
              + " Time: " + std::to_string(timer.elapsed()) + "ms\n";

    std::cout << "Using thread poll calculate single tasks(50000)... "
           << "Threads: " + std::to_string(tNum) << '\n';

    timer.start();
    count = 0;
    RThreadPool pool; // RThreadPool pool(tNum);
    for(unsigned i = 0; i < 50000; ++i)
    {
        RFunction<void()> f([i]{
            if(primeNum(i)) ++count;
        });
        pool.submit(std::move(f));
    }
    while(!pool.isIdle()) // isIdle()只是查询任务栈是否为空
        std::this_thread::yield();
    pool.waitingForDone();

    std::cout << "Result: " + std::to_string(count)
              + " Time: " + std::to_string(timer.elapsed()) + "ms\n";

    std::cout << "Using thread poll calculate blocked tasks... "
           << "Threads: " + std::to_string(tNum)
              + " Block: " + std::to_string(50000 / 100) << '\n';

    timer.start();
    pool.start();
    count = 0;
    unsigned block = 50000 / 100;
    std::array<std::future<int>, 100> future;
    for(unsigned i = 0; i < 100; ++i)
    {
        RFunction<int()> f([i, block]{
            unsigned begin = i * block;
            unsigned end = begin + block > 50000 ? 50000 : begin + block;
            int num = 0;
            while(begin < end)
            {
                if(primeNum(begin)) ++num;
                ++begin;
            }
            return num;
        });
        future[i] = pool.submit(std::move(f));
    }
    for(auto &f : future)
        count += f.get();
    pool.waitingForDone();

    std::cout << "Result: " + std::to_string(count)
              + " Time: " + std::to_string(timer.elapsed()) + "ms\n";

    std::cout << "Using multithread calculate blocked tasks... "
           << "Threads: " + std::to_string(tNum)
              + " Block: " + std::to_string(50000 / tNum)<< '\n';

    timer.start();
    count = 0;
    std::vector<RThread> threads;
    threads.reserve(tNum);

    block = 50000 / tNum;
    for(unsigned i = 0; i < tNum; ++i)
    {
        threads.emplace_back([i, block]{
            unsigned begin = i * block;
            unsigned end = begin + block > 50000 ? 50000 : begin + block;
            while(begin < end)
            {
                if(primeNum(begin)) ++count;
                ++begin;
            }
        });
    }
    for(auto &t : threads)
        t.join();

    std::cout << "Result: " + std::to_string(count)
              + " Time: " + std::to_string(timer.elapsed()) + "ms\n";

    return 0;
}
