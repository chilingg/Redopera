#include <RGame.h>
#include <RWindow.h>
#include <RInputModule.h>
#include <RController.h>
#include <RKeeper.h>
#include <REvent.h>
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

class TestCtl
{
public:
    TestCtl():
        ctrl("TestCtl", this),
        shaders({RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)}),
        model(glm::mat4(1))
    {
        ctrl.setUpdataFunc(std::bind(&TestCtl::control, this));
        ctrl.setStartFunc(std::bind(&TestCtl::startEvent, this, std::placeholders::_1));
        ctrl.setFinishFunc(std::bind(&TestCtl::finishEvent, this, std::placeholders::_1));
        ctrl.setProcessFunc(std::bind(&TestCtl::inputEvent, this, std::placeholders::_1));
    }

    void control()
    {
        model = glm::rotate(model, 0.05f, { 0.0f, 1.0f, 0.0f });
        auto itfc = shaders.useInterface();
        itfc.setUniformMatrix(modelLoc, model);
        glBindVertexArray(VAO.get());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void startEvent(StartEvent*)
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), R_BUFF_OFF(0));
        glEnableVertexAttribArray(0);

        GLuint projection = shaders.getUniformLocation("projection");
        modelLoc = shaders.getUniformLocation("model");

        // Interface生存周期内对应的shader program都处于glUseProgram()调用中，析构时自动glUseProgram(0);
        RInterface intf = shaders.useInterface();
        intf.setUniformMatrix(modelLoc, model);
        intf.setUniformMatrix(projection, glm::perspective(-30.0f, 30.0f, 0.0f, 60.0f, 0.0f, 1500.0f));
    }

    void finishEvent(FinishEvent*)
    {
        // finish事件在exce()退出时发起
        shaders.release(); // 显式释放不是必须的，再次只是示范
        VAO.reset();
        VBO.reset();
    }

    void inputEvent(ProcessEvent *e)
    {
        // inputEvent只能监测感兴趣的按键
        if(e->input->anyKeyPress())
            ctrl.breakLoop();
    }

    RController ctrl;

private:
    RKeeper<GLuint> VAO, VBO;
    RShaderProg shaders;
    GLuint modelLoc;
    glm::mat4 model;
};

RWindow *p; // entered信号才能实时监测按键事件
bool observeKeyboard(Keys, BtnAct)
{
    p->closeWindow();
    return true;
}

int main()
{
    RGame game;

    RWindow::Format format;
    format.decorate = false;
    format.background = 0x101018ff;
    RWindow window(500, 500, "Triangle", format);
    p = &window;

    TestCtl t;
    t.ctrl.changeParent(window.ctrl());

    window.show();
    return game.exec(&window);
}
