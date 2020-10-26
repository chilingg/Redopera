#include <RGame.h>
#include <RWindow.h>
#include <RInputModule.h>
#include <RController.h>
#include <REvent.h>
#include <cmath>

using namespace Redopera;

void inputEvent(ProcessEvent *e)
{
    if(RWindow::mainWindow()->input()->anyKeyPress())
        e->sender->breakLoop();
}

void control()
{
    glClearColor(0, std::sin(glfwGetTime()) * .7f, 0, 1.0f);
}

int main()
{
    RGame game;

    RWindow::Format format;
    format.debug = false;
    format.versionMajor = 3;
    format.versionMinor = 3;
    format.fix = true;

    RWindow window(250, 250, "Window", format);

    RController ctrl;
    ctrl.setControlFunc(control);
    ctrl.setProcessFunc(inputEvent);

    window.ctrl()->addChild(&ctrl);
    window.show();

    return game.exec(&window);
}
