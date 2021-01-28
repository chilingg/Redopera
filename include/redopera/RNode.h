#ifndef RNODE_H
#define RNODE_H

#include <string>
#include <functional>
#include <map>
#include <any>
#include <typeinfo>
#include <stdexcept>
#include <atomic>

#include <RRect.h>

namespace Redopera {

class RRenderSys;

class RNode final
{
public:
    enum class Status
    {
        Normal,
        Looping,
        Error,
    };

    struct Instructs;

    RNode();

    template<typename Holder>
    RNode(const std::string &name, Holder *holder):
        RNode()
    {
        name_ = name;
        holder_ = { holder, typeid (holder).hash_code() };
    }

    ~RNode();

    RNode(RNode &) = delete;
    RNode& operator=(const RNode &) = delete;

    bool isLooping() const;
    bool isNormal() const;
    bool isValid() const;
    bool isChild(RNode *child) const;
    bool isChild(const std::string &child) const;
    bool isAncestor(RNode *node) const; // 祖辈
    Status status() const;
    int childrenNumber() const;
    RNode* parent() const;
    const std::string& name() const;
    std::string availableName(const std::string &name) const;
    std::string path() const;
    const RNode* node(const std::string &path) const;
    RNode* node(const std::string &path);
    const RNode* root() const;
    RNode* root();

    template<typename Holder>
    Holder* holder() const
    {
        Holder *p;
        if(typeid (p).hash_code() != holder_.hashcode)
            throw std::runtime_error("Error holder type!");
        return reinterpret_cast<Holder*>(holder_.p);
    }

    void addChild(RNode *child);
    bool freeChild(RNode *child);
    bool freeChild(const std::string &name);
    void freeAllChild();
    void changeParent(RNode *parent);

    const std::string& rename(const std::string &name);

    void setUpdateFunc(std::function<void(RRenderSys *)> func) { updateFunc = func; }
    void setExecFunc(std::function<int()> func) { execFunc = func; }
    void setStartFunc(std::function<void()> func) { startFunc = func; }
    void setFinishFunc(std::function<void()> func) { finishFunc = func; }

    void setTransformFunc(std::function<void(RNode*, const RRect&)> func) { transformFunc = func; }
    void setProcessFunc(std::function<void(RNode*, Instructs *)> func) { processFunc = func; }
    void setCustomFunc(std::function<bool(std::any*)> func) { customFunc = func; }

    // 发起更新 PS:深度优先、由上至下
    void update(RRenderSys *sys);
    // 事件发布接口 PS:深度优先、由下至上
    void dispatchStartEvent(); // The startevent change the node state to Looping
    void dispatchFinishEvent(); // the node state changed from Looping to the finishevent
    // 事件发布接口 PS:深度优先、由下至上
    void dispatchCustomEvent(std::any *data);

    // Pass only direct childnode
    void transformEventToChild(RNode *sender, const RRect &info);
    void processEventToChild(RNode *sender, Instructs *instructs);

    void updateThis(RRenderSys *sys);
    void transform(RNode *sender, const RRect &info); // Default call transformEventTochild
    void process(RNode *sender, Instructs *instructs); // Default call processEventToChild

    int exec();

    void breakLooping();
    void errorToLooping();

private:
    int defaultExecFunc();

    std::function<int()> execFunc;
    std::function<void(RRenderSys *sys)> updateFunc;
    std::function<void()> finishFunc;
    std::function<void()> startFunc;

    std::function<void(RNode*, const RRect &info)> transformFunc;
    std::function<void(RNode*, Instructs*)> processFunc;
    std::function<bool(std::any*)> customFunc;

    std::map<std::string, RNode*> children_;
    std::atomic<Status> state_ = Status::Normal;
    std::string name_ = "Node";
    RNode *parent_ = nullptr;
    struct {
        void *p = nullptr; size_t hashcode;
    } holder_;
};

struct RNode::Instructs
{
    void addInstruct(const std::string &instruct, int value = 1)
    {
        instructs_[instruct].i = value;
    }

    void addInstruct(const std::string &instruct, float value)
    {
        instructs_[instruct].f = value;
    }


    void deleteInstruct(const std::string &instruct)
    {
        instructs_.erase(instruct);
    }

    bool inInstructs(const std::string &instruct)
    {
        return instructs_.count(instruct) != 0;
    }

    int instruct(const std::string &instruct)
    {
        auto it = instructs_.find(instruct);

        if(it == instructs_.end())
            return 0;
        return it->second.i;
    }

    float instructf(const std::string &instruct)
    {
        auto it = instructs_.find(instruct);

        if(it == instructs_.end())
            return 0;
        return it->second.f;
    }

    void clear() { instructs_.clear(); }

private:
    union value {
        int i; float f;
    };

    std::map<std::string, value> instructs_;
};

} // Redopera

#endif // RNODE_H
