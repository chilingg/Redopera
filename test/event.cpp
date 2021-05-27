#include <RGame.h>
#include <RWindow.h>
#include <RInput.h>
#include <RTransform.h>
#include <RDebug.h>
#include <REntity.h>

using namespace Redopera;

int main()
{
    RGame game;

    RWindow::Format format;
    format.debug = false;

    RWindow window(640, 480, "Window", format);
    REntity entity("entity", nullptr);

    entity.addFunc<REntity&>("_start_event", [](REntity &e)
    {
        rDebug << "Start Event";
        rDebug  << "Gamepad number " << RInput::gamepadCount();

        RInput::joyPresented.connect(e.get<RSlot>("slot"), [](JoystickPresent state)
        {
            rDebug << "Joystick " << (state == JoystickPresent::CONNECTED ? "connected" : "disconnected");
        });
    });

    entity.addFunc<REntity&>("_finish_event", [](REntity &){ rDebug << "Finish Event"; });

    window.resized.connect(entity.addComponent<RSlot>("slot", {}), [](int w, int h){ rDebug << "Window resized: " << RSize(w, h); });

    entity.addFunc<REntity&>("_process_event", [](REntity &)
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

    auto exec = [&entity]()
    {
        entity.callFuncToAllFromThis<void>("_process_event");
        return 0;
    };

    entity.callFuncToAllFromThis<void>("_start_event");

    window.show();
    int status = window.exec(exec);

    entity.callFuncToAllFromThis<void>("_finish_event");

    return status;
}
