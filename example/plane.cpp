#include <RWindow.h>
#include <RPlane.h>
#include <RGame.h>
#include <RKeeper.h>
#include <RInput.h>
#include <RTimer.h>
#include <RTextsLoader.h>

using namespace Redopera;

constexpr float SIZE = 480;

template<typename T>
const RRenderSys& RRenderSys::operator<<(T &obj) const
{
    render(obj.texture(), obj.model());
    return *this;
}

class TestCtl
{
public:
    TestCtl():
        node("TestCtrl", this),
        plane({ RPoint(), RSize(36, 36) }, RTexture(RImage::redoperaIcon())),
        viewpro(0, 0, SIZE, SIZE)
    {
        RRectF rect = plane.rect();
        rect.setCenter(viewpro.center());
        plane.setPos(rect.pos());
        plane.flipV();

        RTextsLoader arrowLoad(L"↑", 50, 50);

        arrowLoad.setAlign(RTextsLoader::Align::Mind, RTextsLoader::Align::Mind);
        arrowLoad.setFontSize(36);

        arrow[0].setTexture(arrowLoad.texture());
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
        RRPI rpi = sys->shaders().use();
        rpi.setUniform(sys->loc(RRenderSys::HUE), .1f, .1f, .14f, 1.f);
        sys->usingSingleTexOut();
        *sys<< arrow[0] << arrow[1] << arrow[2] << arrow[3];

        rpi.setUniform(sys->loc(RRenderSys::HUE), 1.f, 1.f, 1.f, 1.f);
        sys->usingTexColorOut();
        *sys << plane;
    }

    void translation(RNode *sender, const RRect &info)
    {
        sender->holder<RWindow>()->renderSys()->setViewport(0, info.width(), 0, info.height());

        float xratio = info.width() / viewpro.width();
        float yratio = info.height() / viewpro.height();
        plane.setPos(plane.pos().x() * xratio, plane.pos().y() * yratio);

        viewpro = info;

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
            if(RInput::cursorMove())
                window->setCursorModel(RWindow::CursorMode::Normal);
        }
        else if(window->cursorMode() == RWindow::CursorMode::Normal)
        {
            if(RInput::cursorMove())
                timer.start();
            else if(timer.elapsed() > 1500)
                window->setCursorModel(RWindow::CursorMode::Hidden);
        }

        // inputEvent只能监测感兴趣的按键
        if(RInput::press(Keys::KEY_ESCAPE))
            node.breakLooping();

        RPoint p;
        int step = 5;
        if(RInput::status(Keys::KEY_LEFT) == BtnAct::PRESS)
            p.rx() -= step;
        if(RInput::status(Keys::KEY_RIGHT) == BtnAct::PRESS)
            p.rx() += step;
        if(RInput::status(Keys::KEY_UP) == BtnAct::PRESS)
            p.ry() += step;
        if(RInput::status(Keys::KEY_DOWN) == BtnAct::PRESS)
            p.ry() -= step;

        if(!p.isOrigin() && viewpro.contains(plane.rect() + p))
            plane.move(p);
    }

    RNode node;

private:
    RPlane plane;
    RPlaneT arrow[4];
    RRectF viewpro;
    RTimer timer;
};

int main()
{
    RGame game;
    RWindow::Format format;
    format.background = 0x101018ff;
    format.debug = false;
    RWindow window(SIZE, SIZE, "Plane", format);

    TestCtl t;
    t.node.changeParent(&window.node);

    window.show();
    return window.node.exec();
}
