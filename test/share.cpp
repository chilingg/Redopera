#include <rsc/RShaderProg.h>
#include <RGame.h>
#include <RRenderSystem.h>
#include <RWindow.h>
#include <RThread.h>
#include <RPlane.h>
#include <RTextbox.h>
#include <RDebug.h>
#include <RInputModule.h>
#include <REvent.h>
#include <RController.h>

#include <future>

using namespace Redopera;

// 测试多线程OpenGL Contex之间共享纹理
// 2020.2.11 Arch Linux Nvidia 驱动 nvidia-304xx 测试失败

const char* vCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec2 aTexCoor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "out vec2 texCoor;\n"
        "void main(void)\n"
        "{\n"
            "gl_Position = projection * model * vec4(aPos, 1.0);\n"
            "texCoor = aTexCoor;\n"
        "}\n";
const GLchar *fCode =
        "#version 330 core\n"
        "in vec2 texCoor; // \n"
        "out vec4 outColor;\n"
        "uniform sampler2D tex;\n"
        "void main(void)\n"
        "{\n"
        "outColor = texture(tex, texCoor.st);\n"
        "}\n";

std::atomic_bool done(false);

std::future<RTexture> future;

std::unique_ptr<RPlane> plane;
std::unique_ptr<RRenderSystem> renderer;

void control()
{
    renderer->renderer() << *plane;
    renderer->renderLine(plane->model());
}

void translation(TransEvent *e)
{
    plane->rPos() = RPoint((e->size.width() - plane->size().width()) / 2, (e->size.height() - plane->size().height()) / 2);
}

void startEvent(StartEvent* e)
{
    renderer->setShaderProg(RShaderProg({ RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)}));

    RTexture tex = future.get();
    plane->rSize() = tex.size();
    plane->setTexture(tex);

    renderer->setViewprot(0, 400, 0, 400);
    TransEvent info{e->sender, {400, 400}};
    translation(&info);
}

void inputEvent(ProcessEvent *e)
{
    if(RWindow::mainWindow()->input()->press(Keys::KEY_ESCAPE))
        e->sender->breakLoop();
}

void finishEvent(FinishEvent*)
{
    done = true;
}

int main()
{
    RGame game;

    RWindow window(400, 400, "Share");
    RController ctrl;
    ctrl.setStartFunc(startEvent);
    ctrl.setProcessFunc(inputEvent);
    ctrl.setFinishFunc(finishEvent);
    ctrl.setTransFunc(translation);
    ctrl.setControlFunc(control);
    window.ctrl()->addChild(&ctrl);

    plane = std::make_unique<RPlane>();
    renderer = std::make_unique<RRenderSystem>();

    std::promise<RTexture> promise;
    future = promise.get_future();

    RThread thread([&promise, &window]{
        RContext::Format format;
        format.shared = window.getWindowHandle();
        RContext context(format);
        if(check(!context, "Failure initialization OpenGL context!"))
            exit(EXIT_FAILURE);

        RTextbox text;
        text.updataTex();
        promise.set_value(text.texture());

        while(!done)
            ;
    });

    window.show();
    game.exec(&window);

    plane.reset();
    renderer.reset();

    return 0;
}
