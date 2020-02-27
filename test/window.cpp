#include <RWindow.h>
#include <RInputModule.h>
#include <RController.h>
#include <cmath>

using namespace Redopera;

void inputEvent(InputEvent e)
{
    if(e.press(Keys::KEY_ESCAPE))
        e.sender->breakLoop();
}

void control()
{
    glClearColor(0, std::sin(glfwGetTime()) * .7f, 0, 1.0f);
}

int main()
{
    RWindow::Format format;
    format.debug = true;
    format.versionMajor = 4;
    format.versionMinor = 3;
    format.fix = true;

    RWindow window(250, 250, "Window", format);
    window.ctrl()->setControlFunc(control);
    window.ctrl()->setInputFunc(inputEvent);
    window.show();

    return window.exec();
}
