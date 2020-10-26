#include <RController.h>
#include <REvent.h>
#include <RInputModule.h>
#include <RDebug.h>

#include <regex>

using namespace Redopera;

template class Redopera::RSignal<>;

RController::RController():
    RController("", nullptr)
{

}

RController::RController(const std::string &name, void *holder):
    updataFunc([]{}),
    execFunc(std::bind(&RController::defaultExecFunc, this)),
    transFunc(std::bind(&RController::defaultTransFunc, this, std::placeholders::_1)),
    processFunc(std::bind(&RController::defaultProcessFunc, this, std::placeholders::_1)),
    closeFunc([](CloseEvent*){}),
    startFunc([](StartEvent*){}),
    finishFunc([](FinishEvent*){}),
    name_(name),
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

const std::string RController::name() const
{
    return name_;
}

const std::string RController::path() const
{
    if (parent_)
        return parent_->path() + '/' + name_;
    else
        return '/' + name_;
}

RController *RController::node(const std::string &path)
{
    static std::regex rNode("[^/]+", std::regex::icase|std::regex::optimize);
    std::sregex_iterator it(path.begin(), path.end(), rNode), end;
    RController *result = nullptr;

    if (it != end)
    {
        if(path[0] == '/')
        {
            if (root()->name_ == it->str())
            {
                result = root();
                ++it;
            }
            else
                return nullptr;
        }
        else
            result = this;

        while (it != end)
        {
            auto str = it->str();
            if (it->str() == "..")
            {
                if (result->parent_)
                    result = result->parent_;
                else
                    return nullptr;
            }
            else
            {
                auto find = std::find_if(result->children_.begin(), result->children_.end(), [&it](RController* ctrl){
                        return ctrl->name_ == it->str();
                });
                if (find == result->children_.end())
                    return nullptr;
                else
                    result = *find;
            }
            ++it;
        }
    }

    return result;
}

RController *RController::root()
{
    if (parent_)
        return parent_->root();
    else
        return this;
}

void RController::setUpdataFunc(std::function<void ()> func)
{
    updataFunc = func;
}

void RController::setExecFunc(std::function<int ()> func)
{
    execFunc = func;
}

void RController::setTransFunc(std::function<void (TransEvent*)> func)
{
    transFunc = func;
}

void RController::setProcessFunc(std::function<void (ProcessEvent*)> func)
{
    processFunc = func;
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

void RController::dispatchEvent(CloseEvent *event)
{
    for(auto node = children_.begin(); node != children_.end();)
        (*node++)->dispatchEvent(event);

    closeFunc(event);
}

void RController::dispatchEvent(StartEvent *event)
{
    if(state_ == Status::Error) return;//错误状态无法进入循环

    startFunc(event);
    for(auto node = children_.begin(); node != children_.end();)
        (*node++)->dispatchEvent(event);

    state_ = Status::Looping;
}

void RController::dispatchEvent(FinishEvent *event)
{
    for(auto node = children_.begin(); node != children_.end();)
    {
        (*node)->state_ = state_.load();
        (*node++)->dispatchEvent(event);
    }
    finishFunc(event);
}

void RController::updataAll()
{
    updataFunc();
    for(auto node = children_.begin(); node != children_.end();)
        (*node++)->updataAll();
}

void RController::updata()
{
    updataFunc();
}

void RController::translation(TransEvent *info)
{
    transFunc(info);
}

void RController::process(ProcessEvent *info)
{
    processFunc(info);
}

void RController::defaultTransFunc(TransEvent *info)
{
    for(auto node = children_.begin(); node != children_.end();)
        (*node++)->transFunc(info);
}

void RController::defaultProcessFunc(ProcessEvent *info)
{
    for(auto node = children_.begin(); node != children_.end();)
        (*node++)->processFunc(info);
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

int RController::defaultExecFunc()
{
    StartEvent sEvent(this);
    dispatchEvent(&sEvent);

    while(loopingCheck() == Status::Looping)
    {
        ProcessEvent instruct(this, nullptr);
        process(&instruct);

        updataAll();
    }

    FinishEvent fEvent(this);
    dispatchEvent(&fEvent);
    closed.emit();

    if(check(state_ == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
