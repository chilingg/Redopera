#ifndef RPLANE_H
#define RPLANE_H

#include "RArea.h"
#include "ROpenGL.h"
#include "RMath.h"
#include "RColor.h"
#include "rsc/RTexture.h"
#include "rsc/RShaderProg.h"

namespace Redopera {

class RTexture;

class RPlane : public RArea
{
public:
    struct RenderTool
    {
        RShaderProg shaders;
        GLuint vao;
        GLuint edgingVAO;
        GLint modelLoc;
        GLint edgingLoc;
        GLuint vbo;
        GLuint edgingVBO;
    };

    struct ModelMat
    {
        glm::mat4 tran, rotate, scale;
    };

    static const std::shared_ptr<RenderTool> setShadersAsThread(const RShaderProg &shaders, GLint modelLoc, GLint edgingLoc = -1);

    RPlane();
    RPlane(int width, int height, int x, int y, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RPlane(int width, int height, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    RPlane(const RSize &size, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    explicit RPlane(const RRect &rect, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RPlane(const RPlane &plane);
    RPlane(const RPlane &&plane);
    RPlane& operator=(const RPlane &plane);
    RPlane& operator=(const RPlane &&plane);
    ~RPlane() = default;

    const RShaderProg& planeShader();
    const glm::mat4& modelMat() const;
    const ModelMat& modelMats() const;
    const RTexture& texture() const;

    void setColorTexture(const RColor &color);
    void setColorTexture(R_RGBA rgba);
    void setColorTexture(unsigned r, unsigned g, unsigned b, unsigned a = 0xffu);
    void setTexture(const RImage &img);
    void setTexture(const RTexture &tex);

    void update();
    void render();
    void render(const RShaderProg &shaders, GLuint mLoc);
    void edging(const RColor &color = RColor(0xff0000ff));
    void edging(const RShaderProg &shaders, GLuint mLoc);
    void edgingAll();
    void edgingAll(const RShaderProg &shaders, GLuint mLoc);

protected:
    static const std::shared_ptr<RenderTool> renderTool();

    void defaultRenderControl(const RShaderProg &shaders, GLuint mLoc);
    std::function<void(const RShaderProg &shaders, GLuint mLoc)>renderControl;

private:
    thread_local static std::weak_ptr<RenderTool> tRenderTool;

    struct Vertexs { GLuint ao1, ao2, bo1, bo2; };

    static Vertexs createVaertexObject();
    static RenderTool createRenderTool();

    std::shared_ptr<RenderTool> renderTool_;
    ModelMat mats_;
    glm::mat4 model_;
    RTexture texture_;
};

} // Redopera

#endif // RPLANE_H
