#include <RWindow.h>
#include <RPlane.h>
#include <RGame.h>
#include <RKeeper.h>
#include <RInput.h>
#include <RTimer.h>

using namespace Redopera;

class TestCtl
{
public:
    TestCtl():
        node("TestCtrl", this),
        plane({ RPoint(), RSize(36, 36) })
    {
        plane.setTexture(RTexture(RImage::redoperaIcon()));
        plane.setFlipV();

        RTextsLoader arrowLoad(L"↑", 50, 50);

        arrowLoad.setAlign(RTextsLoader::Align::Mind, RTextsLoader::Align::Mind);
        arrowLoad.setFontSize(36);

        arrow[0].setTexture(arrowLoad.texture(), true);
        arrow[0].setSize(arrowLoad.size());

        arrow[1] = arrow[0];
        arrow[1].setRotate(0, 0, glm::radians(90.0f));

        arrow[2] = arrow[0];
        arrow[2].setRotate(0, 0, glm::radians(180.0f));

        arrow[3] = arrow[0];
        arrow[3].setRotate(0, 0, glm::radians(270.0f));

        node.setUpdateFunc([this](RRenderSys *sys){ update(sys); });
        node.setTransformFunc([this](RNode *sender, const RRect &info){ translation(sender, info); });
        node.setProcessFunc([this](RNode *sender, RNode::Instructs *ins) { processEvent(sender, ins); });
    }

    void update(RRenderSys *sys)
    {
        sys->setCurrentShaders("SingleShaders");
        RRPI rpi = sys->shaders()->use();
        rpi.setUniform(sys->shaders()->getUniformLoc("color"), .1f, .1f, .14f);
        *sys<< arrow[0] << arrow[1] << arrow[2] << arrow[3];
        rpi.reset();

        sys->setCurrentShaders("SimpleShaders");
        *sys << plane;
    }

    void translation(RNode *sender, const RRect &info)
    {
        sender->holder<RWindow>()->renderSys()->setViewprot(0, info.width(), 0, info.height());

        viewpro = info;

        plane.rRect().setCenter(info.center());

        arrow[0].rRect().setCenter(info.center());
        arrow[0].rRect().move(0, 60);
        arrow[1].rRect().setCenter(info.center());
        arrow[1].rRect().move(-60, 0);
        arrow[2].rRect().setCenter(info.center());
        arrow[2].rRect().move(0, -60);
        arrow[3].rRect().setCenter(info.center());
        arrow[3].rRect().move(60, 0);
    }

    void processEvent(RNode *, RNode::Instructs *)
    {
        RWindow* window = node.root()->holder<RWindow>();
        if(window->cursorMode() == RWindow::CursorMode::Hidden)
        {
            if(RInput::input().cursorMove())
                window->setCursorModel(RWindow::CursorMode::Normal);
        }
        else if(window->cursorMode() == RWindow::CursorMode::Normal)
        {
            if(RInput::input().cursorMove())
                timer.start();
            else if(timer.elapsed() > 1500)
                window->setCursorModel(RWindow::CursorMode::Hidden);
        }

        // inputEvent只能监测感兴趣的按键
        if(RInput::input().press(Keys::KEY_ESCAPE))
            node.breakLooping();

        RPoint p;
        int step = 5;
        if(RInput::input().status(Keys::KEY_LEFT) == BtnAct::PRESS)
            p.rx() -= step;
        if(RInput::input().status(Keys::KEY_RIGHT) == BtnAct::PRESS)
            p.rx() += step;
        if(RInput::input().status(Keys::KEY_UP) == BtnAct::PRESS)
            p.ry() += step;
        if(RInput::input().status(Keys::KEY_DOWN) == BtnAct::PRESS)
            p.ry() -= step;

        if(!p.isOrigin() && viewpro.contains(plane.rect() + p))
            plane.rRect().move(p);
    }

    RNode node;

private:
    RPlane plane;
    RPlane arrow[4];
    RRect viewpro;
    RTimer timer;
};

int main()
{
    RGame game;
    RWindow::Format format;
    format.background = 0x101018ff;
    format.debug = false;
    RWindow window(480, 480, "Plane", format);

    TestCtl t;
    t.node.changeParent(&window.node);

    window.show();
    return window.node.exec();
}
