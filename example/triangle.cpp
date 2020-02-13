#include <RWindow.h>
#include <RKeeper.h>
#include <RDebug.h>
#include <rsc/RShaderProg.h>

using namespace Redopera;

const char *vCode =
        "#version 330\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "out vec3 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * model * vec4(aPos, 1.0);\n"
        "   float n = 1 - (gl_Position.z+1)/2;\n"
        "   n = n * n * n * 5;\n"
        "   if(aPos.x > 0) color = vec3(0, 0, n);\n"
        "   else if(aPos.x < 0) color = vec3(0, 0, n);\n"
        "   else color = vec3(n, 0, 0);\n"
        "}\n";

const char *fCode =
        "#version 330\n"
        "in vec3 color;\n"
        "out vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "   fragColor = vec4(color, 1.0);\n"
        "}\n";

class TestCtl: public RController
{
public:
    TestCtl(RController *parent, const std::string &name):
        RController(parent, name),
        shaders({RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)}),
        model(glm::mat4(1))
    {}

    void control() override
    {
        model = glm::rotate(model, 0.05f, { 0.0f, 1.0f, 0.0f });
        auto itf = shaders.useInterface();
        itf.setUniformMatrix(modelLoc, model);
        glBindVertexArray(VAO.get());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

protected:
    void startEvent(RStartEvent &) override
    {
        // start事件在调用exce()时发起
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        VAO.reset(vao, [](GLuint vao){ glDeleteVertexArrays(1, &vao); });
        VBO.reset(vbo, [](GLuint vbo){ glDeleteBuffers(1, &vbo); });

        float vertices[] = {
             0.0f, 53.0f, 0.0f,
            20.0f, 13.0f, 0.0f,
           -20.0f, 13.0f, 0.0f
        };
        glBindVertexArray(VAO.get());
        glBindBuffer(GL_ARRAY_BUFFER, VBO.get());
        // 复制数据到VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 设置顶点属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), bufOff(0));
        glEnableVertexAttribArray(0);

        GLuint projection = shaders.getUniformLocation("projection");
        modelLoc = shaders.getUniformLocation("model");

        // Interface生存周期内对应的shader program都处于glUseProgram()调用中，析构时自动glUseProgram(0);
        RInterface intf = shaders.useInterface();
        intf.setUniformMatrix(modelLoc, model);
        // intf.setViewprot(projection, -2.0f, 2.0f, -2.0f, 2.0f); // 正交视图
        intf.setPerspective(projection, -30.0f, 30.0f, 0.0f, 60.0f, 0.0f, 1500.0f);
    }

    void finishEvent(RFinishEvent &) override
    {
        // finish事件在exce()退出时发起
        shaders.release(); // 显式释放不是必须的，再次只是示范
    }

    void inputEvent(RInputEvent &e) override
    {
        // inputEvent只能监测感兴趣的按键
        if(e.press(Keys::KEY_ESCAPE))
            getParent()->breakLoop();
    }

private:
    RKeeper<GLuint> VAO, VBO;
    RShaderProg shaders;
    GLuint modelLoc;
    glm::mat4 model;
};

RWindow *p; // entered信号才能实时监测按键事件
bool observeKeyboard(Keys, ButtonAction, Modifier)
{
    p->breakLoop();
    return true;
}

int main()
{
    RWindow::Format format;
    format.decorate = false;
    format.keysSigal = true;
    format.initWidth = 500;
    format.initHeight = 500;
    format.background = 0x181010;
    RWindow window(format, nullptr, "Triangle");
    p = &window;

    TestCtl t(&window, "ChildNode");

    window.entered.connect(observeKeyboard);

    window.show();
    return window.exec();
}
