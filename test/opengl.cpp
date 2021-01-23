#include <RGame.h>
#include <RContext.h>
#include <RShaders.h>
#include <RDebug.h>

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
    RGame game;

    rDebug << "======== Using OpenGL Transform feedback to sqrt calculat ========\n";

    RContext::Format format;
    format.versionMajor = 3;
    format.versionMinor = 3;
    RContext context(format);

    if(rCheck(!context, "Failure initialization OpenGL context!"))
        exit(EXIT_FAILURE);
    else
        rDebug << "OpenGl Context " << GLVersion.major << '.' << GLVersion.minor << " created\n";

    RShader vertex(vCode, RShader::Type::Vertex);
    RShaders renderProg({vertex});

    const char * varyings[] = { "outValue" };
    glTransformFeedbackVaryings(renderProg.id(), 1, varyings, GL_INTERLEAVED_ATTRIBS);

    renderProg.reLinkProgram();
    // Interface实例存在期间，对应程序都处于using状态
    RRPI interface = renderProg.use();
    GLuint program = renderProg.id();
    rDebug << "Number: \t1 \t 2 \t 3 \t 4 \t 5";

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

    rDebug << "Result:\t" << feedback[0] << '\t' << feedback[1] << '\t'
           << feedback[2] << '\t' << feedback[3] << '\t' << feedback[4];

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
