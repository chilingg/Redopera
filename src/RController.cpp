#include "RController.h"
#include "RInputModule.h"
#include "RDebug.h"

using namespace Redopera;

template class Redopera::RSignal<>;

std::mutex RController::mutex;
RController::Status RController::globalState(RController::Status::Normal);

void RController::terminateTree(RController::Status status)
{
    Status s = status != Status::Error ? Status::Finished : Status::Error;
    globalState = s;

    std::lock_guard<std::mutex> guard(mutex);
    Root root = controllerRoot();
    for(auto node : *root)
        node->state_ = s;
}

RController::RController(void *holder):
    controlFunc([]{}),
    execFunc(std::bind(&RController::defaultExecFunc, this)),
    translateFunc(std::bind(&RController::translationToChildren, this, std::placeholders::_1)),
    inputFunc([](InputEvent){}),
    closeFunc([](CloseEvent&){}),
    startFunc([](StartEvent){}),
    finishFunc([](FinishEvent){}),
    state_(Status::Normal),
    parent_(nullptr),
    holder_(holder)
{
    addNodeToRoot(this);
}

RController::~RController()
{
    freeAllChild();
    if(parent_)
        parent_->children_.erase(this);
    else
        deleteNodeToRoot(this);
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

bool RController::isReady() const
{
    return state_ == Status::Normal || state_ == Status::Finished;
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

void RController::setTranslateFunc(std::function<void (const TransEvent &)> func)
{
    translateFunc = func;
}

void RController::setInputFunc(std::function<void (InputEvent)> func)
{
    inputFunc = func;
}

void RController::setCloseFunc(std::function<void (CloseEvent &)> func)
{
    closeFunc = func;
}

void RController::setStartFunc(std::function<void (StartEvent)> func)
{
    startFunc = func;
}

void RController::setFinishFunc(std::function<void (FinishEvent)> func)
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
    else
        deleteNodeToRoot(this);

    parent_ = parent;

    Status s;
    if(parent_)
    {
        s = parent_->state_;
        parent_->children_.insert(this);
    }
    else {
        s = globalState;
        addNodeToRoot(this);
    }

    if(s != state_)
    {
        if(s == Status::Looping)
        {
            StartEvent e(parent_);
            dispatchEvent(e);
        }
        else if(state_ == Status::Looping)
        {
            state_ = s;
            FinishEvent e(parent_);
            dispatchEvent(e);
        }
    }
}

void RController::activeOnce()
{
    for(auto it = children_.begin(); it != children_.end();)
    {
        if((*it)->isValid())
            (*it)->activeOnce();
        else
        {
            (*it++)->changeParent(nullptr);
            continue;
        }
         ++it;
    }
    control();
}

void RController::control()
{
    controlFunc();
}

void RController::translation(const TransEvent &info)
{
    translateFunc(info);
}

void RController::translationToChildren(const TransEvent &info)
{
    for(auto node : children_)
        node->translation(info);
}

void RController::dispatchEvent(InputEvent info)
{
    for(auto node : children_)
        node->dispatchEvent(info);
    inputFunc(info);
}

void RController::dispatchEvent(CloseEvent &event)
{
    for(auto node : children_)
        node->dispatchEvent(event);
    closeFunc(event);
}

void RController::dispatchEvent(StartEvent event)
{
    if(!isReady()) return;//错误状态无法进入循环

    state_ = Status::Looping;
    for(auto node : children_)
        node->dispatchEvent(event);
    startFunc(event);
}

void RController::dispatchEvent(FinishEvent event)
{
    for(auto node : children_)
    {
        node->state_ = state_.load();
        node->dispatchEvent(event);
    }
    finishFunc(event);
}

int RController::exec()
{
    return execFunc();
}

RController::Status RController::loopingCheck()
{
    if(state_ == Status::Finished)
    {
        CloseEvent e(this);
        dispatchEvent(e);
        if(e.stop)
            state_ = Status::Looping;
    }

    return state_;
}

void RController::breakLoop()
{
    Status loop = Status::Looping;
    state_.compare_exchange_strong(loop, Status::Finished);
}

void RController::setStatus(RController::Status status)
{
    state_ = status;
}

RController::Root RController::controllerRoot()
{
    static std::set<RController*> ROOT;
    return &ROOT;
}

int RController::defaultExecFunc()
{
    StartEvent sEvent(this);
    dispatchEvent(sEvent);

    while(loopingCheck() == Status::Looping)
        activeOnce();

    FinishEvent fEvent(this);
    dispatchEvent(fEvent);
    closed.emit();

    if(check(state_ == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void RController::addNodeToRoot(RController *node)
{
    std::lock_guard<std::mutex> guard(mutex);
    controllerRoot()->insert(node);
}

void RController::deleteNodeToRoot(RController *node)
{
    std::lock_guard<std::mutex> guard(mutex);
    controllerRoot()->erase(node);
}
