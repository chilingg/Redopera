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

    window.node.setStartFunc([]{
        rDebug << "Start Event";
        rDebug  << "Gamepad number " << RInput::gamepadCount();

        RInput::joyPresented.connect([](JoystickPresent state){
            rDebug << "Joystick " << (state == JoystickPresent::CONNECTED ? "connected" : "disconnected");
            return true;
        });
    });

    window.node.setFinishFunc([]{ rDebug << "Finish Event"; });

    window.node.setTransformFunc([](RNode *, const RRect &info){ rDebug << "Translation Event: " << info; });

    window.node.setProcessFunc([](RNode *, RNode::Instructs *)
    {
        if(RInput::anyKeyPress())
            rDebug << "Any key Press";
        if(RInput::anyMouseBtnPress())
            rDebug << "Any mouse button Press";
        if(!RInput::wheel().isOrigin())
            rDebug << "Mouse whell " << RInput::wheel();
        if(RInput::cursorMove())
            rDebug << "Move cursor to " << RInput::cursorPos();
        if(!RInput::charInput().empty())
            rDebug << "Character Input: " << RInput::charInput();

        if(RInput::gamepadCount())
        {
            for(size_t i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; ++i)
            {
                if(RInput::press(static_cast<GamepadBtn>(i)))
                    rDebug << "Gamepad button " << i << "is pressed";
            }
            if(std::abs(RInput::status(GamepadAxes::LEFT_X)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_LEFT_X " << RInput::status(GamepadAxes::LEFT_X);
            if(std::abs(RInput::status(GamepadAxes::LEFT_Y)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_LEFT_Y " << RInput::status(GamepadAxes::LEFT_Y);
            if(std::abs(RInput::status(GamepadAxes::RIGHT_X)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_RIGHT_X " << RInput::status(GamepadAxes::RIGHT_X);
            if(std::abs(RInput::status(GamepadAxes::RIGHT_Y)) > 0.2f)
                rDebug << "GAMEPAD_AXIS_RIGHT_Y " << RInput::status(GamepadAxes::RIGHT_Y);
            if(RInput::status(GamepadAxes::LEFT_TRIGGER) > -.8f)
                rDebug << "GAMEPAD_AXIS_LEFT_TRIGGER " << RInput::status(GamepadAxes::LEFT_TRIGGER);
            if(RInput::status(GamepadAxes::RIGHT_TRIGGER) > -.8f)
                rDebug << "GAMEPAD_AXIS_RIGHT_TRIGGER " << RInput::status(GamepadAxes::RIGHT_TRIGGER);
        }
    });

    window.show();
    return window.node.exec();
}
