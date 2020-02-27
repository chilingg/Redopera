#ifndef RCONTEXT_H
#define RCONTEXT_H

#include "ROpenGL.h"
#include <memory>

namespace Redopera {

// 使用RWindow的场合，初始化由RWindow实例负责

class RContext
{
public:
    struct Format
    {
        bool vSync          = true;     // 垂直同步
        bool forward        = true;     // 前向兼容(清除当前版本不推荐的特性）
        bool debug          = false;    // OpenGL的Debug输出
        bool depth          = false;    // 深度测试
        int versionMajor    = 4;        // OpenGL主版本号
        int versionMinor    = 3;        // OpenGL副版本号
        GLFWwindow *shared  = nullptr;  // 共享上下文
    };

    RContext();
    ~RContext();

    operator bool(){ return init; }

    GLFWwindow* getContex();

    GLFWwindow* setContex();
    GLFWwindow* setContex(const Format &fmt);
    void setContex(GLFWwindow *contex);

    void release();

private:
    static void initGLFW();
    static bool init;

    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> contex_;
};

} // ns Redopera

#endif // RCONTEXT_H
