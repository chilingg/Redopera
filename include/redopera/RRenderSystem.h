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
    void setLineColor(const RColor& color);
    void setProjectionMat(const glm::mat4& mat);
    void setViewMat(const glm::mat4& mat);

    const glm::mat4& getProjectionMat() const;
    const glm::mat4& getViewMat() const;
    const RShaderProg& shaderProg();
    const RTexture& texture();
    GLuint projectionLocal();
    GLuint viewLocal();
    GLuint modelLocal();

    void bindVAO();
    void bindLineVAO();
    void unbindVAO();

    void drawRect();
    void drawLine();

    void renderLine(const glm::mat4 &mat);
    void renderLine(const RRect &rect);

private:
    RTexture lineColor_;
    GLuint pLoc_, vLoc_, mLoc_;
    std::array<GLuint, 2> vao_, vbo_;
    RShaderProg shaders_;
    glm::mat4 pMat_, vMat_;
};

} // ns Redopera

template<typename T>
Redopera::RRenderSystem& operator<<(Redopera::RRenderSystem &renderer, T obj)
{
    renderer.bindVAO();

    auto inter = renderer.shaderProg().useInterface();
    inter.setUniformMatrix(renderer.modelLocal(), obj.model());
    obj.texture().bind();
    renderer.drawRect();

    renderer.unbindVAO();

    return renderer;
}

template<typename T>
Redopera::RRenderSystem& operator<<(Redopera::RRenderSystem &renderer, T* obj)
{
    renderer.bindVAO();

    auto inter = renderer.shaderProg().useInterface();
    inter.setUniformMatrix(renderer.modelLocal(), obj->model());
    obj->texture().bind();
    renderer.drawRect();

    renderer.unbindVAO();

    return renderer;
}

#endif // RENDERSYSTEM_H
