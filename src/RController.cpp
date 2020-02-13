#include "RController.h"
#include "RDebug.h"

#include <algorithm>
#include <regex>
#include <thread>

using namespace Redopera;

std::mutex RController::mutex;
RController::Status RController::globalState(RController::Status::Normal);

RController::RootTree RController::rootTree()
{
    static RootTree ROOT_TREE = std::make_shared<std::map<std::string, RController*>>();
    static std::mutex mutex;

    std::lock_guard<std::mutex> guard(mutex);
    while(!ROOT_TREE.unique())
        std::this_thread::yield();

    return ROOT_TREE;
}

void RController::terminateTree(RController::Status status)
{
    Status s = status != Status::Error ? Status::Finished : Status::Error;
    RootTree root = rootTree();
    globalState = s;
    for(auto node : *root)
        node.second->state_ = s;
}

bool RController::queryTreeList(TreeList &tList)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(tList != treeList())
    {
        tList = treeList();
        return false;
    }
    return true;
}

std::map<RController *, std::string>&  RController::treeList()
{
    static std::map<RController*, std::string> TREE_LIST;
    return TREE_LIST;
}

void RController::insertTreeNode()
{
    std::lock_guard<std::mutex> guard(mutex);
    treeList()[this] = getPathName();
}

void RController::eraseTreeNode()
{
    std::lock_guard<std::mutex> guard(mutex);
    treeList().erase(this);
}

RController::RController(RController *parent, const std::string &name):
    state_(Status::Normal),
    parent_(parent)
{
    Status s;

    if(parent_)
    {
        rename(name);
        parent_->children_.emplace(name_, this);
        s = parent_->state_.load();
    }
    else {
        name_ = name;
        s = globalState;
        RootTree root = rootTree(); // 阻塞rootTree()至root析构
        while(root->count(name_))
            name_ += '+';
        root->emplace(name_, this);
        insertTreeNode();
    }

    if(s == Status::Looping)
    {
        RStartEvent e(parent_);
        startEvent(e);
    }
}

RController::~RController()
{
    freeAllChild();
    if(parent_)
        parent_->children_.erase(name_);
    else
        rootTree()->erase(name_);
    eraseTreeNode();
}

void RController::activeOnce()
{
    for(auto &pair : children_)
        pair.second->activeOnce();
    control();
}

void RController::control()
{

}

void RController::translation(const RController::TranslationInfo &info)
{
    for(auto &pair : children_)
        pair.second->translation(info);
}

void RController::addChild(RController *child)
{
    if(check(!child || child == this || isAncestor(child), "Failure to add child!"))
        return;

    child->changeParent(this);
}

void RController::freeChild(RController *child)
{
    if(children_.count(child->name_))
        changeParent(nullptr);
}

void RController::freeAllChild()
{
    while(!children_.empty())
    {
        auto c = children_.begin();
        c->second->changeParent(nullptr);
    }
}

void RController::changeParent(RController *parent)
{
    assert(parent != parent_);

    if(parent_)
        parent_->children_.erase(name_);
    else
        rootTree()->erase(name_);

    parent_ = parent;

    // 父节点为nullptr的，都挂在rootTree下
    Status s;
    if(parent_)
    {
        s = parent_->state_;
        rename(name_);
        parent_->children_.emplace(name_, this);
    }
    else {
        s = globalState;
        RootTree root = rootTree(); // 阻塞rootTree()至root析构
        while(root->count(name_))
            name_ += '+';
        root->emplace(name_, this);
        insertTreeNode();
    }

    if(s != state_)
    {
        if(s == Status::Looping)
        {
            RStartEvent e(parent_);
            dispatchEvent(e);
        }
        else if(state_ == Status::Looping)
        {
            RFinishEvent e(parent_);
            dispatchEvent(e);
        }
    }
}

void RController::rename(std::string name)
{
    if(name.empty())
        name = defaultName();

    if(parent_)
    {
        while(parent_->children_.count(name))
            name += '+';
    }
    else {
        RootTree root = rootTree(); // 阻塞rootTree()至root析构
        while(root->count(name))
            name += '+';
    }
    name_.swap(name);
    insertTreeNode();
}

void RController::setStatus(RController::Status status)
{
    state_ = status;
}

void RController::childensTask(std::function<void (RController *)> task)
{
    for(auto child : children_)
        task(child.second);
}

RController *RController::node(const std::string &path)
{
    static std::regex rNode("[^/]+", std::regex::icase|std::regex::optimize);
    std::sregex_iterator it(path.begin(), path.end(), rNode), end;
    RController *result = nullptr;

    if(it != end)
    {
        if(path[0] == '/')
        {
            RootTree root = rootTree();
            auto n = root->find(it->str());
            if(n != root->end())
                result = n->second;
            else
                return nullptr;
        }
        else {
            if(it->str() == "..")
                result = parent_;
            else {
                result = getChild(it->str());
                if(!result)
                    return nullptr;
            }
        }
        ++it;
    }

    for(;it != end; ++it)
    {
        if(it->str() == "..")
        {
            if(result)
                result = result->parent_;
            else
                return result;
        }
        else {
            result = result->getChild(it->str());
            if(!result)
                return nullptr;
        }
    }

    return result;
}

RController::Status RController::status() const
{
    return state_;
}

const std::string &RController::name() const
{
    return name_;
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
    return children_.count(child->name_);
}

bool RController::isChild(const std::string &child) const
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

std::string RController::getPathName() const
{
    std::string name('/' + name_);
    RController *parent = parent_;
    while(parent)
    {
        name = '/' + parent->name_ + name;
        parent = parent->parent_;
    }

    return name;
}

int RController::getChildrenSize() const
{
    return children_.size();
}

RController *RController::getChild(const std::string &child)
{
    auto it = children_.find(child);
    return it != children_.end() ? it->second : nullptr;
}

RController *RController::getParent() const
{
    return parent_;
}

int RController::exec()
{
    assert(state_ != Status::Looping);

    RStartEvent sEvent(this);
    dispatchEvent(sEvent);

    while(loopingCheck() == Status::Looping)
        activeOnce();

    RFinishEvent fEvent(this);
    dispatchEvent(fEvent);
    closed.emit();

    if(check(state_ == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void RController::breakLoop()
{
    Status loop = Status::Looping;
    state_.compare_exchange_strong(loop, Status::Finished);
}

void RController::inputEvent(RInputEvent &)
{

}

void RController::startEvent(RStartEvent &)
{

}

void RController::closeEvent(RCloseEvent &)
{

}

void RController::finishEvent(RFinishEvent &)
{

}

void RController::dispatchEvent(RInputEvent &event)
{
    for(auto &pair : children_)
        pair.second->dispatchEvent(event);
    inputEvent(event);
}

void RController::dispatchEvent(RCloseEvent &event)
{
    for(auto &pair : children_)
        pair.second->dispatchEvent(event);
    closeEvent(event);
}

void RController::dispatchEvent(RStartEvent &event)
{
    if(!isReady()) return;//错误状态无法进入循环

    state_ = Status::Looping;
    for(auto &pair : children_)
        pair.second->dispatchEvent(event);
    startEvent(event);
}

void RController::dispatchEvent(RFinishEvent &event)
{
    assert(state_ != Status::Looping);

    for(auto &pair : children_)
    {
        pair.second->state_ = state_.load();
        pair.second->dispatchEvent(event);
    }
    finishEvent(event);
}

RController::Status RController::loopingCheck()
{
    if(state_ == Status::Finished)
    {
        RCloseEvent e(this);
        dispatchEvent(e);
        if(e.stop)
            state_ = Status::Looping;
    }

    return state_;
}
