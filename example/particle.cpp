#include <RGame.h>
#include <RPlane.h>
#include <RWindow.h>
#include <deque>
#include <cstdlib>

using namespace Redopera;

std::deque<std::pair<RPlane, RPoint2>> particles;
std::unique_ptr<RPlane> init;

int gravity = 1;

void start()
{
    init = std::make_unique<RPlane>();
    init->setTexture(RTexture::createWhiteTex());
    init->setRect(RRect(2400, 0, 24, 24));
}

void finish()
{
    particles.clear();
    init.reset();
}

void update(RRenderSys *sys)
{
    if(rand() % 2 == 0)
        particles.emplace_back(*init, RPoint2(std::rand() % 40 - 20, std::rand() % 20 + 70));

    for(auto it = particles.begin(); it != particles.end();)
    {
        it->second.ry() -= gravity;
        it->first.rRect().move(it->second);

        if(it->second.y() < -60 && (rand() % 1000000))
            it = particles.erase(it);
        else
        {
            *sys << it->first;
            ++it;
        }
    }
}

void transform(RNode *sender, const RTransform&)
{
    sender->holder<RWindow>()->renderSys()->setViewprot(0, 4800, 0, 4800);
}

int main()
{
    RGame game;
    RWindow::Format format;
    format.debug = false;
    RWindow window(480, 480, "Particle", format);

    window.node.setStartFunc(start);
    window.node.setFinishFunc(finish);
    window.node.setUpdateFunc(update);
    window.node.setTransformFunc(transform);

    window.show();
    return window.node.exec();
}
