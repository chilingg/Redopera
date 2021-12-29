#include <render/ROpenGL.h>
#include <render/RProgram.h>
#include <render/RContext.h>

#include <RGame.h>
#include <RWindow.h>
#include <RFormat.h>
#include <RKeeper.h>

#include <SDL2/SDL.h>

using namespace Redopera;

template <typename T>
using RKeeperF = RKeeper<T, std::function<void(T)>>;

const char *vCode =
        "#version 330\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projecte;\n"
        "uniform mat4 model;\n"
        "out vec3 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projecte * model * vec4(aPos, 1.0);\n"
        "   float n = abs(aPos.x / 20);\n"
        "   color = vec3(abs(n - 1), 0, n - (gl_Position.z * 2.5));\n"
        "}\n";

const char *fCode =
        "#version 330\n"
        "in vec3 color;\n"
        "out vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "   fragColor = vec4(color, 1.0);\n"
        "}\n";

class Triangle
{
public:
    Triangle():
        shaders({RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment)}),
        model(glm::mat4(1))
    {}

    void update()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        model = glm::rotate(model, 0.05f, { 0.0f, 1.0f, 0.0f });
        auto itfc = shaders.use();
        itfc.setUniformMat(modelLoc, model);
        glBindVertexArray(VAO.get());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void start()
    {
        glClearColor( .1f, 0.f, 0.f, 1.f );

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
        intf.setUniformMat(projection, math::perspective({ -30.0f, 0.0f, 60.0f, 60.0f }, 0.0f, 1500.0f));
    }

private:
    RKeeperF<GLuint> VAO, VBO;
    RProgram shaders;
    GLuint modelLoc;
    glm::mat4 model;
};

int main(int arg, char **argv)
{
    RGame game(SDL_INIT_VIDEO);
    RWindow window(540, 540, "OpenGL Test", SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    RContext context(window);

    Triangle triangle;
    triangle.start();

    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        triangle.update();
        window.swapBuffers();
    }

   return 0;
}
