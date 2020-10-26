#ifndef REVENT_H
#define REVENT_H

#include "RPoint.h"
#include "RSize.h"

#include <map>
#include <string>

class GLFWwindow;

namespace Redopera {

class RController;
class RInputModule;

// 事件通知类集合 ********************

// 循环开始事件
class StartEvent
{
public:
    explicit StartEvent(RController *sender):
        sender(sender) {}
    RController *sender;
};

// 循环结束事件
class FinishEvent
{
public:
    explicit FinishEvent(RController *sender):
        sender(sender) {}
    RController *sender;
};

// 申请关闭事件
class CloseEvent
{
public:
    explicit CloseEvent(RController *sender):
        sender(sender) {}
    RController *sender;
    bool stop = false; // true驳回关闭申请
};

struct TransEvent
{
    RController *sender;
    const RSize size;
    const RPoint pos = RPoint(0);
};

struct ProcessEvent
{
    ProcessEvent(RController *sender, RInputModule* input):
        sender(sender), input(input) {}

    void addInstruct(const std::string instruct, int value = 1)
    {
        instructs_.emplace(instruct, value);
    }

    int instruct(const std::string &instruct) const
    {
        auto it = instructs_.find(instruct);

        if (it == instructs_.end())
            return 0;
        else
            return it->second;
    }

    RController *sender;
    RInputModule *input;

private:
    std::map<std::string, int> instructs_;
};

} // Redopera

#endif // REVENT_H
