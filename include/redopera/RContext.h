#ifndef RCONTEXT_H
#define RCONTEXT_H

#include "ROpenGL.h"
#include <memory>

namespace Redopera {

class RContext
{
public:
    struct Format
    {
        bool vSync          = false;     // 垂直同步
        bool forward        = true;     // 前向兼容(清除当前版本不推荐的特性）
        bool debug          = false;    // OpenGL的Debug输出
        bool depth          = false;    // 深度测试
        int versionMajor    = 3;        // OpenGL主版本号
        int versionMinor    = 3;        // OpenGL副版本号
        GLFWwindow *shared  = nullptr;  // 共享上下文
    };

    RContext(const Format &fmt = format):
        contex_(nullptr, glfwDestroyWindow)
    {
        setContex(fmt);
    }

    ~RContext() = default;

    operator bool(){ return contex_ != nullptr; }

    GLFWwindow* getContex()
    {
        return contex_.get();
    }

    bool setContex(const Format &fmt)
    {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, fmt.versionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, fmt.versionMinor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, fmt.forward);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, fmt.debug);
        glfwWindowHint(GLFW_VISIBLE, false);
        GLFWwindow *window = glfwCreateWindow(1, 1, "", nullptr, fmt.shared);

        glfwMakeContextCurrent(window);
        if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            glfwDestroyWindow(window);
            return false;
        }

        contex_.reset(window);
        glfwSwapInterval(fmt.vSync ? 1 : 0);
        return true;
    }

    void setContex(GLFWwindow *contex)
    {
        contex_.reset(contex);
    }

    void release()
    {
        contex_.reset();
    }

private:
    static constexpr Format format = { true, true, false, false, 3, 3, nullptr };

    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> contex_;
};

} // ns Redopera

#endif // RCONTEXT_H
