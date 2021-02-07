#include <RWindow.h>
#include <RGame.h>
#include <RPlane.h>
#include <RRect.h>
#include <RInput.h>
#include <RTextsLoader.h>

using namespace Redopera;

const wchar_t* texts =
        L"If I should see you,\n"
        "After long years,\n"
        "How should I greet you?\n"
        "With silence and tears?";

const int WIDTH = 480;
const int HEIGHT = 480;
const glm::vec3 fcolor(.7f, .7f, .7f);

std::unique_ptr<RPlane[]> label;

void update(RRenderSys *sys)
{
    *sys << label[0] << label[1] << label[2] << label[3];

    sys->renderLine(label[0].model());
    sys->renderLine(label[1].model());
    sys->renderLine(label[2].model());
    sys->renderLine(label[3].model());
}

void startEvent()
{
    label = std::make_unique<RPlane[]>(4);

    RTextsLoader::Format fmt;
    fmt.lSpacing = 1.6f;
    fmt.wSpacing = 1.3f;
    fmt.align = { RTextsLoader::Align::Top, RTextsLoader::Align::Left };

    RTextsLoader textsLoader(texts, 220, 110);
    textsLoader.setTextsFormat(fmt);
    RRect rect;

    rect.setSize(220, 110);
    rect.setX(20);
    rect.setTop(HEIGHT - 20);
    label[0].setModel(rect);
    label[0].setTexture(textsLoader.texture());

    textsLoader.setAlign(RTextsLoader::Align::Mind, RTextsLoader::Align::Mind);
    rect.setTop(rect.bottom() - 20);
    label[1].setModel(rect);
    label[1].setTexture(textsLoader.texture());

    textsLoader.setAlign(RTextsLoader::Align::Bottom, RTextsLoader::Align::Right);
    rect.setTop(rect.bottom() - 20);
    label[2].setModel(rect);
    label[2].setTexture(textsLoader.texture());

    textsLoader.setAlign(RTextsLoader::Align::Top, RTextsLoader::Align::Right);
    textsLoader.rSize().setSize(90, 240);
    textsLoader.vertical();
    rect.setSize(textsLoader.size());
    rect.setX(label[0].rect().right() + 20);
    rect.setTop(label[0].rect().top());
    label[3].setTexture(textsLoader.texture());
    label[3].setModel(rect);

    RWindow::focusWindow()->renderSys()->setCurrentShaders("SingleShaders");
    RWindow::focusWindow()->renderSys()->setViewprot(0, WIDTH, 0, HEIGHT);
    RRPI rpi = RWindow::focusWindow()->renderSys()->shaders()->use();
    rpi.setUniform(RWindow::focusWindow()->renderSys()->shaders()->getUniformLoc("color"), fcolor);
}

void inputEvent(RNode *sender, RNode::Instructs*)
{
    static RPlane *hold;
    static RPoint2 prePos;

    if (RInput::press(Keys::KEY_ESCAPE))
        sender->breakLooping();

    if (RInput::press(MouseBtn::LEFT))
    {
        RPoint2 pos = RInput::cursorPos();

        if (label[0].rect().contains(pos))
            hold = &label[0];
        else if (label[1].rect().contains(pos))
            hold = &label[1];
        else if (label[2].rect().contains(pos))
            hold = &label[2];
        else if (label[3].rect().contains(pos))
            hold = &label[3];

        if(hold)
        {
            prePos = pos;
        }
    }
    else if (RInput::release(MouseBtn::LEFT))
    {
        hold = nullptr;
    }

    if (hold && RInput::cursorMove())
    {
        hold->move(RInput::cursorPos() - prePos);
        prePos = RInput::cursorPos();
    }
}

void finishEvent()
{
    label.reset();
}

int main()
{
    RGame game;

    RWindow::Format format;
    format.background = 0x101020ff;
    format.viewport = RWindow::Viewport::Fix;
    RWindow textWin(WIDTH, HEIGHT, "Texts Window", format);

    //RFont font("/usr/share/fonts/wenquanyi/wqy-zenhei/wqy-zenhei.ttc");
    RFont font = RFont::sourceCodePro();
    font.setSize(12);
    RFont::setDefaultFont(font);

    RNode node;
    node.setProcessFunc(inputEvent);
    node.setStartFunc(startEvent);
    node.setUpdateFunc(update);
    node.setFinishFunc(finishEvent);
    textWin.node.addChild(&node);

    textWin.show();
    return textWin.node.exec();
}
