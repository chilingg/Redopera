#ifndef RCONTROLLER_H
#define RCONTROLLER_H

#include "REvent.h"
#include "RSigslot.h"
#include "RSize.h"
#include "RPoint.h"

#include <set>
#include <atomic>

namespace Redopera {

class RController final
{
    using Root = std::set<RController*>*;

public:
    enum class Status
    {
        Normal,
        Looping,
        Finished,
        Error,
    };

    static void terminateTree(Status status = Status::Finished);

    explicit RController(RController *parent = nullptr, void *holder = nullptr,
                         std::function<void(StartEvent)> sEvent = [](StartEvent){});
    ~RController();

    RController(RController &ctl) = delete;
    RController& operator=(const RController &ctl) = delete;

    bool isLooping() const;
    bool isNormal() const;
    bool isValid() const;
    bool isReady() const;
    bool isChild(RController *child) const;
    bool isAncestor(RController *node) const; // 祖辈
    bool isFree() const;
    Status status() const;
    int getChildrenSize() const;
    RController* getParent() const;
    void* getHolder() const;

    void setControlFunc(std::function<void()> func);
    void setExecFunc(std::function<int()> func);
    void setTranslateFunc(std::function<void(const TransEvent&)> func);
    void setInputFunc(std::function<void(InputEvent)> func);
    void setCloseFunc(std::function<void(CloseEvent&)> func);
    void setStartFunc(std::function<void(StartEvent)> func);
    void setFinishFunc(std::function<void(FinishEvent)> func);

    void addChild(RController *child);
    void freeChild(RController *child);
    void freeAllChild();
    void changeParent(RController *parent);

    void activeOnce();
    void control();
    void translation(const TransEvent &info);
    void translationToChildren(const TransEvent &info);
    void dispatchEvent(InputEvent info);

    // 事件发布接口 PS:深度优先、由下至上
    // 已由RController负责调用
    void dispatchEvent(CloseEvent &event);
    void dispatchEvent(StartEvent event);
    void dispatchEvent(FinishEvent event);

    int exec();

    // 若状态是Finishi则发起closeEvent
    Status loopingCheck();
    void breakLoop();
    void setStatus(Status status);

    RSignal<> closed;

private:
    static Root controllerRoot();

    int defaultExecFunc();

    static std::mutex mutex;
    static Status globalState;

    void addNodeToRoot(RController *node);
    void deleteNodeToRoot(RController *node);

    std::function<void()> controlFunc;
    std::function<int()> execFunc;
    std::function<void(const TransEvent &event)> translateFunc;
    std::function<void(InputEvent event)> inputFunc;
    std::function<void(CloseEvent &event)> closeFunc;    //尝试终止exec循环
    std::function<void(StartEvent event)> startFunc;    //exec循环开始
    std::function<void(FinishEvent event)> finishFunc;  //exec循环终止

    std::atomic<Status> state_;
    std::set<RController*> children_;
    RController *parent_ = nullptr;
    void *holder_;

    _RSLOT_TAIL_
};

} // Redopera

#endif // RCONTROLLER_H
