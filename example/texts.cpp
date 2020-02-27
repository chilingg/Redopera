#include <RWindow.h>
#include <RController.h>
#include <RInputModule.h>
#include <RTextbox.h>

using namespace Redopera;

const wchar_t* texts =
        L"If I should see you,\n"
        "After long years,\n"
        "How should I greet you?\n"
        "With silence and tears?";

const int WIDTH = 450;
const int HEIGHT = 490;

std::unique_ptr<RTextsbox[]> textbox;

void control()
{
    textbox[0].edging(RColor(50, 50, 50));
    textbox[1].edging(RColor(50, 50, 50));
    textbox[0].render();
    textbox[1].render();
    textbox[2].render();
    textbox[3].render();
}

void startEvent(StartEvent)
{
    textbox[0].setTexts(texts);
    textbox[0].setFontSize(13);
    textbox[0].setAlign(RArea::Align::Mind, RArea::Align::Left);
    textbox[0].setFontColor(210, 210, 210);
    textbox[0].setSize(190, 90);
    textbox[0].setPadding(8, 8, 8, 8);
    textbox[0].setlineSpacing(1.4);
    textbox[0].setPos(20, HEIGHT - textbox[0].height() - 20);

    textbox[1] = textbox[0];
    textbox[1].setAlign(RArea::Align::Mind, RArea::Align::Right);
    textbox[1].setX(WIDTH - textbox[1].width() - 20);

    textbox[2] = textbox[0];
    textbox[2].setHeight(320);
    textbox[2].setlineSpacing(2.6f);
    textbox[2].setSpacing(0.5f);
    textbox[2].setWordSpacing(1.5f);
    textbox[2].setAlign(RArea::Align::Top, RArea::Align::Mind);
    textbox[2].verticalTypeset();
    textbox[2].setBackColor(0xff251515);
    textbox[2].setY(HEIGHT - textbox[2].height() - textbox[1].height() - 40);

    textbox[3] = textbox[2];
    textbox[3].setAlign(RArea::Align::Mind, RArea::Align::Mind);
    textbox[3].setX(WIDTH - textbox[3].width() - 20);

    // textbox的着色器设置（与plane并不共享，建议永远与窗口尺寸等同，避免字体渲染虚化）
    // 此处获取的着色器是RTextsbox默认共享的着色器
    const RShaderProg &shaders = textbox[0].textsShader();
    RInterface inter = shaders.useInterface();
    inter.setViewprot(shaders.getUniformLocation("projection"), 0, WIDTH, 0, HEIGHT);
}

void inputEvent(InputEvent e)
{
    if(e.press(Keys::KEY_ESCAPE))
        e.sender->breakLoop();
}

int main()
{
    RWindow::Format format;
    format.background = 0x101018;
    format.fix = true;
    RWindow textWin(WIDTH, HEIGHT, "Texts Window", format);
    textWin.ctrl()->setInputFunc(inputEvent);
    textWin.ctrl()->setStartFunc(startEvent);
    textWin.ctrl()->setControlFunc(control);

    textbox = std::make_unique<RTextsbox[]>(4);

    textWin.show();
    textWin.exec();

    textbox.reset();
    return 0;
}
