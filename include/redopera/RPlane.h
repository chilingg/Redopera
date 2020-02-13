#ifndef RPLANE_H
#define RPLANE_H

#include "RArea.h"
#include "rsc/RTexture.h"
#include "rsc/RShaderProg.h"

namespace Redopera {

class RPlane : public RArea
{
public:
    struct RenderTool
    {
        RShaderProg &shaders;
        GLuint &vao;
        GLuint &modelLoc;
        GLuint &edgingVAO;
        GLuint &edgingLoc;
    };

    struct ModelMat
    {
        glm::mat4 tran, rotate, scale;
    };

    static const RenderTool& planeRenderTool();
    static void setPlaneShadersAsThread(const RShaderProg &shaders, GLuint modelLoc, GLuint edgingLoc = -1);
    static const RShaderProg& planeShader();

    RPlane();
    RPlane(int width, int height, int x, int y, int z = 0, const RTexture &tex = RTexture::whiteTex(), const std::string &name = "Plane");
    RPlane(int width, int height, const RPoint &pos, const RTexture &tex = RTexture::whiteTex(), const std::string &name = "Plane");
    RPlane(const RSize &size, const RPoint &pos, const RTexture &tex = RTexture::whiteTex(), const std::string &name = "Plane");
    explicit RPlane(const RRect &rect, int z = 0, const RTexture &tex = RTexture::whiteTex(), const std::string &name = "Plane");
    explicit RPlane(const Format &area, const RTexture &tex = RTexture::whiteTex(), const std::string &name = "Plane");
    RPlane(const RPlane &plane);
    RPlane(const RPlane &&plane);
    RPlane& operator=(const RPlane &plane);
    RPlane& operator=(const RPlane &&plane);
    ~RPlane() override = default;

    const glm::mat4& modelMat() const;
    const ModelMat& modelMats() const;
    const RTexture& texture() const;
    const std::string& name() const;

    void setColorTexture(const RColor &color);
    void setColorTexture(R_RGBA rgba);
    void setColorTexture(unsigned r, unsigned g, unsigned b, unsigned a = 0xffu);
    void setTexture(const RImage &img);
    void setTexture(const RTexture &tex);
    void rename(std::string name);

    void update();
    void render();
    void render(const RShaderProg &shaders, GLuint mLoc);
    void edging(const RColor &color = RColor(0xff0000ff));
    void edging(const RShaderProg &shaders, GLuint mLoc);
    void edgingAll();
    void edgingAll(const RShaderProg &shaders, GLuint mLoc);

protected:
    virtual void renderControl(const RShaderProg &shaders, GLuint mLoc);

private:
    thread_local static RShaderProg tPlaneShaders;
    thread_local static GLuint MODEL_LOC;
    thread_local static GLuint EDGING_LOC;

    std::string name_;
    ModelMat mats_;
    glm::mat4 model_;
    RTexture texture_;
};

} // Redopera

#endif // RPLANE_H
