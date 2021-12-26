#include <thread/RThreadPool.h>
#include <assert.h>

using namespace Redopera;

RThreadPool::RThreadPool(int tNum):
    done_(false),
    index_(0)
{
    start(tNum);
}

RThreadPool::~RThreadPool()
{
    done_ = true;
}

bool RThreadPool::isRun() const
{
    return !done_;
}

bool RThreadPool::isIdle() const
{
    for(auto &stack : stacks_)
    {
        if(!stack->empty())
            return false;
    }
    return true;
}

int RThreadPool::threadNumber() const
{
    return threads_.size();
}

bool RThreadPool::runOneTask()
{
    RFunction<void()> task;
    for(auto &stack : stacks_)
    {
        if(stack->tryPop(task))
        {
            task();
            return true;
        }
    }
    return false; //线程池任务为空
}

void RThreadPool::waitingForDone()
{
    done_ = true;
    {
    std::vector<RThread> temp;
    temp.swap(threads_);
    }
}

void RThreadPool::start(int tNum)
{
    done_ = false;

    if(tNum < 1)
        tNum = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() : 1;
    assert(tNum > 0);

    stacks_.reserve(tNum);
    threads_.reserve(tNum);
    try {
        for(int i = 0; i < tNum; ++i)
        {
            stacks_.emplace_back(std::make_unique<RThreadStack<RFunction<void()>>>());
            threads_.emplace_back(&RThreadPool::workerThread, this, stacks_[i].get());
        }
    }
    catch(...) {
        done_ = true;
        throw;
    }
}

void RThreadPool::workerThread(RThreadStack<RFunction<void ()> > *stack)
{
    while(!done_)
    {
        RFunction<void()> task;
        if(stack->tryPop(task))
            task();
        else if(!runOneTask())
            std::this_thread::yield();
    }
}
