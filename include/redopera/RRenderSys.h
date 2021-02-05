#ifndef RRENDERSYS_H
#define RRENDERSYS_H

#include "RShaders.h"
#include "RKeeper.h"
#include "RRect.h"
#include "rsc/RTexture.h"

#include <string>
#include <map>

namespace Redopera {

class RRenderSys
{
    struct RenderTools
    {
        GLuint pLoc_, vLoc_, mLoc_;
        RShaders shaders_;
    };

public:
    static RShaders createSimpleShaders();
    static RShaders createSimpleSingleShaders();

    RRenderSys();
    RRenderSys(const std::string &name, const RShaders &shaders);
    RRenderSys(const std::string &name, const RShaders &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc);

    ~RRenderSys();

    template<typename T>
    const Redopera::RRenderSys& operator<<(T &obj) const
    {
        bindVAO();
        auto inter = shaders()->use();
        inter.setUniformMat(modelLocal(), obj.model());
        obj.texture().bind();
        drawPlane();

        return *this;
    }

    RRenderSys(const RRenderSys&) = delete;
    RRenderSys& operator=(const RRenderSys&) = delete;

    GLuint projectLocal() const;
    GLuint viewLocal() const;
    GLuint modelLocal() const;

    const RShaders* shaders() const;
    const RShaders* queryShaders(const std::string &name) const;

    void setCurrentShaders(const std::string &name);

    std::string addShaders(const std::string &name, const RShaders &shaders);
    std::string addShaders(const std::string &name, const RShaders &shaders, GLuint pLoc, GLuint vLoc, GLuint mLoc);

    void removeShaders(const std::string &name);

    void setViewprot(float left, float right, float bottom, float top, float near = -127.0f, float far = 128.0f);
    void setPerspective(float left, float right, float bottom, float top, float near, float far);
    void setProjectionMat(const glm::mat4& mat);

    void setCamera(float x, float y, float z);
    void setCamera();
    void setViewMat(const glm::mat4& mat);

    void bindVAO() const;
    void unbindVAO() const;

    void drawPlane() const;

    void render(const RTexture &tex, const glm::mat4 &model);

    void renderLine(const glm::mat4 &mat);
    void renderLine(const RRect &rect);

private:
    void initialize();
    std::string availableName(const std::string &name) const;

    GLuint vao_, vbo_;
    std::string mainShaders_;
    RTexture lineColor_;
    std::map<std::string, RenderTools> renderers_;
};

} // ns Redopera

#endif // RRENDERSYS_H
