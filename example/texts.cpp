#include <RWindow.h>
#include <RPlane.h>
#include <RTextbox.h>
#include <RTimer.h>

using namespace Redopera;

const wchar_t* texts =
        L"If I should see you,\n"
        "After long years,\n"
        "How should I greet you?\n"
        "With silence and tears?";

class TextWindow: public RWindow
{
public:
    TextWindow(const RWindow::Format &format):
        RWindow(format)
    {}

    void control() override
    {
        text[0].edging(RColor(50, 50, 50));
        text[1].edging(RColor(50, 50, 50));
        text[0].render();
        text[1].render();
        text[2].render();
        text[3].render();
    }

protected:
    void startEvent(RStartEvent &) override
    {
        // textbox的着色器设置（与plane并不共享，建议永远与窗口尺寸等同，避免字体渲染虚化）
        const RShaderProg &shaders = RTextsbxo::textboxShader();
        RInterface inter = shaders.useInterface();
        inter.setViewprot(shaders.getUniformLocation("projection"), 0, width(), 0, height());

        text->setTexts(texts);
        text[0].setFontSize(13);
        text[2].setAlign(RArea::Align::Mind, RArea::Align::Mind);
        text[0].setFontColor(210, 210, 210);
        text[0].setSize(190, 90);
        text[0].setPadding(8, 8, 8, 8);
        text[0].setlineSpacing(1.4);
        text[0].setPos(20, height() - text[0].height() - 20);

        text[1] = text[0];
        text[1].setAlign(RArea::Align::Mind, RArea::Align::Right);
        text[1].setX(width() - text[1].width() - 20);

        text[2] = text[0];
        text[2].setHeight(320);
        text[2].setlineSpacing(2.6f);
        text[2].setSpacing(0.5f);
        text[2].setAlign(RArea::Align::Top, RArea::Align::Mind);
        text[2].verticalTypeset();
        text[2].setBackColor(0xff251515);
        text[2].setY(height() - text[2].height() - text[1].height() - 40);

        text[3] = text[2];
        text[2].setAlign(RArea::Align::Mind, RArea::Align::Mind);
        text[3].setX(width() - text[3].width() - 20);
    }

    void inputEvent(RInputEvent &e) override
    {
        if(e.press(Keys::KEY_ESCAPE))
            breakLoop();
    }

private:
    RTextsbxo text[4];
};

int main()
{
    RWindow::Format format;
    format.initWidth = 450;
    format.initHeight = 490;
    format.background = 0x181010;
    format.fix = true;
    TextWindow textWin(format);

    textWin.show();
    return textWin.exec();
}
