#include <RController.h>
#include <REvent.h>
#include <RInputModule.h>
#include <RDebug.h>

using namespace Redopera;

template class Redopera::RSignal<>;

RController::RController(void *holder):
    controlFunc([]{}),
    execFunc(std::bind(&RController::threadExecFunc, this)),
    translateFunc(std::bind(&RController::defaultTransFunc, this, std::placeholders::_1)),
    inputFunc([](InputEvent*){}),
    closeFunc([](CloseEvent*){}),
    startFunc([](StartEvent*){}),
    finishFunc([](FinishEvent*){}),
    state_(Status::Normal),
    parent_(nullptr),
    holder_(holder)
{

}

RController::~RController()
{
    freeAllChild();
    if(parent_)
        parent_->children_.erase(this);
}

bool RController::isLooping() const
{
    return state_ == Status::Looping;
}

bool RController::isNormal() const
{
    return state_ == Status::Normal;
}

bool RController::isValid() const
{
    return state_ == Status::Normal || state_ == Status::Looping;
}

bool RController::isChild(RController *child) const
{
    return children_.count(child);
}

bool RController::isAncestor(RController *node) const
{
    if(!parent_)
        return false;
    else if (parent_ != node)
        return parent_->isAncestor(node);
    else
        return true;
}

bool RController::isFree() const
{
    return !parent_;
}

RController::Status RController::status() const
{
    return state_;
}

int RController::getChildrenSize() const
{
    return children_.size();
}

RController *RController::getParent() const
{
    return parent_;
}

void *RController::getHolder() const
{
    return holder_;
}

void RController::setControlFunc(std::function<void ()> func)
{
    controlFunc = func;
}

void RController::setExecFunc(std::function<int ()> func)
{
    execFunc = func;
}

void RController::setTransFunc(std::function<void (TransInfo*)> func)
{
    translateFunc = func;
}

void RController::setInputFunc(std::function<void (InputEvent*)> func)
{
    inputFunc = func;
}

void RController::setCloseFunc(std::function<void (CloseEvent*)> func)
{
    closeFunc = func;
}

void RController::setStartFunc(std::function<void (StartEvent*)> func)
{
    startFunc = func;
}

void RController::setFinishFunc(std::function<void (FinishEvent*)> func)
{
    finishFunc = func;
}

void RController::addChild(RController *child)
{
    if(check(!child || child == this || isAncestor(child), "Failure to add child!"))
        return;

    child->changeParent(this);
}

void RController::freeChild(RController *child)
{
    if(children_.count(child))
        child->changeParent(nullptr);
}

void RController::freeAllChild()
{
    while(!children_.empty())
    {
        auto c = children_.begin();
        (*c)->changeParent(nullptr);
    }
}

void RController::changeParent(RController *parent)
{
    if(parent_)
        parent_->children_.erase(this);

    parent_ = parent;

    Status s;
    if(parent_)
    {
        s = parent_->state_;
        parent_->children_.insert(this);
    }
    else {
        s = Status::Normal;
    }

    if(s != state_)
    {
        if(s == Status::Looping)
        {
            StartEvent e(parent_);
            dispatchEvent(&e);
        }
        else if(state_ == Status::Looping)
        {
            state_ = s;
            FinishEvent e(parent_);
            dispatchEvent(&e);
        }
    }
}

void RController::dispatchEvent(InputEvent *info)
{
    for(auto node : children_)
        node->dispatchEvent(info);
    inputFunc(info);
}

void RController::dispatchEvent(CloseEvent *event)
{
    for(auto node : children_)
        node->dispatchEvent(event);
    closeFunc(event);
}

void RController::dispatchEvent(StartEvent *event)
{
    if(state_ == Status::Error) return;//错误状态无法进入循环

    state_ = Status::Looping;
    for(auto node : children_)
        node->dispatchEvent(event);
    startFunc(event);
}

void RController::dispatchEvent(FinishEvent *event)
{
    for(auto node : children_)
    {
        node->state_ = state_.load();
        node->dispatchEvent(event);
    }
    finishFunc(event);
}

void RController::activeOnce()
{
    std::for_each(children_.begin(), children_.end(), std::mem_fn(&RController::activeOnce));
    control();
}

void RController::control()
{
    controlFunc();
}

void RController::translation(TransInfo *info)
{
    translateFunc(info);
}

int RController::exec()
{
    return execFunc();
}

RController::Status RController::loopingCheck()
{
    if (state_ != Status::Looping)
    {
        CloseEvent e(this);
        dispatchEvent(&e);
        if(e.stop && state_ == Status::Normal)
            state_ = Status::Looping;
    }

    return state_;
}

void RController::breakLoop()
{
    if (parent_)
        parent_->breakLoop();
    else {
        Status loop = Status::Looping;
        state_.compare_exchange_strong(loop, Status::Normal);
    }
}

void RController::errorLoop()
{
    if (parent_)
        parent_->errorLoop();
    else {
        Status loop = Status::Looping;
        state_.compare_exchange_strong(loop, Status::Error);
    }
}

void RController::setAsMainCtrl()
{
    execFunc = std::bind(&RController::defaultExecFunc, this);
}

void RController::pushFuncToThreadExec(std::function<void()> func)
{
    funcs_.push(func);
}

int RController::defaultExecFunc()
{
    StartEvent sEvent(this);
    dispatchEvent(&sEvent);

    while(loopingCheck() == Status::Looping)
        activeOnce();

    FinishEvent fEvent(this);
    dispatchEvent(&fEvent);
    closed.emit();

    if(check(state_ == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int RController::threadExecFunc()
{
    StartEvent sEvent(this);
    dispatchEvent(&sEvent);

    while(loopingCheck() == Status::Looping)
    {
        std::function<void()> func;
        for(int i = 0; i < 10; ++i)
        {
            if (funcs_.tryPop(func))
                func();
            else
                break;
        }
        activeOnce();
    }

    FinishEvent fEvent(this);
    dispatchEvent(&fEvent);
    closed.emit();

    if(check(state_ == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void RController::defaultTransFunc(TransInfo *info)
{
    for(auto node : children_)
        node->translation(info);
}
