#include <RGame.h>
#include <RWindow.h>
#include <RInput.h>
#include <cmath>

using namespace Redopera;

void process(RNode *sender, RNode::Instructs*)
{
    if(RInput::anyKeyPress())
        sender->breakLooping();
}

void control(RRenderSys *)
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

    window.node.setUpdateFunc(control);
    window.node.setProcessFunc(process);

    window.show();
    return window.node.exec();
}
