#include <RNode.h>
#include <regex>
#include <cassert>

using namespace Redopera;

RNode::RNode():
    execFunc([this]{ return defaultExecFunc(); }),
    updateFunc([](RRenderSys *){}),
    finishFunc([](){}),
    startFunc([](){}),
    transformFunc([this](RNode *sender, const RRect &info){ transformEventToChild(sender, info); }),
    processFunc([this](RNode *sender, Instructs *instructs){ processEventToChild(sender, instructs); }),
    customFunc([](std::any *){ return true; })
{

}

RNode::~RNode()
{
    freeAllChild();
    if(parent_)
        parent_->children_.erase(parent_->findChild(this));
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
    auto it = findChild(child);
    return it != children_.end();
}

bool RNode::isChild(const RName &child) const
{
    auto it = findChild(child);
    return it != children_.end();
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

const RName& RNode::name() const
{
    return name_;
}

RName RNode::availableName(const RName &name) const
{
    auto it = findChild(name);

    if(it != children_.end())
    {
        size_t n = 0;
        std::string rename;
        do {
            ++n;
            rename = name.toString() + std::to_string(n);
            it = findChild(rename);
        } while(it != children_.end());

        return rename;
    }

    return name;
}

std::string RNode::path() const
{
    if (parent_)
        return parent_->path() + '/' + name_.toString();
    else
        return '/' + name_.toString();
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
            auto find = result->findChild(it->str());
            if(find == result->children_.end())
                return nullptr;
            else
                result = *find;
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
    if(isChild(child))
    {
        child->changeParent(nullptr);
        return true;
    }
    return false;
}

bool RNode::freeChild(const RName &name)
{
    auto it = findChild(name);
    if(it != children_.end())
    {
        (*it)->changeParent(nullptr);
        return true;
    }
    return false;
}

void RNode::freeAllChild()
{
    while(!children_.empty())
    {
        auto c = children_.begin();
        (*c)->changeParent(nullptr);
    }
}

void RNode::changeParent(RNode *parent)
{
    if(parent == this)
        return;

    if(parent_)
        parent_->children_.erase(parent_->findChild(this));
    parent_ = parent;

    Status s = Status::Normal;
    if(parent_)
    {
        name_ = parent_->availableName(name_);

        s = parent_->state_;
        parent_->children_.push_back(this);
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

RName RNode::rename(const RName &name)
{
    if(parent_)
    {
        auto it = parent_->findChild(name);
        if(it != children_.end() && *it != this)
            name_ = parent_->availableName(name);
        else
            name_ = name;
    }
    else
        name_ = name;

    return name_;
}

void RNode::update(RRenderSys *sys)
{
    updateFunc(sys);
    std::for_each(children_.begin(), children_.end(), [sys](RNode *c) { c->update(sys); });
}

void RNode::dispatchStartEvent()
{
    assert(state_ != Status::Looping);
    if(state_ == Status::Error)
        return;

    startFunc();
    std::for_each(children_.begin(), children_.end(), [](RNode *c) { c->dispatchStartEvent(); });

    state_ = Status::Looping; // 事件结束后更改状态，避免在Startevent响应中入树重复触发Startevent
}

void RNode::dispatchFinishEvent()
{
    assert(state_ != Status::Looping);

    std::for_each(children_.begin(), children_.end(), [this](RNode *c) {
        c->state_ = state_;
        c->dispatchFinishEvent();
    });
    finishFunc();
}

void RNode::dispatchCustomEvent(std::any *data)
{
    customFunc(data);
    std::for_each(children_.begin(), children_.end(), [&data](RNode *c) { c->dispatchCustomEvent(data); });
}

void RNode::transformEventToChild(RNode *sender, const RRect &info)
{
    std::for_each(children_.begin(), children_.end(), [sender, &info](RNode *c) { c->transformFunc(sender, info); });
}

void RNode::processEventToChild(RNode *sender, RNode::Instructs *instructs)
{
    static thread_local std::vector<RNode*> children;

    // 因为process event可能会改变children，所以复制一份后再发布event
    children = children_;
    std::for_each(children.begin(), children.end(), [sender, &instructs](RNode *c) { c->processFunc(sender, instructs); });
    children.clear();
}

void RNode::updateThis(RRenderSys *sys)
{
    updateFunc(sys);
}

void RNode::transform(RNode *sender, const RRect &info)
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
        if(state_ == Status::Looping)
            state_ = Status::Normal;
    }
}

void RNode::errorToLooping()
{
    if (parent_)
        parent_->errorToLooping();
    else {
        if(state_ == Status::Looping)
            state_ = Status::Error;
    }
}

int RNode::defaultExecFunc()
{
    dispatchStartEvent();

    Instructs instructs;
    while(state_ == Status::Looping)
    {
        process(this, &instructs);
        update(nullptr);
    }

    dispatchFinishEvent();

    if(state_ == Status::Error)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

std::vector<RNode*>::iterator RNode::findChild(RNode *node)
{
    return std::find(children_.begin(), children_.end(), node);
}

std::vector<RNode*>::iterator RNode::findChild(const RName &name)
{
    return std::find_if(children_.begin(), children_.end(), [&name](RNode *child) { return child->name_ == name; });
}

std::vector<RNode*>::const_iterator RNode::findChild(RNode *node) const
{
    return std::find(children_.cbegin(), children_.cend(), node);
}

std::vector<RNode*>::const_iterator RNode::findChild(const RName &name) const
{
    return std::find_if(children_.begin(), children_.end(), [&name](RNode *child) { return child->name_ == name; });
}
