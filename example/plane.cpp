#include <RWindow.h>
#include <RController.h>
#include <RInputModule.h>
#include <RPlane.h>
#include <RGame.h>
#include <RRect.h>
#include <RKeeper.h>
#include <REvent.h>
#include <RRenderSystem.h>
#include <RTextbox.h>
#include <rsc/RShaderProg.h>
#include <rsc/RImage.h>
#include <RDebug.h>

using namespace Redopera;

const GLchar *vCode =
R"--(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 texCoor;
void main(void)
{
    texCoor = aTexCoor;
    gl_Position = projection * view * model* vec4(aPos, 1.0);
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

class TestCtl
{
public:
    TestCtl():
        ctrl(this),
        plane(36, 36, RPoint(0, 0), RImage::redoperaIcon())
    {
        plane.flipV();

        RTextbox arrowLoad(L"↑", 50, 50, 0, 0);

        arrowLoad.setAlign(RTextbox::Align::Mind, RTextbox::Align::Mind);
        arrowLoad.setFontSize(36);
        arrowLoad.setFontColor(50, 50, 60);
        arrow[0].setTexture(arrowLoad.texture());
        arrow[0].rPos() = arrowLoad.pos();
        arrow[0].rSize() = arrowLoad.size();
        arrow[1] = arrow[0];
        arrow[2] = arrow[0];
        arrow[3] = arrow[0];

        arrow[1].setRotate(0, 0, glm::radians(90.0f));
        arrow[2].setRotate(0, 0, glm::radians(180.0f));
        arrow[3].setRotate(0, 0, glm::radians(270.0f));

        ctrl.setControlFunc(std::bind(&TestCtl::control, this));
        ctrl.setStartFunc(std::bind(&TestCtl::startEvent, this, std::placeholders::_1));
        ctrl.setInputFunc(std::bind(&TestCtl::inputEvent, this, std::placeholders::_1));
        ctrl.setTransFunc(std::bind(&TestCtl::translation, this, std::placeholders::_1));

        rSystem.setShaderProg(RShaderProg({ RShader(vCode, RShader::Type::Vertex),
                                          RShader(fCode, RShader::Type::Fragment)}));
        rSystem.setCameraMove();
    }

    void control()
    {
        rSystem.render(arrow[0]);
        rSystem.render(arrow[1]);
        rSystem.render(arrow[2]);
        rSystem.render(arrow[3]);

        rSystem.render(plane);
    }

    void startEvent(StartEvent*)
    {
        RWindow * window = RGame::getMainWindow();

        TransInfo info = { &ctrl, window->size(), RPoint(0) };
        translation(&info);
    }

    void translation(TransInfo* info)
    {
        viewpro.set(info->size, info->pos);

        plane.rPos().setX(info->size.width()/2 - plane.size().width()/2);
        plane.rPos().setY(info->size.height()/2 - plane.size().height()/2);

        arrow[0].rPos().set(info->size.width()/2 - arrow[0].size().width()/2, info->size.height()/2 - arrow[0].size().height()/2 + 60, 0);
        arrow[1].rPos().set(info->size.width()/2 - arrow[0].size().width()/2 - 60, info->size.height()/2 - arrow[0].size().height()/2, 0);
        arrow[2].rPos().set(info->size.width()/2 - arrow[0].size().width()/2, info->size.height()/2 - arrow[0].size().height()/2 - 60, 0);
        arrow[3].rPos().set(info->size.width()/2 - arrow[0].size().width()/2 + 60, info->size.height()/2 - arrow[0].size().height()/2, 0);

        // 必须设置一次的视口
        rSystem.setViewprot(0, info->size.width(), 0, info->size.height());
    }

    void inputEvent(InputInfo *e)
    {
        RWindow* window = RGame::getMainWindow();
        if(window->cursorMode() == RWindow::CursorMode::Hidden)
        {
            if(e->pos() != prePos)
                window->setCursorModel(RWindow::CursorMode::Normal);
        }
        else if(window->cursorMode() == RWindow::CursorMode::Normal)
        {
            if(e->pos() != prePos)
                timer.start();
            else if(e->pos() == prePos && timer.elapsed() > 1500)
                window->setCursorModel(RWindow::CursorMode::Hidden);
        }

        // inputEvent只能监测感兴趣的按键
        if(e->press(Keys::KEY_ESCAPE))
            ctrl.getParent()->breakLoop();

        RPoint3 p(0);
        if(e->status(Keys::KEY_LEFT) == BtnAct::PRESS)
            p.rx() -= 4;
        if(e->status(Keys::KEY_RIGHT) == BtnAct::PRESS)
            p.rx() += 4;
        if(e->status(Keys::KEY_UP) == BtnAct::PRESS)
            p.ry() += 4;
        if(e->status(Keys::KEY_DOWN) == BtnAct::PRESS)
            p.ry() -= 4;
        if(!p.isOrigin() && viewpro.contains(plane.rect() + p))
            plane.rPos() = plane.pos() + p;

        prePos = e->pos();
    }

    RController ctrl;

private:
    RPlane plane;
    RPlane arrow[4];
    RRect viewpro;
    RTimer timer;
    RPoint2 prePos;
    RRenderSystem rSystem;
};

int main()
{
    RGame game;
    RWindow::Format format;
    format.background = 0x101018;
    RWindow window(480, 480, "Plane", format);

    TestCtl t;
    t.ctrl.changeParent(window.ctrl());

    window.show();
    return game.exec(&window);
}
