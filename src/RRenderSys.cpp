#include <RRenderSys.h>
#include <RModelMat.h>
#include <rsc/RTexture.h>

using namespace Redopera;

static const GLchar *VERTEX_CODE =
R"--(
#version 330 core

layout(location = 0) in vec2 POS;
layout(location = 1) in vec2 TEX_COOR;

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECT;

out vec2 texCoor;

void main(void)
{
    texCoor = TEX_COOR;
    gl_Position = PROJECT * VIEW * MODEL* vec4(POS, 0.0, 1.0);
}
)--";

static const GLchar *FRAGMENT_CODE =
R"--(
#version 430 core

uniform vec4 HUE = vec4(1, 1, 1, 1);
uniform sampler2D TEX;

in vec2 texCoor;
out vec4 outColor;

subroutine void ColorOutFunc();

subroutine(ColorOutFunc) void texOut()
{
    outColor = texture(TEX, texCoor) * HUE;
}

subroutine(ColorOutFunc) void singleTexOut()
{
    outColor = vec4(HUE.rgb, texture(TEX, texCoor) * HUE.a);
}

subroutine(ColorOutFunc) void hueOut()
{
    outColor = HUE;
}

subroutine uniform ColorOutFunc colorOutFunc;

void main(void)
{
    colorOutFunc();
}
)--";

const RName RRenderSys::PROJECT = "PROJECT";
const RName RRenderSys::VIEW = "VIEW";
const RName RRenderSys::MODEL = "MODEL";
const RName RRenderSys::HUE = "HUE";

const RName RRenderSys::COLOR_OUT_FUNC = "colorOutFunc";
const RName RRenderSys::TEX_OUT = "texOut";
const RName RRenderSys::SINGLE_OUT = "singleTexOut";
const RName RRenderSys::HUE_OUT = "hueOut";

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

    registerUniform({ PROJECT, VIEW, MODEL, HUE });
    registerSubroutine(RShader::Type::Fragment, COLOR_OUT_FUNC, { TEX_OUT, SINGLE_OUT, HUE_OUT });
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
    rpi.setUniformMat(loc(PROJECT), glm::ortho(left, right, bottom, top, near, far));
}

void RRenderSys::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(PROJECT), glm::perspective(left, right, bottom, top, near, far));
}

void RRenderSys::setProjectMat(const glm::mat4 &mat)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(PROJECT), mat);
}

void RRenderSys::setViewMove()
{
    setViewMove(0, 0, 0);
}

void RRenderSys::setViewMove(float x, float y, float z)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(VIEW), glm::translate(glm::mat4(1), { -x, -y, -z }));
}

void RRenderSys::setViewMat(const glm::mat4 &mat)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(VIEW), mat);
}

void RRenderSys::setHue(unsigned r, unsigned g, unsigned b, unsigned a)
{
    setHue(glm::vec4(r/255.f, g/255.f, b/255.f, a/255.f));
}

void RRenderSys::setHue(const glm::vec4 &color)
{
    RRPI rpi = data_.shaders.use();
    rpi.setUniform(loc(HUE), color);
}

void RRenderSys::setHue(const glm::vec3 &color)
{
    setHue(glm::vec4(color, 1.f));
}

void RRenderSys::usingTexColorOut()
{
    setSubroutine(RShader::Type::Fragment, COLOR_OUT_FUNC, TEX_OUT);
}

void RRenderSys::usingSingleTexOut()
{
    setSubroutine(RShader::Type::Fragment, COLOR_OUT_FUNC, SINGLE_OUT);
}

void RRenderSys::usingHueOut()
{
    setSubroutine(RShader::Type::Fragment, COLOR_OUT_FUNC, HUE_OUT);
}

void RRenderSys::setHue(RColor color)
{
    setHue(glm::vec4(color.r()/255.f, color.g()/255.f, color.b()/255.f, color.a()/255.f));
}

void RRenderSys::render(const RTexture &tex, const glm::mat4 &model) const
{
    bindVAO();
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(MODEL), model);
    tex.bind();
    drawPlane();
}

void RRenderSys::renderLine(const glm::mat4 &mat)
{
    bindVAO();
    RRPI rpi = data_.shaders.use();
    rpi.setUniformMat(loc(MODEL), mat);
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
