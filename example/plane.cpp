#include <render/ROpenGL.h>
#include <render/RProgram.h>
#include <render/RContext.h>
#include <render/RTexture.h>
#include <render/RRenderer.h>
#include <rsc/RImage.h>
#include <rsc/RFont.h>

#include <RTimer.h>
#include <RGame.h>
#include <RInput.h>
#include <RWindow.h>

using namespace Redopera;

constexpr float SIZE = 480;

struct Plane
{
    RRectF rect;
    RTexture texture;
};

class Renderer : public RRenderer
{
public:
    Renderer():
        project_(program_.getUniformLoc("project")),
        model_(program_.getUniformLoc("model")),
        hue_(program_.getUniformLoc("hue"))
    {
        glClearColor(.05f, 0.f, 0.1f, 1.f);
        // 设置混合
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    const GLuint project_, model_, hue_;
};

class Scene
{
public:
    Scene():
        plane{{ RPointF(), RSizeF(36, 36) }, RImage::redoperaIcon() },
        viewport(0, 0, SIZE, SIZE)
    {
        RRectF rect = plane.rect;
        rect.setCenter(viewport.center());
        plane.rect.setPos(rect);

        RFont font;
        font.setSize(36);
        RFont::Glyph glyph = font.getGlyph(L'↑');
        RTexture::setTextureFormat(RTexture::SingleToLinear4);
        arrow.load(glyph.data.get(), glyph.width, glyph.height, 1);
        RTexture::setTextureFormat(RTexture::Nearest4);

        rect.setSize(arrow.size().convert<float>());
        rect.setCenter(viewport.center());

        arrowModel[0] = glm::translate(glm::mat4(1), { rect.centerX(), rect.centerY() + 60, 0 });
        arrowModel[0] = glm::rotate(arrowModel[0], glm::radians(180.f), { 0.f, 0.f, 1.f });
        arrowModel[0] = glm::scale(arrowModel[0], { rect.width(), rect.height(), 0 });

        arrowModel[1] = glm::translate(glm::mat4(1), { rect.centerX(), rect.centerY() - 60, 0 });
        arrowModel[1] = glm::scale(arrowModel[1], { rect.width(), rect.height(), 0 });

        arrowModel[2] = glm::translate(glm::mat4(1), { rect.centerX() + 60, rect.centerY(), 0 });
        arrowModel[2] = glm::rotate(arrowModel[2], glm::radians(90.f), { 0.f, 0.f, 1.f });
        arrowModel[2] = glm::scale(arrowModel[2], { rect.width(), rect.height(), 0 });

        arrowModel[3] = glm::translate(glm::mat4(1), { rect.centerX() - 60, rect.centerY(), 0 });
        arrowModel[3] = glm::rotate(arrowModel[3], glm::radians(-90.f), { 0.f, 0.f, 1.f });
        arrowModel[3] = glm::scale(arrowModel[3], { rect.width(), rect.height(), 0 });

        auto rpi = renderer.program().use();
        rpi.setUniformMat(renderer.project_, math::viewport(viewport));
    }

    void update()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.bindVAO();
        RRPI rpi = renderer.program().use();

        rpi.setUniform(renderer.hue_, .1f, .1f, .2f, 1.f);
        for(int i = 0; i < 4; ++i)
        {
            rpi.setUniformMat(renderer.model_, arrowModel[i]);
            arrow.bind();
            renderer.draw();
        }

        math::setRectAs(model, plane.rect);
        model[1][1] = -model[1][1];
        rpi.setUniformMat(renderer.model_, model);
        rpi.setUniform(renderer.hue_, 1.f, 1.f, 1.f, 1.f);
        plane.texture.bind();
        renderer.draw();

        renderer.unbindVAO();
    }

    void processEvent(const SDL_Event &e)
    {
        if(e.type == SDL_WINDOWEVENT)
        {
            if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                glViewport(0, 0, e.window.data1, e.window.data2);

                auto pos = viewport.center();
                viewport.setSize(e.window.data1, e.window.data2);
                viewport.setCenter(pos);

                auto rpi = renderer.program().use();
                rpi.setUniformMat(renderer.project_, math::viewport(viewport));
            }
        }
    }

    void process()
    {
        if(RWindow::queryCursorStatus() == SDL_DISABLE)
        {
            if(RInput::mouse().move())
                RWindow::showCursor(true);
        }
        else if(RWindow::queryCursorStatus() == SDL_ENABLE)
        {
            if(RInput::mouse().move())
                timer.start();
            else if(timer.elapsed() > 1500)
                RWindow::showCursor(false);
        }

        if(RInput::key().press(SDL_SCANCODE_ESCAPE))
        {
            SDL_Event e;
            e.type = SDL_QUIT;
            SDL_PushEvent(&e);
        }

        RPointF p;
        int step = 5;
        if(RInput::key().status(SDL_SCANCODE_LEFT) == R_PRESSED)
            p.rx() -= step;
        if(RInput::key().status(SDL_SCANCODE_RIGHT) == R_PRESSED)
            p.rx() += step;
        if(RInput::key().status(SDL_SCANCODE_UP) == R_PRESSED)
            p.ry() += step;
        if(RInput::key().status(SDL_SCANCODE_DOWN) == R_PRESSED)
            p.ry() -= step;

        if(!p.isOrigin())
            plane.rect.move(p);
    }

private:
    Plane plane;
    RTexture arrow;
    RRectF viewport;
    RTimer timer;
    Renderer renderer;
    glm::mat4 model;
    glm::mat4 arrowModel[4];
};

int main()
{
    RGame game;
    RWindow window(SIZE, SIZE, "Plane", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    RContext context(window, RContext::Format());

    Scene scene;

    bool quit = false;
    SDL_Event e;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0 )
        {
            if(e.type == SDL_QUIT)
                quit = true;

            scene.processEvent(e);
        }

        scene.process();
        scene.update();

        window.swapBuffers();
    }

   return 0;
}
