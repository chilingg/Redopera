#include <RRenderSystem.h>
#include <RPlane.h>
#include <RTextbox.h>
#include <RSprite.h>
#include <RRect.h>

using namespace Redopera;

RRenderSystem::RRenderSystem():
    lineColor_(RTexture::redTex())
{
    glGenVertexArrays(2, vao_.data());
    glGenBuffers(2, vbo_.data());

    float plane[]{
            0.5f,-0.5f, 1.0f, 0.0f,//右下
            0.5f, 0.5f, 1.0f, 1.0f,//右上
           -0.5f,-0.5f, 0.0f, 0.0f,//左下
           -0.5f, 0.5f, 0.0f, 1.0f,//左上
    };

    glBindVertexArray(vao_[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), R_BUFF_OFF(2*sizeof(float)));
    glBindVertexArray(0);

    float edge[]{
             .5f, -.5f,//右下
             .5f,  .5f,//右上
            -.5f,  .5f,//左上
            -.5f, -.5f,//左下
    };

    glBindVertexArray(vao_[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edge), edge, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindVertexArray(0);
}

RRenderSystem::~RRenderSystem()
{
    glDeleteVertexArrays(2, vao_.data());
    glDeleteBuffers(2, vbo_.data());
}

void RRenderSystem::setShaderProg(const RShaderProg &shaders)
{
    shaders_ = shaders;
    pLoc_ = shaders_.getUniformLocation("projection");
    vLoc_ = shaders_.getUniformLocation("view");
    mLoc_ = shaders_.getUniformLocation("model");
}

void RRenderSystem::setShaderProg(const RShaderProg &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc)
{
    shaders_ = shaders;
    pLoc_ = pLoc;
    vLoc_ = vLoc;
    mLoc_ = mLoc;
}

void RRenderSystem::setViewprot(float left, float right, float bottom, float top, float near, float far)
{
    pMat_ = glm::ortho(left, right, bottom, top, near, far);
    RInterface itfc = shaders_.useInterface();
    itfc.setUniformMatrix(pLoc_, 4, glm::value_ptr(pMat_));
}

void RRenderSystem::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    pMat_ = glm::perspective(left, right, bottom, top, near, far);
    RInterface itfc = shaders_.useInterface();
    itfc.setUniformMatrix(pLoc_, 4, glm::value_ptr(pMat_));
}

void RRenderSystem::setCameraMove(float x, float y, float z)
{
    vMat_ = glm::translate(glm::mat4(1), {-x, -y, -z});
    RInterface itfc = shaders_.useInterface();
    itfc.setUniformMatrix(vLoc_, 4, glm::value_ptr(vMat_));
}

void RRenderSystem::setCameraMove()
{
    setCameraMove(0, 0, 0);
}

void RRenderSystem::setLineColor(const RColor &color)
{
    lineColor_ = RTexture::colorTexture(color);
}

void RRenderSystem::setProjectionMat(const glm::mat4 &mat)
{
    pMat_ = mat;
}

void RRenderSystem::setViewMat(const glm::mat4 &mat)
{
    vMat_ = mat;
}

const glm::mat4 &RRenderSystem::getProjectionMat() const
{
    return pMat_;
}

const glm::mat4 &RRenderSystem::getViewMat() const
{
    return vMat_;
}

const RShaderProg &RRenderSystem::shaderProg()
{
    return shaders_;
}

const RTexture &RRenderSystem::texture()
{
    return lineColor_;
}

GLuint RRenderSystem::projectionLocal()
{
    return pLoc_;
}

GLuint RRenderSystem::viewLocal()
{
    return vLoc_;
}

GLuint RRenderSystem::modelLocal()
{
    return mLoc_;
}

void RRenderSystem::bindVAO()
{
    glBindVertexArray(vao_[0]);
}

void RRenderSystem::bindLineVAO()
{
    glBindVertexArray(vao_[1]);
}

void RRenderSystem::unbindVAO()
{
    glBindVertexArray(0);
}

void RRenderSystem::drawRect()
{
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RRenderSystem::drawLine()
{
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void RRenderSystem::renderLine(const glm::mat4 &mat)
{
    glBindVertexArray(vao_[1]);

    RInterface inter = shaders_.useInterface();
    inter.setUniformMatrix(mLoc_, mat);
    lineColor_.bind();
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
}

void RRenderSystem::renderLine(const RRect &rect)
{
    glBindVertexArray(vao_[1]);

    glm::mat4 model_ = glm::translate(glm::mat4(1), { rect.left() + rect.width()/2, rect.bottom() + rect.height()/2, 0 });
    model_ = glm::scale(model_, { rect.width(), rect.height(), 0.0f });

    RInterface inter = shaders_.useInterface();
    inter.setUniformMatrix(mLoc_, model_);
    lineColor_.bind();
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
}
