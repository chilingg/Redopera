#include <RNode.h>
#include <RTransform.h>
#include <regex>
#include <cassert>

using namespace Redopera;

RNode::RNode():
    execFunc([this]{ return defaultExecFunc(); }),
    updateFunc([](RRenderSys *){}),
    finishFunc([](){}),
    startFunc([](){}),
    transformFunc([this](RNode *sender, const RTransform &info){ transformEventToChild(sender, info); }),
    processFunc([this](RNode *sender, Instructs *instructs){ processEventToChild(sender, instructs); }),
    customFunc([](std::any *){ return true; })
{

}

RNode::~RNode()
{
    freeAllChild();
    if(parent_)
        parent_->children_.erase(name_);
}

bool RNode::isLooping() const
{
    return state_ == Status::Looping;
}

bool RNode::isNormal() const
{
    return state_ == Status::Normal;
}

bool RNode::isValid() const
{
    return state_ == Status::Looping || state_ == Status::Normal;
}

bool RNode::isChild(RNode *child) const
{
    auto it = children_.find(child->name_);
    return it->second == child;
}

bool RNode::isChild(const std::string &child) const
{
    return children_.count(child) != 0;
}

bool RNode::isAncestor(RNode *node) const
{
    if(!parent_)
        return false;
    else if (parent_ != node)
        return parent_->isAncestor(node);
    else
        return true;
}

RNode::Status RNode::status() const
{
    return state_;
}

int RNode::childrenNumber() const
{
    return children_.size();
}

RNode *RNode::parent() const
{
    return parent_;
}

const std::string& RNode::name() const
{
    return name_;
}

std::string RNode::availableName(const std::string &name) const
{
    std::string an = name;
    auto it = children_.find(an);
    for (size_t n = 1; it != children_.end(); ++n)
    {
       an = name + std::to_string(n);
       it = children_.find(an);
    }

    return an;
}

std::string RNode::path() const
{
    if (parent_)
        return parent_->path() + '/' + name_;
    else
        return '/' + name_;
}

const RNode *RNode::node(const std::string &path) const
{
    static std::regex rNode("[^/]+", std::regex::icase|std::regex::optimize);

    std::sregex_iterator it(path.begin(), path.end(), rNode), end;
    if(it == end)
        return nullptr;

    const RNode *result = nullptr;

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


    while (it != end && result)
    {
        if(it->str() == "..")
            result = result->parent_;
        else
        {
            auto find = result->children_.find(it->str());
            if(find == result->children_.end())
                return nullptr;
            else
                result = find->second;
        }
        ++it;
    }

    return result;
}

RNode *RNode::node(const std::string &path)
{
    return const_cast<RNode*>(const_cast<const RNode*>(this)->node(path));
}

RNode *RNode::root()
{
    if (parent_)
        return parent_->root();
    else
        return this;
}

const RNode *RNode::root() const
{
    if (parent_)
        return parent_->root();
    else
        return this;
}

void RNode::addChild(RNode *child)
{
    if(!child || child == this || isAncestor(child))
        throw std::runtime_error("Failure to add child!");

    child->changeParent(this);
}

bool RNode::freeChild(RNode *child)
{
    if(children_.count(child->name_))
    {
        child->changeParent(nullptr);
        return true;
    }
    return false;
}

bool RNode::freeChild(const std::string &name)
{
    auto it = children_.find(name);
    if(it != children_.end())
    {
        it->second->changeParent(nullptr);
        return true;
    }
    return false;
}

void RNode::freeAllChild()
{
    while(!children_.empty())
    {
        auto c = children_.begin();
        c->second->changeParent(nullptr);
    }
}

void RNode::changeParent(RNode *parent)
{
    if(parent == this)
        throw std::runtime_error("This parent is self");

    if(parent_)
        parent_->children_.erase(name_);
    parent_ = parent;

    Status s = Status::Normal;
    if(parent_)
    {
        name_ = parent_->availableName(name_);

        s = parent_->state_;
        parent_->children_.emplace(name_, this);
    }

    if(s != state_)
    {
        if(s == Status::Looping)
            dispatchStartEvent();
        else if(state_ == Status::Looping)
        {
            state_ = s;
            dispatchFinishEvent();
        }
    }
}

const std::string &RNode::rename(const std::string &name)
{
    if(parent_)
    {
        parent_->children_.erase(name_);
        name_ = parent_->availableName(name);
        parent_->children_.emplace(name_, this);
    }
    else
        name_ = name;

    return name_;
}

void RNode::update(RRenderSys *sys)
{
    updateFunc(sys);
    for(auto& [n, p] : children_)
        p->update(sys);
}

void RNode::dispatchStartEvent()
{
    assert(state_ != Status::Looping);
    if(state_ == Status::Error)
        return;

    startFunc();
    for(auto& [n, p] : children_)
        p->dispatchStartEvent();

    state_ = Status::Looping; // 事件结束后更改状态，避免在Startevent响应中入树重复触发Startevent
}

void RNode::dispatchFinishEvent()
{
    assert(state_ != Status::Looping);

    for(auto& [n, p] : children_)
    {
        p->state_ = state_.load();
        p->dispatchFinishEvent();
    }
    finishFunc();
}

void RNode::dispatchCustomEvent(std::any *data)
{
    customFunc(data);
    for(auto& [n, p] : children_)
        p->dispatchCustomEvent(data);
}

void RNode::transformEventToChild(RNode *sender, const RTransform &info)
{
    for(auto& [n, p] : children_)
        p->transformFunc(sender, info);
}

void RNode::processEventToChild(RNode *sender, RNode::Instructs *instructs)
{
    for(auto& [n, p] : children_)
        p->processFunc(sender, instructs);
}

void RNode::updateThis(RRenderSys *sys)
{
    updateFunc(sys);
}

void RNode::transform(RNode *sender, const RTransform &info)
{
    transformFunc(sender, info);
}

void RNode::process(RNode *sender, RNode::Instructs *instructs)
{
    processFunc(sender, instructs);
}

int RNode::exec()
{
    return execFunc();
}

void RNode::breakLooping()
{
    if (parent_)
        parent_->breakLooping();
    else {
        Status loop = Status::Looping;
        state_.compare_exchange_strong(loop, Status::Normal);
    }
}

void RNode::errorToLooping()
{
    if (parent_)
        parent_->errorToLooping();
    else {
        Status loop = Status::Looping;
        state_.compare_exchange_strong(loop, Status::Error);
    }
}

int RNode::defaultExecFunc()
{
    dispatchStartEvent();

    Instructs instructs;
    while(state_ == Status::Looping)
    {
        process(this, &instructs);
    }

    dispatchFinishEvent();

    if(state_ == Status::Error)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
