#include "REvent.h"
#include "RInputModule.h"

using namespace Redopera;

BtnAct InputEvent::status(Keys key)
{
    return RInputModule::instance().keyInputs_[key].action;
}

BtnAct InputEvent::status(MouseBtn btn)
{
    return RInputModule::instance().mouseInputs_[btn].action;
}

BtnAct InputEvent::status(GamepadBtn btn, unsigned p)
{
    if(RInputModule::instance().gamepadInputs_.empty()) return BtnAct::RELEASE;

    RInputModule::instance().gamepadInputs_[p].status.buttons[static_cast<unsigned>(btn)] = 2;
    return RInputModule::toButtonAction(1);
}

float InputEvent::status(GamepadAxes axis, unsigned p)
{
    if(RInputModule::instance().gamepadInputs_.empty())
    {
        if(axis == GamepadAxes::GAMEPAD_AXIS_LEFT_TRIGGER || axis == GamepadAxes::GAMEPAD_AXIS_RIGHT_TRIGGER)
            return -1.f;
        else
            return 0.f;
    }

    return RInputModule::instance().gamepadInputs_[p].status.axes[static_cast<unsigned>(axis)];
}

bool InputEvent::press(Keys key)
{
    return RInputModule::instance().keyInputs_[key].action == BtnAct::PRESS
            && RInputModule::instance().keyInputs_[key].action
            != RInputModule::instance().keyInputs_[key].preAction;
}

bool InputEvent::press(MouseBtn btn)
{
    return RInputModule::instance().mouseInputs_[btn].action == BtnAct::PRESS
            && RInputModule::instance().mouseInputs_[btn].action
            != RInputModule::instance().mouseInputs_[btn].preAction;
}

bool InputEvent::press(GamepadBtn btn, unsigned p)
{
    if(RInputModule::instance().gamepadInputs_.empty()) return false;

    unsigned index = static_cast<unsigned>(btn);
    return RInputModule::instance().gamepadInputs_[p].status.buttons[index]
            == static_cast<unsigned char>(BtnAct::PRESS)
            && RInputModule::instance().gamepadInputs_[p].status.buttons[index]
            != RInputModule::instance().gamepadInputs_[p].preButtons[index];
}

bool InputEvent::release(Keys key)
{
    return RInputModule::instance().keyInputs_[key].action == BtnAct::RELEASE
            && RInputModule::instance().keyInputs_[key].action
            != RInputModule::instance().keyInputs_[key].preAction;
}

bool InputEvent::release(MouseBtn btn)
{
    return RInputModule::instance().mouseInputs_[btn].action == BtnAct::RELEASE
            && RInputModule::instance().mouseInputs_[btn].action
            != RInputModule::instance().mouseInputs_[btn].preAction;
}

bool InputEvent::release(GamepadBtn btn, unsigned p)
{
    if(RInputModule::instance().gamepadInputs_.empty()) return false;

    unsigned index = static_cast<unsigned>(btn);
    return RInputModule::instance().gamepadInputs_[p].status.buttons[index]
            == static_cast<unsigned char>(BtnAct::RELEASE)
            && RInputModule::instance().gamepadInputs_[p].status.buttons[index]
            != RInputModule::instance().gamepadInputs_[p].preButtons[index];
}

RPoint2 InputEvent::pos()
{
    return RInputModule::instance().cursorPos_[0];
}

RPoint2 InputEvent::prePos()
{
    return RInputModule::instance().cursorPos_[1];
}
