#include <render/RRenderer.h>
#include <RColor.h>

using namespace Redopera;

namespace  {

const GLchar *VERTEX_CODE =
R"--(
#version 430 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex_coor;

uniform mat4 model;
uniform mat4 view = mat4(1);
uniform mat4 project;

out vec2 tex_coor;

void main(void)
{
    tex_coor = in_tex_coor;
    gl_Position = project * view * model* vec4(in_pos, 0.0, 1.0);
}
)--";

const GLchar *FRAGMENT_CODE =
R"--(
#version 430 core

uniform sampler2D tex;
uniform vec4 hue = vec4(1, 1, 1, 1);

in vec2 tex_coor;
out vec4 out_color;

void main(void)
{
    out_color = texture(tex, tex_coor) * hue;
}
)--";

} // ns

RProgram RRenderer::createSimpleProgram()
{
    return { RShader(VERTEX_CODE, RShader::Type::Vertex), RShader(FRAGMENT_CODE, RShader::Type::Fragment) };
}

void RRenderer::createPlaneVAO(GLuint &vao, GLuint &vbo)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    GLfloat plane[]{
            0.5f,-0.5f, 1.0f, 0.0f, //右下
            0.5f, 0.5f, 1.0f, 1.0f, //右上
           -0.5f,-0.5f, 0.0f, 0.0f, //左下
           -0.5f, 0.5f, 0.0f, 1.0f, //左上

            0.5f, 0.5f, 1.0f, 1.0f, //右上 不知为和1.0采集不到纹素
            0.5f,-0.5f, 1.0f, 0.0f, //右下
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), R_BUFF_OFF(2*sizeof(GLfloat)));
    glBindVertexArray(0);
}

void RRenderer::setBackgroundColor(RGBA rgba)
{
    RColor::ColorData color;
    color.rgba = rgba;
    glClearColor(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.f);
}

RRenderer::RRenderer():
    RRenderer(createSimpleProgram())
{

}

RRenderer::RRenderer(const RProgram &program):
    program_(program)
{
    createPlaneVAO(vao_, vbo_);
    setProgram(program);
}

RRenderer::~RRenderer()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void RRenderer::bindVAO() const
{
    glBindVertexArray(vao_);
}

void RRenderer::unbindVAO() const
{
    glBindVertexArray(0);
}

void RRenderer::draw() const
{
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RRenderer::drawLine() const
{
    glDrawArrays(GL_LINE_LOOP, 2, 4);
}

void RRenderer::setProgram(const RProgram &program)
{
    program_ = program;
}
