#include <RGame.h>
#include <RWindow.h>
#include <RInput.h>
#include <RTransform.h>
#include <RDebug.h>

using namespace Redopera;

int main()
{
    RGame game;

    RWindow::Format format;
    format.debug = false;

    RWindow window(640, 480, "Window", format);
    RPoint2 pos;

    window.node.setStartFunc([&pos]{
        rDebug << "Start Event";
        pos = RInput::input().cursorPos();
        rDebug  << "Gamepad number " << RInput::input().gamepadCount();

        RInput::input().joyPresented.connect([](JoystickPresent state){
            rDebug << "Joystick " << (state == JoystickPresent::CONNECTED ? "connected" : "disconnected");
            return true;
        });
    });

    window.node.setFinishFunc([]{ rDebug << "Finish Event"; });

    window.node.setTransformFunc([](RNode *, const RRect &info){ rDebug << "Translation Event: " << info; });

    window.node.setProcessFunc([&pos](RNode *, RNode::Instructs *)
    {
        if(RInput::input().anyKeyPress())
            rDebug << "Any key Press";
        if(RInput::input().anyMouseBtnPress())
            rDebug << "Any mouse button Press";
        if(!RInput::input().wheel().isOrigin())
            rDebug << "Mouse whell " << RInput::input().wheel();
        if(RInput::input().cursorPos() != pos)
        {
            pos = RInput::input().cursorPos();
            rDebug << "Move cursor to " << pos;
        }

        if(RInput::input().gamepadCount())
        {
            for(size_t i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; ++i)
            {
                if(RInput::input().press(static_cast<GamepadBtn>(i)))
                    rDebug << "Gamepad button " << i << "is pressed";
            }
            if(std::abs(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_X)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_LEFT_X " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_X);
            if(std::abs(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_Y)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_LEFT_Y " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_Y);
            if(std::abs(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_X)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_RIGHT_X " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_X);
            if(std::abs(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_Y)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_RIGHT_Y " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_Y);
            if(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_TRIGGER) > -.8f)
                rDebug << "GAMEPAD_AXIS_LEFT_TRIGGER " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_LEFT_TRIGGER);
            if(RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_TRIGGER) > -.8f)
                rDebug << "GAMEPAD_AXIS_RIGHT_TRIGGER " << RInput::input().status(GamepadAxes::GAMEPAD_AXIS_RIGHT_TRIGGER);
        }
    });

    window.show();
    return window.node.exec();
}
