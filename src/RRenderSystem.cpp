#include <RRenderSystem.h>
#include <RPlane.h>
#include <RTextbox.h>

using namespace Redopera;

template void RRenderSystem::render<RPlane>(RPlane &n);
template void RRenderSystem::renderLine<RPlane>(RPlane &n);

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
