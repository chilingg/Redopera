#include <RGame.h>
#include <render/RContext.h>
#include <render/RShader.h>
#include <render/RProgram.h>

#include <SDL2/SDL.h>
#include <fmt/core.h>

using namespace Redopera;

const char *vCode =
        "#version 330\n"

        "in float inValue;\n"
        "out float outValue;\n"

        "void main(void)\n"
        "{\n"
        "    outValue = sqrt(inValue);\n"
        "}\n";

int main()
{
    RGame game(SDL_INIT_VIDEO);
    RWindow window(1, 1, "OpenGL Feedback Test", SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    RContext::Format fmt;
    fmt.debug = true;
    fmt.forward = true;
    fmt.rasterizer = false;
    fmt.vSync = false;
    fmt.versionMajor = 4;
    fmt.versionMinor = 3;
    RContext context(window, fmt);

    RShader vertex(vCode, RShader::Type::Vertex);
    RProgram renderProg({vertex});

    const char * varyings[] = { "outValue" };
    glTransformFeedbackVaryings(renderProg.id(), 1, varyings, GL_INTERLEAVED_ATTRIBS);

    renderProg.reLinkProgram();
    // Interface实例存在期间，对应程序都处于using状态
    RRPI interface = renderProg.use();
    GLuint program = renderProg.id();
    fmt::print("Number: \t1 \t 2 \t 3 \t 4 \t 5\n");

    // ================ OpenGL ================
    // Reference: https://open.gl/feedback

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create input VBO and vertex format
    GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    GLint inputAttrib = glGetAttribLocation(program, "inValue");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

    // Create transform feedback buffer
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);

    // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 5);
    glEndTransformFeedback();

    glDisable(GL_RASTERIZER_DISCARD);

    glFlush();

    // Fetch and print results
    GLfloat feedback[5];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

    fmt::print("Result:\t{}\t{}\t{}\t{}\t{}\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4]);

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
