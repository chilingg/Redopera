#ifndef REVENT_H
#define REVENT_H

#include "RPoint.h"
#include "RSize.h"

namespace Redopera {

class RController;

// 事件通知类集合 ********************

// 循环开始事件
class StartEvent
{
public:
    explicit StartEvent(RController *sender = nullptr):
        sender(sender) {}
    RController *sender;
};

// 循环结束事件
class FinishEvent
{
public:
    explicit FinishEvent(RController *sender = nullptr):
        sender(sender) {}
    RController *sender;
};

// 申请关闭事件
class CloseEvent
{
public:
    explicit CloseEvent(RController *sender = nullptr):
        sender(sender) {}
    RController *sender;
    bool stop = false; // true驳回关闭申请
};

struct TransEvent
{
    RController *sender;
    const RSize size;
    const RPoint pos = RPoint(0);
    struct {
        float x, y, z;
    } const rotate { 0.0f, 0.0f, 0.0f };
};

enum class Keys;
enum class MouseBtn;
enum class GamepadBtn;
enum class BtnAct;
enum class GamepadAxes;

struct InputEvent
{
    InputEvent(RController *sender = nullptr):
        sender(sender) {}

    BtnAct status(Keys key);
    BtnAct status(MouseBtn btn);
    BtnAct status(GamepadBtn btn, unsigned p = 0);
    float status(GamepadAxes axis, unsigned p = 0);

    bool press(Keys key);
    bool press(MouseBtn btn);
    bool press(GamepadBtn btn, unsigned p = 0);

    bool release(Keys key);
    bool release(MouseBtn btn);
    bool release(GamepadBtn btn, unsigned p = 0);

    RPoint2 pos();
    RPoint2 prePos();

    RController *sender;
};

} // Redopera

#endif // REVENT_H
