#include <RRenderSys.h>
#include <RModelMat.h>
#include <rsc/RTexture.h>

using namespace Redopera;

static const GLchar *VERTEX_CODE =
R"--(
#version 430 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex_coor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

out vec2 tex_coor;

void main(void)
{
    tex_coor = in_tex_coor;
    gl_Position = project * view * model* vec4(in_pos, 0.0, 1.0);
}
)--";

static const GLchar *FRAGMENT_CODE =
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

const RName RRenderSys::project = "project";
const RName RRenderSys::view = "view";
const RName RRenderSys::model = "model";
const RName RRenderSys::hue = "hue";

RShaders RRenderSys::createSimpleShaders()
{
    return { RShader(VERTEX_CODE, RShader::Type::Vertex), RShader(FRAGMENT_CODE, RShader::Type::Fragment) };
}

void RRenderSys::createPlaneVAO(GLuint &vao, GLuint &vbo)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    GLfloat plane[]{
            0.5f,-0.5f, 1.0f, 0.0f,//右下
            0.5f, 0.5f, 1.0f, 1.0f,//右上
           -0.5f,-0.5f, 0.0f, 0.0f,//左下
           -0.5f, 0.5f, 0.0f, 1.0f,//左上

            0.5f, 0.5f, 0.999f, 0.999f,//右上 不知为和1.0采集不到纹素
            0.5f,-0.5f, 1.0f, 0.0f,//右下
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

RRenderSys::RRenderSys():
    RRenderSys(createSimpleShaders())
{
    registerUniform({ project, view, model, hue });
}

RRenderSys::RRenderSys(const RShaders &shaders):
    white_(RTexture::createWhiteTex())
{
    createPlaneVAO(vao_, vbo_);
    setShaders(shaders);
}

RRenderSys::~RRenderSys()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

bool RRenderSys::isUniform(RName name)
{
    return uniform_.count(name);
}

GLint RRenderSys::loc(RName name) const
{
    return uniform_.at(name);
}

const RDict<GLuint> &RRenderSys::locList() const
{
    return uniform_;
}

const RShaders &RRenderSys::shaders() const
{
    return shaders_;
}

GLuint RRenderSys::vao() const
{
    return vao_;
}

void RRenderSys::bindVAO() const
{
    glBindVertexArray(vao_);
}

void RRenderSys::unbindVAO() const
{
    glBindVertexArray(0);
}

void RRenderSys::drawPlane() const
{
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RRenderSys::drawLine() const
{
    glDrawArrays(GL_LINE_LOOP, 2, 4);
}

void RRenderSys::registerUniform(std::initializer_list<RName> list)
{
    for(const RName &name : list)
    {
        GLint loc = shaders_.getUniformLoc(name.toString());
        if(loc < 0)
            return;
        uniform_[name] = loc;
    }
}

void RRenderSys::registerUniform(const std::string &uniform, RName name)
{
    GLint loc = shaders_.getUniformLoc(uniform);
    if(loc < 0)
        return;
    uniform_[name] = loc;
}

void RRenderSys::setShaders(const RShaders &shaders)
{
    uniform_.clear();
    shaders_ = shaders;
}

void RRenderSys::setViewport(float left, float right, float bottom, float top, float near, float far)
{
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(project), glm::ortho(left, right, bottom, top, near, far));
}

void RRenderSys::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(project), glm::perspective(left, right, bottom, top, near, far));
}

void RRenderSys::setProjectMat(const glm::mat4 &mat)
{
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(project), mat);
}

void RRenderSys::setViewMove()
{
    setViewMove(0, 0, 0);
}

void RRenderSys::setViewMove(float x, float y, float z)
{
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(view), glm::translate(glm::mat4(1), { -x, -y, -z }));
}

void RRenderSys::setViewMat(const glm::mat4 &mat)
{
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(view), mat);
}

void RRenderSys::setHue(unsigned r, unsigned g, unsigned b, unsigned a)
{
    setHue(glm::vec4(r/255.f, g/255.f, b/255.f, a/255.f));
}

void RRenderSys::setHue(const glm::vec4 &color)
{
    RRPI rpi = shaders_.use();
    rpi.setUniform(loc(hue), color);
}

void RRenderSys::setHue(const glm::vec3 &color)
{
    setHue(glm::vec4(color, 1.f));
}

void RRenderSys::setHue(RColor color)
{
    setHue(glm::vec4(color.r()/255.f, color.g()/255.f, color.b()/255.f, color.a()/255.f));
}

void RRenderSys::render(const RTexture &tex, const glm::mat4 &model) const
{
    bindVAO();
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(RRenderSys::model), model);
    tex.bind();
    drawPlane();
}

void RRenderSys::renderLine(const glm::mat4 &mat)
{
    bindVAO();
    RRPI rpi = shaders_.use();
    rpi.setUniformMat(loc(model), mat);
    white_.bind();
    drawLine();
}

void RRenderSys::renderLine(const RRect &rect)
{
    RModelMat mat(rect);
    renderLine(mat.model());
}
