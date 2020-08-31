#include <RGame.h>
#include <RWindow.h>
#include <RInputModule.h>
#include <RController.h>
#include <REvent.h>
#include <RDebug.h>
#include <RTime.h>

using namespace Redopera;

RPoint2 pos;

void startEvent(StartEvent *)
{
    rDebug << "Loop start.";
}

void closeEvent(CloseEvent *e)
{
    rDebug << "Loop would close.";
    // e->stop = true; // Stop close
    e->stop = false;
}

void finishEvent(FinishEvent *)
{
    rDebug << "Loop finish.";
}

void transInfo(TransInfo *info)
{
    rDebug << "Translate info:" << info->pos << info->size;
}

void inputEvent(InputEvent *e)
{
    if (e->press(Keys::KEY_ESCAPE))
        e->sender->closeWindow();

    if (e->anyKeyPress())
        rDebug << "Any key Press.";
    if (e->press(Keys::KEY_S))
        rDebug << "Key S is press.";
    if (e->release(Keys::KEY_S))
        rDebug << "Key S is release.";

    RPoint2 p = e->pos();
    if (pos != p)
    {
        pos = p;
        rDebug << "Cursor position move to " << p;
    }

    if (e->wheel())
        rDebug << "Mouse whell is" << e->wheel();

    if (e->press(MouseBtn::MOUSE_BUTTON_LEFT))
        rDebug << "Mouse left button is press.";
    if (e->release(MouseBtn::MOUSE_BUTTON_LEFT))
        rDebug << "Mouse left button is release.";
}

int main()
{
    RGame game;

    RWindow::Format format;
    format.debug = false;
    format.versionMajor = 3;
    format.versionMinor = 3;
    format.fix = false;
    format.viewport = RWindow::Viewport::Fix;

    RWindow window(640, 480, "Window", format);

    RController ctrl;
    ctrl.setStartFunc(startEvent);
    ctrl.setCloseFunc(closeEvent);
    ctrl.setFinishFunc(finishEvent);
    ctrl.setTransFunc(transInfo);
    ctrl.setInputFunc(inputEvent);

    window.ctrl()->addChild(&ctrl);
    window.show();

    return game.exec(&window);
}
