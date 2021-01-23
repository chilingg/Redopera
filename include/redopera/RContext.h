#ifndef RCONTEXT_H
#define RCONTEXT_H

#include "ROpenGL.h"
#include <memory>

namespace Redopera {

extern thread_local bool threadOnlyContext;

class RContext
{
public:
    struct Format
    {
#ifndef NDEBUG
        bool debug          = true;     // OpenGL的Debug输出
#else
        bool debug          = false;
#endif
        bool vSync          = true;     // 垂直同步
        bool forward        = true;     // 前向兼容(清除当前版本不推荐的特性）
        bool rasterizer     = false;    // 光栅化
        int versionMajor    = 3;        // OpenGL主版本号
        int versionMinor    = 3;        // OpenGL副版本号
        GLFWwindow *shared  = nullptr;  // 共享上下文
    };

    static const Format& defaultContexFormat() { return defaultFormat; }
    static void setDefualtContexFormat(const Format &format) { defaultFormat = format; }

    RContext(const Format &fmt = defaultFormat);
    RContext(GLFWwindow *context);
    ~RContext();

    operator bool();

    GLFWwindow* getHandle() const;

    const Format& format() const;

    bool setContext(const Format &fmt);
    bool setContext(GLFWwindow *context, const Format &fmt);

    void setRasterizer(bool b);

    void release();

private:
    // OpenGL Debug信息
    static void openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                           GLsizei length, const GLchar *message, const void *userParam);

    static Format defaultFormat;

    Format format_;
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> context_;
};

} // ns Redopera

#endif // RCONTEXT_H
