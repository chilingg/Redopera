#ifndef RRENDERSYS_H
#define RRENDERSYS_H

#include "RShaders.h"
#include "RName.h"
#include "RColor.h"
#include "RRect.h"

#include <unordered_map>

namespace Redopera {

class RTexture;

class RRenderSys
{
    struct Subroutine
    {
        GLuint uniform;
        std::unordered_map<RName, GLuint> index;
    };

    struct SubroutineData
    {
        std::vector<GLuint> table;
        std::unordered_map<RName, Subroutine> data;
    };

    struct ShadersData
    {
        RShaders shaders;
        std::unordered_map<RName, GLuint> uniform;
        std::unordered_map<RShader::Type, SubroutineData> stage_;
    };

public:
    static RShaders createSimpleShaders();

    static const RName PROJECT;
    static const RName VIEW;
    static const RName MODEL;
    static const RName HUE;

    static const RName COLOR_OUT_FUNC;
    static const RName TEX_OUT;
    static const RName SINGLE_OUT;
    static const RName HUE_OUT;

    RRenderSys();
    RRenderSys(const RShaders &shaders);

    ~RRenderSys();

    // 自定义
    template<typename T>
    const Redopera::RRenderSys& operator<<(T &obj) const;

    RRenderSys(const RRenderSys&) = delete;
    RRenderSys& operator=(const RRenderSys&) = delete;

    bool isUniform(const RName &name);
    GLint loc(const RName &name) const;
    const std::unordered_map<RName, GLuint>& locList() const;
    const std::unordered_map<RShader::Type, SubroutineData>& stageData() const;
    const RShaders& shaders() const;
    GLuint vao() const;

    void bindVAO() const;
    void unbindVAO() const;

    void drawPlane() const;
    void drawLine() const;

    void registerUniform(std::initializer_list<RName> list);
    bool registerSubroutine(RShader::Type type, const RName &func, std::initializer_list<RName> list);

    void setShaders(const RShaders &shaders);
    void setSubroutine(RShader::Type type, const RName &func, const RName &opt);

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

    void usingTexColorOut();
    void usingSingleTexOut();
    void usingHueOut();

    void render(const RTexture &tex, const glm::mat4 &model) const;

    void renderLine(const glm::mat4 &mat);
    void renderLine(const RRect &rect);

private:
    void createVAO();

    GLuint vao_ = 0, vbo_ = 0;
    ShadersData data_;
};

} // ns Redopera

#endif // RRENDERSYS_H
