#include <RWindow.h>

using namespace Redopera;

class TestWin : public RWindow
{
public:
    TestWin(const Format &format):
        RWindow(format)
    {}

    void control() override
    {
        glClearColor(std::sin(glfwGetTime()), 0, 0, 1.0f);
    }
protected:
    void inputEvent(RInputEvent &e) override
    {
        if(e.press(Keys::KEY_ESCAPE))
            breakLoop();
    }
};

int main()
{
    RWindow::Format format;
    format.debug = true;
    format.versionMajor = 4;
    format.versionMinor = 3;
    format.fix = true;
    format.initWidth = 250;
    format.initHeight = 250;

    TestWin window(format);
    window.show();

    return window.exec();
}
