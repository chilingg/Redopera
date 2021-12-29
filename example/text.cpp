#include <render/render.h>
#include <rsc/RImage.h>

#include <RTextLoader.h>
#include <RGame.h>
#include <RWindow.h>
#include <SDL2/SDL.h>
#include <unordered_map>

using namespace Redopera;

constexpr float SIZE = 480;

const wchar_t* texts =
        L"If I should see you,\n"
        "After long years,\n"
        "How should I greet you?\n"
        "With silence and tears?";

class Renderer : public RRenderer
{
public:
    Renderer():
        project_(program_.getUniformLoc("project")),
        model_(program_.getUniformLoc("model")),
        hue_(program_.getUniformLoc("hue"))
    {
        auto fmt =  RTexture::Nearest3;
        fmt.wrap = { RTexture::Wrap::Repeat, RTexture::Wrap::Repeat };
        RTexture::setTextureFomat(fmt);
        tex_ = RTexture::createWhiteTex();

        setBackgroundColor(0x101020ff);

        // 设置混合
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 禁用字节对齐限制（字体图片1位缓存）
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        auto rpi = program_.use();
        rpi.setUniformMat(project_, math::viewport({ 0, 0, SIZE, SIZE }));
        rpi.setUniform(hue_, .7f, .3f, .3f, 1.f);
    }

    void renderRect(glm::mat4 &model)
    {
        tex_.bind();
        drawLine();
    }

    const GLuint project_, model_, hue_;
    RTexture tex_;
};

class Scene
{
public:
    Scene()
    {
        RTextLoader::Format fmt;
        fmt.lSpacing = 1.6f;
        fmt.wSpacing = 1.3f;
        fmt.padding = { 8, 8, 8, 8 };
        RTextLoader loader(fmt);
        // loader.setFont(RFont("/usr/share/fonts/wenquanyi/wqy-zenhei/wqy-zenhei.ttc"));
        loader.setFontSize(12);

        RTexture::setTextureFomat(RTexture::SingleToLinear4);
        RImage img(nullptr, 220, 110, 1);
        RRectF rect({ 20, SIZE - 110 }, img.size().convert<float>());

        img.fill(0);
        loader.makeTextBoxBitmapAs(img, texts, RTextLoader::Align::Top, RTextLoader::Align::Left);
        label[0].texture.load(img.flipV());
        rect.move(0, -20);
        math::setRectAs(label[0].model, rect);

        img.fill(0);
        loader.makeTextBoxBitmapAs(img, texts, RTextLoader::Align::Mind, RTextLoader::Align::Mind);
        label[1].texture.load(img.flipV());
        rect.move(0, -130);
        math::setRectAs(label[1].model, rect);

        img.fill(0);
        loader.makeTextBoxBitmapAs(img, texts, RTextLoader::Align::Bottom, RTextLoader::Align::Right);
        label[2].texture.load(img.flipV());
        rect.move(0, -130);
        math::setRectAs(label[2].model, rect);
    }

    void update()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.bindVAO();
        RRPI rpi = renderer.program().use();

        rpi.setUniformMat(renderer.model_, label[0].model);
        label[0].texture.bind();
        renderer.draw();
        renderer.renderRect(label[0].model);

        rpi.setUniformMat(renderer.model_, label[1].model);
        label[1].texture.bind();
        renderer.draw();
        renderer.renderRect(label[1].model);

        rpi.setUniformMat(renderer.model_, label[2].model);
        label[2].texture.bind();
        renderer.draw();
        renderer.renderRect(label[2].model);

        renderer.unbindVAO();
    }

private:
    struct{
        RTexture texture;
        glm::mat4 model;
    } label[3];
    Renderer renderer;
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
        }

        scene.update();
        window.swapBuffers();
    }

   return 0;
}
