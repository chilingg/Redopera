#include <RRenderSys.h>

using namespace Redopera;

const GLchar *VERTEX_CODE =
R"--(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
out vec2 texCoor;
void main(void)
{
    texCoor = aTexCoor;
    gl_Position = project * view * model* vec4(aPos, 1.0);
}
)--";

const GLchar *FRAGMENT_CODE =
R"--(
#version 330 core
uniform sampler2D tex;
in vec2 texCoor;
out vec4 outColor;
void main(void)
{
    outColor = texture(tex, texCoor);
}
)--";

const GLchar *SINGLE_TEXTURE_CODE =
R"--(
#version 330 core
uniform sampler2D tex;
uniform vec3 color = vec3(1., 1., 1.);
uniform vec4 background = vec4(0., 0., 0., 0.);
in vec2 texCoor;
out vec4 outColor;
void main(void)
{
    outColor = vec4(color.rgb, texture(tex, texCoor).r);
    outColor += background * (1 - outColor.a);
}
)--";

RShaders RRenderSys::createSimpleShaders()
{
    return { RShader(VERTEX_CODE, RShader::Type::Vertex), RShader(FRAGMENT_CODE, RShader::Type::Fragment) };
}

RShaders RRenderSys::createSimpleSingleShaders()
{
    return { RShader(VERTEX_CODE, RShader::Type::Vertex), RShader(SINGLE_TEXTURE_CODE, RShader::Type::Fragment) };
}

RRenderSys::RRenderSys()
{
    initialize();
}

RRenderSys::RRenderSys(const std::string &name, const RShaders &shaders):
    RRenderSys(name, shaders, shaders.uniformLoccal("project"), shaders.uniformLoccal("view"), shaders.uniformLoccal("model"))
{

}

RRenderSys::RRenderSys(const std::string &name, const RShaders &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc):
    mainShaders_(name)
{
    initialize();
    addShaders(name, shaders, pLoc, vLoc, mLoc);
}

RRenderSys::~RRenderSys()
{
    glDeleteVertexArrays(2, &vao_);
    glDeleteBuffers(2, &vbo_);
}

GLuint RRenderSys::projectLocal() const
{
    return renderers_.at(mainShaders_).pLoc_;
}

GLuint RRenderSys::viewLocal() const
{
    return renderers_.at(mainShaders_).vLoc_;
}

GLuint RRenderSys::modelLocal() const
{
    return renderers_.at(mainShaders_).mLoc_;
}

const RShaders &RRenderSys::shaders() const
{
    return renderers_.at(mainShaders_).shaders_;
}

const RShaders *RRenderSys::queryShaders(const std::string &name) const
{
    auto it = renderers_.find(name);
    if(it == renderers_.end())
        return nullptr;
    return &it->second.shaders_;
}

void RRenderSys::setMainShaders(const std::string &name)
{
    mainShaders_ = name;
}

std::string RRenderSys::addShaders(const std::string &name, const RShaders &shaders)
{
    return addShaders(name, shaders, shaders.uniformLoccal("project"), shaders.uniformLoccal("view"), shaders.uniformLoccal("model"));
}

std::string RRenderSys::addShaders(const std::string &name, const RShaders &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc)
{
    std::string n = availableName(name);
    renderers_[n] = { pLoc, vLoc, mLoc, shaders };

    return n;
}

void RRenderSys::removeShaders(const std::string &name)
{
    renderers_.erase(name);
}

void RRenderSys::setViewprot(float left, float right, float bottom, float top, float near, float far)
{
    glm::mat4 mat = glm::ortho(left, right, bottom, top, near, far);

    for(auto& [name, renderer] : renderers_)
    {
        RRPI rpi = renderer.shaders_.use();
        rpi.setUniformMat(renderer.pLoc_, mat);
    }
}

void RRenderSys::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    glm::mat4 mat = glm::perspective(left, right, bottom, top, near, far);
    for(auto& [name, renderer] : renderers_)
    {
        RRPI rpi = renderer.shaders_.use();
        rpi.setUniformMat(renderer.pLoc_, mat);
    }
}

void RRenderSys::setProjectionMat(const glm::mat4 &mat)
{
    for(auto& [name, renderer] : renderers_)
    {
        RRPI rpi = renderer.shaders_.use();
        rpi.setUniformMat(renderer.pLoc_, mat);
    }
}

void RRenderSys::setCamera(float x, float y, float z)
{
    glm::mat4 mat = glm::translate(glm::mat4(1), {-x, -y, -z});
    for(auto& [name, renderer] : renderers_)
    {
        RRPI rpi = renderer.shaders_.use();
        rpi.setUniformMat(renderer.vLoc_, mat);
    }
}

void RRenderSys::setCamera()
{
    setCamera(0.f, 0.f, 0.f);
}

void RRenderSys::setViewMat(const glm::mat4 &mat)
{
    for(auto& [name, renderer] : renderers_)
    {
        RRPI rpi = renderer.shaders_.use();
        rpi.setUniformMat(renderer.vLoc_, mat);
    }
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

void RRenderSys::renderLine(const glm::mat4 &mat)
{
    glBindVertexArray(lineVao_);

    RRPI inter = shaders().use();
    inter.setUniformMat(modelLocal(), mat);
    lineColor_.bind();
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
}

void RRenderSys::renderLine(const RRect &rect)
{
    glBindVertexArray(lineVao_);

    glm::mat4 model_ = glm::translate(glm::mat4(1), { rect.left() + rect.width()/2, rect.bottom() + rect.height()/2, 0 });
    model_ = glm::scale(model_, { rect.width(), rect.height(), 0.0f });

    RRPI inter = shaders().use();
    inter.setUniformMat(modelLocal(), model_);
    lineColor_.bind();
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
}

void RRenderSys::initialize()
{
    glGenVertexArrays(2, &vao_); // vao_ & lineVao_
    glGenBuffers(2, &vbo_); // vbo_ & lineVbo

    float plane[]{
            0.5f,-0.5f, 1.0f, 0.0f,//右下
            0.5f, 0.5f, 1.0f, 1.0f,//右上
           -0.5f,-0.5f, 0.0f, 0.0f,//左下
           -0.5f, 0.5f, 0.0f, 1.0f,//左上
    };

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), R_BUFF_OFF(2*sizeof(float)));
    glBindVertexArray(0);

    float edge[]{
             .5f, -.5f, //右下
             .5f,  .5f, //右上
            -.5f,  .5f, //左上
            -.5f, -.5f, //左下
    };

    glBindVertexArray(lineVao_);
    glBindBuffer(GL_ARRAY_BUFFER, LineVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edge), edge, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindVertexArray(0);

    lineColor_ = RTexture::createRedTex();
}

std::string RRenderSys::availableName(const std::string &name) const
{
    std::string an = name;
    auto it = renderers_.find(an);
    for (size_t n = 1; it != renderers_.end(); ++n)
    {
       an = name + std::to_string(n);
       it = renderers_.find(an);
    }

    return an;
}
