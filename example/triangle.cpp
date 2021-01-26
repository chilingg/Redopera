#include <RGame.h>
#include <RWindow.h>
#include <RInput.h>
#include <RKeeper.h>

using namespace Redopera;

const char *vCode =
        "#version 330\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projecte;\n"
        "uniform mat4 model;\n"
        "out vec3 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projecte * model * vec4(aPos, 1.0);\n"
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
        node("TestCtl", this),
        shaders({RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)}),
        model(glm::mat4(1))
    {
        node.setUpdateFunc([this](RRenderSys *sys){ update(sys); });
        node.setStartFunc([this]{ startEvent(); });
        node.setProcessFunc([this](RNode *sender, RNode::Instructs*ins){ processEvent(sender, ins); });
    }

    void update(RRenderSys *)
    {
        model = glm::rotate(model, 0.05f, { 0.0f, 1.0f, 0.0f });
        auto itfc = shaders.use();
        itfc.setUniformMat(modelLoc, model);
        glBindVertexArray(VAO.get());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void startEvent()
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

        GLuint projection = shaders.getUniformLoc("projecte");
        modelLoc = shaders.getUniformLoc("model");

        // Interface生存周期内对应的shader program都处于glUseProgram()调用中，析构时自动glUseProgram(0);
        RRPI intf = shaders.use();
        intf.setUniformMat(modelLoc, model);
        intf.setUniformMat(projection, glm::perspective(-30.0f, 30.0f, 0.0f, 60.0f, 0.0f, 1500.0f));
    }

    void processEvent(RNode *sender, RNode::Instructs*)
    {
        // inputEvent只能监测感兴趣的按键
        if(RInput::input().anyKeyPress())
            sender->breakLooping();
    }

    RNode node;

private:
    RKeeper<GLuint> VAO, VBO;
    RShaders shaders;
    GLuint modelLoc;
    glm::mat4 model;
};

int main()
{
    RGame game;

    RWindow::Format format;
    format.debug = false;
    format.decorate = false;
    format.background = 0x101018ff;
    RWindow window(500, 500, "Triangle", format);
    // Rwindow默认的transformFunc会对RRenderSys设置Viewport和向下传递Transform，在此两者都不需要
    window.node.setTransformFunc([](RNode*, const RTransform&){});

    TestCtl t;
    t.node.changeParent(&window.node);

    window.show();
    return window.node.exec();
}
