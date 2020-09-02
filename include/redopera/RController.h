#ifndef RCONTROLLER_H
#define RCONTROLLER_H

#include "RSigslot.h"

#include <set>
#include <atomic>

namespace Redopera {

class TransInfo;
class InputInfo;
class CloseEvent;
class StartEvent;
class FinishEvent;

class RController final
{
public:
    enum class Status
    {
        Normal,
        Looping,
        Error,
    };

    explicit RController(void *holder = nullptr);
    ~RController();

    RController(RController &ctl) = delete;
    RController& operator=(const RController &ctl) = delete;

    bool isLooping() const;
    bool isNormal() const;
    bool isValid() const;
    bool isChild(RController *child) const;
    bool isAncestor(RController *node) const; // 祖辈
    bool isFree() const;
    Status status() const;
    int getChildrenSize() const;
    RController* getParent() const;
    void* getHolder() const;

    void setControlFunc(std::function<void()> func);
    void setExecFunc(std::function<int()> func);
    void setTransFunc(std::function<void(TransInfo*)> func);
    void setInputFunc(std::function<void(InputInfo*)> func);
    void setCloseFunc(std::function<void(CloseEvent*)> func);
    void setStartFunc(std::function<void(StartEvent*)> func);
    void setFinishFunc(std::function<void(FinishEvent*)> func);

    void addChild(RController *child);
    void freeChild(RController *child);
    void freeAllChild();
    void changeParent(RController *parent);

    // 事件发布接口 PS:深度优先、由下至上
    // 已由RController负责调用
    void dispatchEvent(CloseEvent *event);
    void dispatchEvent(StartEvent *event);
    void dispatchEvent(FinishEvent *event);

    void activeOnce();
    void control();
    void translation(TransInfo *info);
    void inputProcess(InputInfo *info);

    int exec();

    // 若状态是Finishi则发起closeEvent
    Status loopingCheck();
    void breakLoop();
    void errorLoop();

    RSignal<> closed;

private:
    int defaultExecFunc();
    void defaultTransFunc(TransInfo *info);
    void defaultInputFunc(InputInfo *event);

    std::function<void()> controlFunc;
    std::function<int()> execFunc;
    std::function<void(TransInfo *event)> transFunc;
    std::function<void(InputInfo *event)> inputFunc;
    std::function<void(CloseEvent *event)> closeFunc;   //尝试终止exec循环
    std::function<void(StartEvent *event)> startFunc;   //exec循环开始
    std::function<void(FinishEvent *event)> finishFunc; //exec循环终止

    std::atomic<Status> state_;
    std::set<RController*> children_;
    RController *parent_ = nullptr;
    void *holder_;
};

} // Redopera

#endif // RCONTROLLER_H
