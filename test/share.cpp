#include <rsc/RShaderProg.h>
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
GLuint mLoc;

// 未设置当前线程OpenGL Context之前无法创建OpenGL相关对象
std::unique_ptr<RPlane> plane;
std::unique_ptr<RShaderProg> shaders;

void control()
{
    plane->render(*shaders, mLoc);
}

void startEvent(StartEvent)
{
    shaders->attachShader({ RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)});
    shaders->linkProgram();
    auto inter = shaders->useInterface();
    mLoc = shaders->getUniformLocation("model");

    RTexture tex = future.get();
    plane->setSize(tex.size());
    plane->setTexture(tex);
}

void translation(const TransEvent &e)
{
    shaders->useInterface().setViewprot(
                shaders->getUniformLocation("projection"), 0, e.size.width(), 0, e.size.height());
    plane->setPos((e.size.width() - plane->width()) / 2, (e.size.height() - plane->height()) / 2);
}

void inputEvent(InputEvent e)
{
    if(e.press(Keys::KEY_ESCAPE))
        e.sender->breakLoop();
}

void finishEvent(FinishEvent)
{
    done = true;
}

int main()
{
    RWindow::Format format;
    RWindow window(400, 400, "Share", format);
    window.ctrl()->setStartFunc(startEvent);
    window.ctrl()->setInputFunc(inputEvent);
    window.ctrl()->setFinishFunc(finishEvent);
    window.ctrl()->setTranslateFunc(translation);
    window.ctrl()->setControlFunc(control);

    plane = std::make_unique<RPlane>();
    shaders = std::make_unique<RShaderProg>();

    std::promise<RTexture> promise;
    future = promise.get_future();

    RThread thread([&promise, &window]{

        RContext context;
        if(check(!context, "Failure initialization OpenGL context!"))
            exit(EXIT_FAILURE);

        RContext::Format format;
        format.shared = window.getWindowHandle();
        GLFWwindow *winhandle = context.setContex(format);

        if(check(!winhandle, "Failure get OpenGL context"))
            exit(EXIT_FAILURE);

        RTextsbox text;
        text.update();
        promise.set_value(text.textTexture());

        while(!done)
            ;
    });

    window.show();
    window.exec();

    plane.reset();
    shaders.reset();

    return 0;
}
