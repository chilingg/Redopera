#ifndef RRENDERSYS_H
#define RRENDERSYS_H

#include "RShaders.h"
#include "RColor.h"
#include "RRect.h"
#include "RDict.h"
#include "rsc/RTexture.h"

namespace Redopera {

class RTexture;

class RRenderSys
{
public:
    static RShaders createSimpleShaders();
    static void createPlaneVAO(GLuint &vao, GLuint &vbo);

    static const RName project;
    static const RName view;
    static const RName model;
    static const RName hue;

    RRenderSys();
    RRenderSys(const RShaders &shaders);

    ~RRenderSys();

    // 自定义
    template<typename T>
    Redopera::RRenderSys& operator<<(T &obj)
    {
        render(obj.texture(), obj.model());
        return *this;
    }

    RRenderSys(const RRenderSys&) = delete;
    RRenderSys& operator=(const RRenderSys&) = delete;

    bool isUniform(RName name);
    GLint loc(RName name) const;
    const RDict<GLuint>& locList() const;
    const RShaders& shaders() const;
    GLuint vao() const;

    void bindVAO() const;
    void unbindVAO() const;

    void drawPlane() const;
    void drawLine() const;

    void registerUniform(std::initializer_list<RName> list);
    void registerUniform(const std::string &uniform, RName name);

    void setShaders(const RShaders &shaders);

    void setViewport(float left, float right, float bottom, float top, float near = -127.0f, float far = 128.0f);
    void setPerspective(float left, float right, float bottom, float top, float near, float far);
    void setProjectMat(const glm::mat4 &mat);

    void setViewMove();
    void setViewMove(float x, float y, float z);
    void setViewMat(const glm::mat4 &mat);

    void setHue(unsigned r, unsigned g, unsigned b, unsigned a = 0xff);
    void setHue(RColor color);
    void setHue(const glm::vec4 &color);
    void setHue(const glm::vec3 &color);

    void render(const RTexture &tex, const glm::mat4 &model) const;

    void renderLine(const glm::mat4 &mat);
    void renderLine(const RRect &rect);

private:
    GLuint vao_ = 0, vbo_ = 0;
    RShaders shaders_;
    RTexture white_;
    RDict<GLuint> uniform_;
};

} // ns Redopera

#endif // RRENDERSYS_H
