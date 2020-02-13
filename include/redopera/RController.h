#ifndef RCONTROLLER_H
#define RCONTROLLER_H

#include "REvent.h"
#include "RSigslot.h"
#include "RSize.h"
#include "RMath.h"

#include <map>
#include <set>
#include <atomic>

namespace Redopera {

class RController
{
public:
    enum class Status
    {
        Normal,
        Looping,
        Finished,
        Error,
    };

    struct TranslationInfo
    {
        const RController *sender;
        RSize size;
        RPoint pos = RPoint(0);
        glm::vec3 rotate { 0.0f, 0.0f, 0.0f };
    };

    using RootTree = std::shared_ptr<std::map<std::string, RController*>>;
    using TreeList = std::map<RController *, std::string>;

    constexpr static const char* defaultName() { return "Controller"; }
    // rootTree()会阻塞直到之前返回的RootTree释放
    static RootTree rootTree();
    static void terminateTree(Status status = Status::Finished);
    static bool queryTreeList(TreeList &tList);

    explicit RController(RController *parent = nullptr, const std::string &name = defaultName());
    virtual ~RController();
    RController(RController &ctl) = delete;
    RController& operator=(const RController &ctl) = delete;

    void activeOnce();
    //控制类最主要函数，需子类自己实现
    virtual void control();
    //变化信息传递，默认为原样转递给子结点
    virtual void translation(const TranslationInfo &info);

    void addChild(RController *child);
    void freeChild(RController *child);
    void freeAllChild();
    void changeParent(RController *parent);
    void rename(std::string name);

    RController* node(const std::string &path);
    Status status() const;
    const std::string& name() const;
    bool isLooping() const;
    bool isNormal() const;
    bool isValid() const;
    bool isReady() const;
    bool isChild(RController *child) const;
    bool isChild(const std::string &child) const;
    bool isAncestor(RController *node) const;//祖辈
    bool isFree() const;
    std::string getPathName() const;
    int getChildrenSize() const;
    RController* getChild(const std::string &child);
    RController* getParent() const;

    int exec();         //调用自身及所有子节点的contral()
    void breakLoop();   //退出exec循环

    RSignal<> closed;

protected:
    //事件响应。感兴趣的子类负责重写
    virtual void inputEvent(RInputEvent &event);
    virtual void closeEvent(RCloseEvent &event);    //尝试终止exec循环
    virtual void startEvent(RStartEvent &event);    //exec循环开始
    virtual void finishEvent(RFinishEvent &event);  //exec循环终止

    //事件发布接口 PS:深度优先、由下至上
    //由子类负责调用
    void dispatchEvent(RInputEvent &event);
    //已由RController负责调用
    void dispatchEvent(RCloseEvent &event);
    void dispatchEvent(RStartEvent &event);
    void dispatchEvent(RFinishEvent &event);

    // 若状态是Finishi则发起closeEvent
    Status loopingCheck();
    void setStatus(Status status);

    void childensTask(std::function<void(RController*)> task);

private:
    static std::map<RController*, std::string>& treeList();

    static std::mutex mutex;
    static Status globalState;

    void insertTreeNode();
    void eraseTreeNode();

    std::string name_;
    std::atomic<Status> state_;
    std::map<std::string, RController*> children_;
    RController *parent_ = nullptr;

    _RSLOT_TAIL_
};

} // Redopera

#endif // RCONTROLLER_H
