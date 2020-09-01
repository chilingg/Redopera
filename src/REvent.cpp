#include <REvent.h>
#include <RWindow.h>
#include <RInputModule.h>

using namespace Redopera;

BtnAct InputInfo::status(Keys key)
{
    return sender->inputModule()->keyStatus(key);
}

BtnAct InputInfo::status(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn);
}

BtnAct InputInfo::status(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return BtnAct::RELEASE;

    return RInputModule::toButtonAction(sender->inputModule()->gamepad_[jid].status.buttons[static_cast<unsigned>(btn)]);
}

float InputInfo::status(GamepadAxes axis, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
    {
        if(axis == GamepadAxes::GAMEPAD_AXIS_LEFT_TRIGGER || axis == GamepadAxes::GAMEPAD_AXIS_RIGHT_TRIGGER)
            return -1.f;
        else
            return 0.f;
    }

    return sender->inputModule()->gamepad_[jid].status.axes[static_cast<unsigned>(axis)];
}

bool InputInfo::press(Keys key)
{
    return sender->inputModule()->keyStatus(key) == BtnAct::PRESS && sender->inputModule()->keyDown_.count(key);
}

bool InputInfo::press(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn) == BtnAct::PRESS && sender->inputModule()->mouseDown_.count(btn);
}

bool InputInfo::press(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return false;

    unsigned index = static_cast<unsigned>(btn);
    return sender->inputModule()->gamepad_[jid].status.buttons[index] && !sender->inputModule()->gamepad_[jid].preButtons[index];
}

bool InputInfo::release(Keys key)
{
    return sender->inputModule()->keyStatus(key) == BtnAct::RELEASE && sender->inputModule()->keyUp_.count(key);
}

bool InputInfo::release(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn) == BtnAct::RELEASE && sender->inputModule()->mouseUp_.count(btn);
}

bool InputInfo::release(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return false;

    unsigned index = static_cast<unsigned>(btn);
    return !sender->inputModule()->gamepad_[jid].status.buttons[index] && sender->inputModule()->gamepad_[jid].preButtons[index];
}

RPoint2 InputInfo::pos()
{
    return sender->inputModule()->cursorPos();
}

int InputInfo::wheel()
{
    return sender->inputModule()->wheel_;
}

bool InputInfo::anyKeyPress()
{
    return !sender->inputModule()->keyDown_.empty();
}
