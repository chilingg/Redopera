#include <RInput.h>
#include <RFormat.h>
#include <RGame.h>
#include <render/RContext.h>

using namespace Redopera;

int jid = -1;

void testInput()
{
    if(RInput::key().press(SDL_SCANCODE_SPACE))
        fmt::print("Spase key pressed.\n");
    if(RInput::key().release(SDL_SCANCODE_SPACE))
        fmt::print("Spase key released.\n");
    if(RInput::key().repeat(SDL_SCANCODE_SPACE))
        fmt::print("Spase key repeate.\n");
    if(RInput::key().status(SDL_SCANCODE_SPACE) != R_NONE)
        ;//fmt::print("Spase key status is {}.\n", RInput::key().status(SDL_SCANCODE_SPACE));

    if(RInput::mouse().left())
        fmt::print("Mouse left status {}.\n", RInput::mouse().left());
    if(RInput::mouse().move())
        fmt::print("Cusor move to {}.\n", RInput::mouse().pos());

    static constexpr unsigned deadzone = 8000;

    auto joySys = RInput::joystick(0);
    if(joySys)
    {
        if(std::abs(joySys->axis(0)) > deadzone)
            fmt::print("Asis {}: {}\n", 0, joySys->axis(0));
        if(joySys->press(0))
        {
            fmt::print("Joy<{}> button<{}> pressed.\n", RInput::playerToJid(0), 0);
            joySys->rumble(0xff, 0xffff, 500);
        }
        if(joySys->release(0))
        {
            fmt::print("Joy<{}> button<{}> release.\n", RInput::playerToJid(0), 0);
        }
    }
    if(jid >= 0)
    {
        joySys = RInput::joystickFromID(jid);
        if(std::abs(joySys->axis(0)) > deadzone)
            fmt::print("Asis {}: {}\n", 0, joySys->axis(0));
        if(joySys->press(0))
        {
            fmt::print("Joy<{}> button<{}> pressed.\n", jid, 0);
            joySys->rumble(0xff, 0xffff, 500);
        }
        if(joySys->release(0))
        {
            fmt::print("Joy<{}> button<{}> release.\n", jid, 0);
        }
    }
}

int main()
{
    RGame game;
    RWindow window;

    RInput::joyPresented.connect(nullptr, [](bool added, int id)
    {
        if(added)
        {
            if(RInput::joystickFromID(id) != RInput::joystick(0))
                jid = id;
            fmt::print("Joystick <{}> added.\n", id);
        }
        else
        {
            if(id == jid)
                jid = -1;
            fmt::print("Joystick <{}> removed.\n", id);
        }
    });

    bool quit = false;
    SDL_Event e;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
                quit = true;

            RInput::process(e);
        }

        testInput();
        RInput::updataClear();
    }

    return 0;
}
