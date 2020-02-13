#include <rsc/RShaderProg.h>
#include <RWindow.h>
#include <RThread.h>
#include <RPlane.h>
#include <RTextbox.h>
#include <RDebug.h>

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

class Tester : public RWindow
{
public:
    Tester(std::future<RTexture> &&f, const RWindow::Format &format):
        RWindow(format),
        future(std::move(f))
    {}

protected:
    void control() override
    {
        plane.render(shaders, mLoc);
    }

    void startEvent(RStartEvent &) override
    {
        shaders.attachShader({ RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)});
        shaders.linkProgram();
        auto inter = shaders.useInterface();
        mLoc = shaders.getUniformLocation("model");
        inter.setViewprot(shaders.getUniformLocation("projection"), 0, width(), 0, height());

        RTexture tex = future.get();
        plane.setSize(tex.size());
        plane.setPos((width() - plane.width()) / 2, (height() - plane.height()) / 2);
        plane.setTexture(tex);
    }

    void finishEvent(RFinishEvent &) override
    {
        done = true;
    }

    void inputEvent(RInputEvent &e) override
    {
        // inputEvent只能监测感兴趣的按键
        if(e.press(Keys::KEY_ESCAPE))
            breakLoop();
    }

private:
    std::future<RTexture> future;
    RPlane plane;
    RShaderProg shaders;
    GLuint mLoc;
};

int main()
{
    std::promise<RTexture> promise;
    std::future<RTexture> f = promise.get_future();

    std::promise<GLFWwindow*> promise2;
    std::future<GLFWwindow*> f2 = promise2.get_future();

    RThread thread([&promise, &promise2]{

        if(check(!RContext::initialization(), "Failure initialization OpenGL context!"))
            exit(EXIT_FAILURE);

        RContext::Format format;
        GLFWwindow *window = RContext::setContexAsThisThread(format);

        if(check(!window, "Failure get OpenGL context"))
            exit(EXIT_FAILURE);

        promise2.set_value(window);
        RTextsbxo text;
        text.update();
        promise.set_value(text.textTexture());

        while(!done)
            ;
    });

    RWindow::Format format;
    format.shared = f2.get();
    Tester window(std::move(f), format);
    window.setWindowSize(400, 400);

    window.show();
    return window.exec();
}
