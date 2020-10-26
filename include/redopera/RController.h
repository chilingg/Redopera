#ifndef RCONTROLLER_H
#define RCONTROLLER_H

#include "RSigslot.h"

#include <set>
#include <atomic>

namespace Redopera {

class TransEvent;
class ProcessEvent;
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

    RController();
    RController(const std::string &name, void* holder);
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
    const std::string name() const;
    const std::string path() const;
    RController* node(const std::string &path);
    RController* root();

    template<typename T>
    T* holder() const
    {
        return reinterpret_cast<T*>(holder_);
    }

    void setControlFunc(std::function<void()> func);
    void setExecFunc(std::function<int()> func);
    void setTransFunc(std::function<void(TransEvent*)> func);
    void setProcessFunc(std::function<void(ProcessEvent*)> func);
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
    void translation(TransEvent *event);
    void process(ProcessEvent *event);

    void defaultTransFunc(TransEvent *event);
    void defaultProcessFunc(ProcessEvent *event);

    int exec();

    // 若状态是Finishi则发起closeEvent
    Status loopingCheck();
    void breakLoop();
    void errorLoop();

    RSignal<> closed;

private:
    int defaultExecFunc();

    std::function<void()> controlFunc;
    std::function<int()> execFunc;
    std::function<void(TransEvent *event)> transFunc;
    std::function<void(ProcessEvent *event)> inputFunc;
    std::function<void(CloseEvent *event)> closeFunc;   //尝试终止exec循环
    std::function<void(StartEvent *event)> startFunc;   //exec循环开始
    std::function<void(FinishEvent *event)> finishFunc; //exec循环终止

    std::string name_;
    std::atomic<Status> state_;
    std::set<RController*> children_;
    RController *parent_ = nullptr;
    void *holder_;
};

} // Redopera

#define _RCTRL_DECLARE_ public:Redopera::RController* ctrl(){ return &ctrl_; } private:Redopera::RController ctrl_;

#endif // RCONTROLLER_H
