#ifndef RTHEADPOOL_H
#define RTHEADPOOL_H

#include "RThreadStack.h"
#include "RThread.h"
#include "RFunction.h"

#include <atomic>
#include <future>
#include <vector>

namespace Redopera {

class RThreadPool
{
public:
    RThreadPool(int tNum = 0);
    ~RThreadPool();
    RThreadPool(const RThreadPool&) = delete;
    RThreadPool& operator=(const RThreadPool&) = delete;

    bool isIdle() const;
    int threadNumber() const;

    template<typename FuncType>
    std::future<typename std::result_of<FuncType()>::type> submit(FuncType f)
    {
        typedef typename std::result_of<FuncType()>::type resultType;
        std::packaged_task<resultType()> task(std::move(f));
        std::future<resultType> res(task.get_future());

        for(auto &stack : stacks_)
        {
            if(stack->empty())
            {
                stack->push(std::move(task));
                return res;
            }
        }
        //若无空闲线程，则轮次循环递交
        stacks_[++index_ % threads_.size()]->push(std::move(task));
        return res;
    }

    bool runOneTask(); //主动扒拉一个任务到当前线程处理

    void waitingForDone();
    void start(int tNum = 0);

private:
    void workerThread(RThreadStack<RFunction<void()>> *stack);

    std::atomic_bool done_;
    std::atomic_uint index_;
    std::vector<std::unique_ptr<RThreadStack<RFunction<void()>>>> stacks_; // 线程任务栈
    std::vector<RThread> threads_;
};

} // Redopera

#endif // RTHEADPOOL_H
