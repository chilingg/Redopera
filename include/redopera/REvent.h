#ifndef REVENT_H
#define REVENT_H

#include "RPoint.h"
#include "RSize.h"

class GLFWwindow;

namespace Redopera {

class RController;
class RWindow;

// 事件通知类集合 ********************

// 循环开始事件
class StartEvent
{
public:
    explicit StartEvent(RController *sender):
        sender(sender) {}
    RController *sender;
};

// 循环结束事件
class FinishEvent
{
public:
    explicit FinishEvent(RController *sender):
        sender(sender) {}
    RController *sender;
};

// 申请关闭事件
class CloseEvent
{
public:
    explicit CloseEvent(RController *sender):
        sender(sender) {}
    RController *sender;
    bool stop = false; // true驳回关闭申请
};

struct TransInfo
{
    RController *sender;
    const RSize size;
    const RPoint pos = RPoint(0);
};

enum class Keys;
enum class MouseBtn;
enum class GamepadBtn;
enum class BtnAct;
enum class GamepadAxes;
enum class JoystickID;

struct InputEvent
{
    InputEvent(RWindow *sender):
        sender(sender) {}

    BtnAct status(Keys key);
    BtnAct status(MouseBtn btn);
    BtnAct status(GamepadBtn btn, JoystickID jid);
    float status(GamepadAxes axis, JoystickID jid);

    bool press(Keys key);
    bool press(MouseBtn btn);
    bool press(GamepadBtn btn, JoystickID jid);

    bool release(Keys key);
    bool release(MouseBtn btn);
    bool release(GamepadBtn btn, JoystickID jid);

    RPoint2 pos();
    int wheel();

    bool anyKeyPress();

    RWindow *sender;
};

} // Redopera

#endif // REVENT_H
