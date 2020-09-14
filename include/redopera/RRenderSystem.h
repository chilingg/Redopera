#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "rsc/RShaderProg.h"
#include "rsc/RTexture.h"

namespace Redopera {

class RRenderer;

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

    RRenderer renderer();

    void renderLine(const glm::mat4 &mat);
    void renderLine(const RRect &rect);

private:
    RTexture lineColor_;
    GLuint pLoc_, vLoc_, mLoc_;
    std::array<GLuint, 2> vao_, vbo_;
    RShaderProg shaders_;
    glm::mat4 pMat_, vMat_;
};

class RRenderer
{
public:
    RRenderer(RRenderSystem* rsystem):
        rsystem_(rsystem) { rsystem_->bindVAO(); };

    ~RRenderer() { rsystem_->unbindVAO(); };

    RRenderSystem* renderSystem() const { return rsystem_; };

private:
    RRenderSystem * const rsystem_;
};

} // ns Redopera

template<typename T>
const Redopera::RRenderer& operator<<(const Redopera::RRenderer &renderer, T obj)
{
    Redopera::RRenderSystem *rsystem = renderer.renderSystem();

    auto inter = rsystem->shaderProg().useInterface();
    inter.setUniformMatrix(rsystem->modelLocal(), obj.model());
    obj.texture().bind();
    rsystem->drawRect();

    return renderer;
}

template<typename T>
const Redopera::RRenderer& operator<<(const Redopera::RRenderer &renderer, T* obj)
{
    Redopera::RRenderSystem *rsystem = renderer.renderSystem();

    auto inter = rsystem->shaderProg().useInterface();
    inter.setUniformMatrix(rsystem->modelLocal(), obj->model());
    obj->texture().bind();
    rsystem->drawRect();

    return renderer;
}

#endif // RENDERSYSTEM_H
