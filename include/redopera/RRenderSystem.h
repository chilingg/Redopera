#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "rsc/RShaderProg.h"
#include "rsc/RTexture.h"

namespace Redopera {

class RPlane;

class RRenderSystem
{
public:
    RRenderSystem();
    ~RRenderSystem();

    RRenderSystem(const RRenderSystem&) = delete;
    RRenderSystem& operator=(const RRenderSystem&) = delete;

    void setShaderProg(const RShaderProg &shaders);
    void setShaderProg(const RShaderProg &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc);
    void setViewprot(float left, float right, float bottom, float top, float near = -127.0f, float far = 128.0f);
    void setPerspective(float left, float right, float bottom, float top, float near, float far);
    void setCameraMove(float x, float y, float z);
    void setCameraMove();

    void setProjectionMat(const glm::mat4& mat)
    {
        pMat_ = mat;
    }

    void setViewMat(const glm::mat4& mat)
    {
        vMat_ = mat;
    }

    const glm::mat4& getProjectionMat() const
    {
        return pMat_;
    }

    const glm::mat4& getViewMat() const
    {
        return vMat_;
    }

    const RShaderProg& getShaderProg() const
    {
        return shaders_;
    }

    RShaderProg& shaderProg()
    {
        return shaders_;
    }

    template<typename T>
    void render(T &plane)
    {
        glBindVertexArray(vao_[0]);

        RInterface inter = shaders_.useInterface();
        inter.setUniformMatrix(mLoc_, plane.model());
        plane.texture().bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
    }

    template<typename T>
    void renderLine(T &plane)
    {
        glBindVertexArray(vao_[1]);

        RInterface inter = shaders_.useInterface();
        inter.setUniformMatrix(mLoc_, plane.model());
        lineColor_.bind();
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glBindVertexArray(0);
    }

private:
    RTexture lineColor_;
    GLuint pLoc_, vLoc_, mLoc_;
    std::array<GLuint, 2> vao_, vbo_;
    RShaderProg shaders_;
    glm::mat4 pMat_, vMat_;
};

extern template void RRenderSystem::render<RPlane>(RPlane &n);
extern template void RRenderSystem::renderLine<RPlane>(RPlane &n);

} // ns Redopera

#endif // RENDERSYSTEM_H
