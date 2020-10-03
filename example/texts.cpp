#include <RWindow.h>
#include <RGame.h>
#include <RRenderSystem.h>
#include <RController.h>
#include <RInputModule.h>
#include <RTextbox.h>
#include <RPlane.h>
#include <REvent.h>
#include <RDebug.h>
#include <RRect.h>

#include <dependents/stb_truetype.h>

using namespace Redopera;

const wchar_t* texts =
        L"If I should see you,\n"
        "After long years,\n"
        "How should I greet you?\n"
        "With silence and tears?";

const GLchar *vCode =
R"--(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 texCoor;
void main(void)
{
    texCoor = aTexCoor;
    gl_Position = projection * view * model* vec4(aPos, 0.0, 1.0);
}
)--";

const GLchar *fCode =
R"--(
#version 330 core
uniform sampler2D tex;
in vec2 texCoor;
out vec4 outColor;
void main(void)
{
    outColor = texture(tex, texCoor);
}
)--";

const int WIDTH = 480;
const int HEIGHT = 480;
const RColor fcolor(180, 180, 180);

std::unique_ptr<RPlane[]> textbox;
std::unique_ptr<RRenderSystem> renderer;

void control()
{
    renderer->renderer() << textbox[0] << textbox[1] << textbox[2] << textbox[3];
    renderer->renderLine(textbox[0].model());
    renderer->renderLine(textbox[1].model());
    renderer->renderLine(textbox[2].model());
    renderer->renderLine(textbox[3].model());
}

void startEvent(StartEvent*)
{
    textbox = std::make_unique<RPlane[]>(4);
    renderer = std::make_unique<RRenderSystem>();

    RTextbox::Format fmt;
    fmt.fcolor = fcolor;
    fmt.lSpacing = 1.6f;
    fmt.wSpacing = 1.3f;
    fmt.align = { RTextbox::Align::Top, RTextbox::Align::Left };

    RTextbox textsLoader(texts, 220, 110);
    textsLoader.setTextFormat(fmt);
    textbox[0].setTexture(textsLoader.texture());
    textbox[0].rSize().set(220, 110);
    textbox[0].rPos().set(20, HEIGHT - textbox[0].size().height() - 20, 0);

    textsLoader.setAlign(RTextbox::Align::Mind, RTextbox::Align::Mind);
    textbox[1] = textbox[0];
    textbox[1].setTexture(textsLoader.texture());
    textbox[1].rPos().setY(textbox[0].pos().y() - (textbox[0].size().height() + 20) * 1);

    textsLoader.setAlign(RTextbox::Align::Bottom, RTextbox::Align::Right);
    textbox[2] = textbox[0];
    textbox[2].setTexture(textsLoader.texture());
    textbox[2].rPos().setY(textbox[0].pos().y() - (textbox[0].size().height() + 20) * 2);

    textsLoader.setAlign(RTextbox::Align::Top, RTextbox::Align::Right);
    textsLoader.rSize().set(90, 240);
    textsLoader.vertical();
    textbox[3] = textbox[0];
    textbox[3].setTexture(textsLoader.texture());
    textbox[3].rSize().set(90, 240);
    textbox[3].rPos().setX(textbox[0].rect().right() + 20);
    textbox[3].rPos().move(0, -(textbox[3].size().height() - textbox[0].size().height()));

    renderer->setShaderProg(RShaderProg({ RShader(vCode, RShader::Type::Vertex),
                                      RShader(fCode, RShader::Type::Fragment)}));
    renderer->setCameraMove();
    renderer->setViewprot(0, WIDTH, 0, HEIGHT);
}

RPoint2 offset;
RPlane *hold;

void inputEvent(processEvent *e)
{
    const RInputModule *input = RWindow::mainWindow()->input();

    if (input->press(Keys::KEY_ESCAPE))
        e->sender->breakLoop();

    if (input->press(MouseBtn::MOUSE_BUTTON_LEFT))
    {
        RPoint2 pos = input->pos().mirrorV(HEIGHT / 2);

        if (textbox[0].rect().contains(pos))
            hold = &textbox[0];
        else if (textbox[1].rect().contains(pos))
            hold = &textbox[1];
        else if (textbox[2].rect().contains(pos))
            hold = &textbox[2];
        else if (textbox[3].rect().contains(pos))
            hold = &textbox[3];

        if (hold)
        {
            offset = pos - hold->pos();
        }
    }
    if (input->release(MouseBtn::MOUSE_BUTTON_LEFT))
    {
        hold = nullptr;
    }
    if (hold && input->status(MouseBtn::MOUSE_BUTTON_LEFT) == BtnAct::PRESS)
    {
        hold->rPos() = input->pos().mirrorV(HEIGHT / 2) - offset;
    }
}

void finishEvent(FinishEvent *)
{
    textbox.reset();
    renderer.reset();
}

int main()
{
    RGame game;

    RWindow::Format format;
    format.background = 0x101020ff;
    RWindow textWin(WIDTH, HEIGHT, "Texts Window", format);

    //RFont font("/usr/share/fonts/wenquanyi/wqy-zenhei/wqy-zenhei.ttc");
    RFont font = RFont::sourceCodePro();
    font.setSize(12);
    RFont::setDefaultFont(font);

    RController ctrl;
    ctrl.setInputFunc(inputEvent);
    ctrl.setStartFunc(startEvent);
    ctrl.setControlFunc(control);
    ctrl.setFinishFunc(finishEvent);
    textWin.ctrl()->addChild(&ctrl);

    textWin.show();
    return game.exec(&textWin);
}
