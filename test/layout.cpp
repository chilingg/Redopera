#include <RWindow.h>
#include <RGame.h>
#include <RWidget.h>
#include <RDebug.h>

using namespace Redopera;

int w = 800;
int h = 400;

int main()
{
    RGame game;
    RWindow::Format format;
    format.background = 0x101018ff;
    format.debug = false;
    RWindow window(w, h, "Layout", format);

    return 0;
}
