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

uniform vec4 hue = vec4(1, 1, 1, 1);
uniform sampler2D tex;

in vec2 tex_coor;
out vec4 out_color;

subroutine void color_out();

subroutine(color_out) void tex_out()
{
    out_color = texture(tex, tex_coor) * hue;
}

subroutine(color_out) void single_tex_out()
{
    out_color = vec4(hue.rgb, texture(tex, tex_coor) * hue.a);
}

subroutine(color_out) void hue_out()
{
    out_color = hue;
}

subroutine uniform color_out color_out_func;

void main(void)
{
    color_out_func();
}
)--";

const RName RRenderSys::nProject = "project";
const RName RRenderSys::nView = "view";
const RName RRenderSys::nModel = "model";
const RName RRenderSys::nHue = "hue";

const RName RRenderSys::nColorOutFunc = "color_out_func";
const RName RRenderSys::nTexOut = "tex_out";
const RName RRenderSys::nSingleOut = "single_tex_out";
const RName RRenderSys::nHueOut = "hue_out";

RShaders RRenderSys::createSimpleShaders()
{
    return { RShader(VERTEX_CODE, RShader::Type::Vertex), RShader(FRAGMENT_CODE, RShader::Type::Fragment) };
}

RRenderSys::RRenderSys()
{
    createVAO();
}

RRenderSys::RRenderSys(const RShaders &shaders):
    RRenderSys()
{
    setShaders(shaders);
}

RRenderSys::~RRenderSys()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

bool RRenderSys::isUniform(const RName &name)
{
    return data_.uniform.count(name);
}

GLint RRenderSys::loc(const RName &name) const
{
    return data_.uniform.at(name);
}

const std::unordered_map<RName, GLuint> &RRenderSys::locList() const
{
    return data_.uniform;
}

const std::unordered_map<RShader::Type, RRenderSys::SubroutineData> &RRenderSys::stageData() const
{
    return data_.stage_;
}

const RShaders &RRenderSys::shaders() const
{
    return data_.shaders;
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
        GLint loc = data_.shaders.getUniformLoc(name);
        if(loc < 0)
            return;
        data_.uniform[name] = loc;
    }
}

bool RRenderSys::registerSubroutine(RShader::Type type, const RName &func, std::initializer_list<RName> list)
{
    GLint loc = data_.shaders.getSubroutineUniformLoc(type, func);
    if(loc < 0)
        return false;
    data_.stage_[type].data[func].uniform = loc;

    for(const RName& name : list)
    {
        GLuint i = data_.shaders.getSubroutineIndex(type, name);
        if(i != GL_INVALID_INDEX)
            data_.stage_[type].data[func].index[name] = i;
    }

    return true;
}

void RRenderSys::setShaders(const RShaders &shaders)
{
    data_ = { shaders, {}, {} };

    GLint n;
    for(RShader::Type type : RShader::TYPE_LIST)
    {
        glGetProgramStageiv(data_.shaders.id(), static_cast<GLenum>(type), GL_ACTIVE_SUBROUTINES, &n);
        if(n > 0)
            data_.stage_.emplace(type, SubroutineData{ std::vector<GLuint>(n, 0), {} });
    }

    registerUniform({ nProject, nView, nModel, nHue });
    registerSubroutine(RShader::Type::Fragment, nColorOutFunc, { nTexOut, nSingleOut, nHueOut });
    setViewMove();
}

void RRenderSys::setSubroutine(RShader::Type type, const RName &func, const RName &opt)
{
    GLuint loc = data_.stage_.at(type).data.at(func).uniform;
    std::vector<GLuint> &table = data_.stage_.at(type).table;
    table[loc] = data_.stage_.at(type).data.at(func).index.at(opt);
    RRPI rpi = data_.shaders.use();
    rpi.setUniformSubroutines(type, loc+1, table.data());
}

void RRenderSys::setViewport(float left, float right, float bottom, float top, float near, float far)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nProject), glm::ortho(left, right, bottom, top, near, far));
}

void RRenderSys::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nProject), glm::perspective(left, right, bottom, top, near, far));
}

void RRenderSys::setProjectMat(const glm::mat4 &mat)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nProject), mat);
}

void RRenderSys::setViewMove()
{
    setViewMove(0, 0, 0);
}

void RRenderSys::setViewMove(float x, float y, float z)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nView), glm::translate(glm::mat4(1), { -x, -y, -z }));
}

void RRenderSys::setViewMat(const glm::mat4 &mat)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nView), mat);
}

void RRenderSys::setHue(unsigned r, unsigned g, unsigned b, unsigned a)
{
    setHue(glm::vec4(r/255.f, g/255.f, b/255.f, a/255.f));
}

void RRenderSys::setHue(const glm::vec4 &color)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniform(loc(nHue), color);
}

void RRenderSys::setHue(const glm::vec3 &color)
{
    setHue(glm::vec4(color, 1.f));
}

void RRenderSys::usingTexColorOut()
{
    setSubroutine(RShader::Type::Fragment, nColorOutFunc, nTexOut);
}

void RRenderSys::usingSingleTexOut()
{
    setSubroutine(RShader::Type::Fragment, nColorOutFunc, nSingleOut);
}

void RRenderSys::usingHueOut()
{
    setSubroutine(RShader::Type::Fragment, nColorOutFunc, nHueOut);
}

void RRenderSys::setHue(RColor color)
{
    setHue(glm::vec4(color.r()/255.f, color.g()/255.f, color.b()/255.f, color.a()/255.f));
}

void RRenderSys::render(const RTexture &tex, const glm::mat4 &model) const
{
    bindVAO();
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nModel), model);
    tex.bind();
    drawPlane();
}

void RRenderSys::renderLine(const glm::mat4 &mat)
{
    bindVAO();
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(nModel), mat);
    drawLine();
}

void RRenderSys::renderLine(const RRect &rect)
{
    RModelMat mat(rect);
    renderLine(mat.model());
}

void RRenderSys::createVAO()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    GLfloat plane[]{
            0.5f,-0.5f, 1.0f, 0.0f,//右下
            0.5f, 0.5f, 1.0f, 1.0f,//右上
           -0.5f,-0.5f, 0.0f, 0.0f,//左下
           -0.5f, 0.5f, 0.0f, 1.0f,//左上

            0.5f, 0.5f, 1.0f, 1.0f,//右上 不知为和1.0采集不到纹素
            0.5f,-0.5f, 1.0f, 0.0f,//右下
    };

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), R_BUFF_OFF(2*sizeof(GLfloat)));
    glBindVertexArray(0);
}
