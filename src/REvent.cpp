#include <REvent.h>
#include <RWindow.h>
#include <RInputModule.h>

using namespace Redopera;

BtnAct InputEvent::status(Keys key)
{
    return sender->inputModule()->keyStatus(key);
}

BtnAct InputEvent::status(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn);
}

BtnAct InputEvent::status(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return BtnAct::RELEASE;

    return RInputModule::toButtonAction(sender->inputModule()->gamepad_[jid].status.buttons[static_cast<unsigned>(btn)]);
}

float InputEvent::status(GamepadAxes axis, JoystickID jid)
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

bool InputEvent::press(Keys key)
{
    return sender->inputModule()->keyStatus(key) == BtnAct::PRESS && sender->inputModule()->keyDown_.count(key);
}

bool InputEvent::press(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn) == BtnAct::PRESS && sender->inputModule()->mouseDown_.count(btn);
}

bool InputEvent::press(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return false;

    unsigned index = static_cast<unsigned>(btn);
    return sender->inputModule()->gamepad_[jid].status.buttons[index] && !sender->inputModule()->gamepad_[jid].preButtons[index];
}

bool InputEvent::release(Keys key)
{
    return sender->inputModule()->keyStatus(key) == BtnAct::RELEASE && sender->inputModule()->keyUp_.count(key);
}

bool InputEvent::release(MouseBtn btn)
{
    return sender->inputModule()->mouseStatus(btn) == BtnAct::RELEASE && sender->inputModule()->mouseUp_.count(btn);
}

bool InputEvent::release(GamepadBtn btn, JoystickID jid)
{
    if (!sender->inputModule()->gamepad_.count(jid))
        return false;

    unsigned index = static_cast<unsigned>(btn);
    return !sender->inputModule()->gamepad_[jid].status.buttons[index] && sender->inputModule()->gamepad_[jid].preButtons[index];
}

RPoint2 InputEvent::pos()
{
    return sender->inputModule()->cursorPos();
}

int InputEvent::wheel()
{
    return sender->inputModule()->wheel_;
}

bool InputEvent::anyKeyPress()
{
    return !sender->inputModule()->keyDown_.empty();
}
