#include "RContext.h"

using namespace Redopera;

thread_local std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> RContext::contex(nullptr, glfwDestroyWindow);

bool RContext::initialization()
{
    struct glfwHandle
    {
        glfwHandle() { init = glfwInit(); }
        ~glfwHandle() { glfwTerminate(); }
        bool init;
    };
    static glfwHandle glfw;

    return glfw.init;
}

GLFWwindow *RContext::setContexAsThisThread(const RContext::Format &format)
{
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, format.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, format.versionMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, format.forward);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, format.debug);
    glfwWindowHint(GLFW_VISIBLE, false);
    contex.reset(glfwCreateWindow(1, 1, "", nullptr, format.shared));
    if(!contex)
        return nullptr;

    glfwMakeContextCurrent(contex.get());
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        return nullptr;

    glfwSwapInterval(format.vSync ? 1 : 0);

    return contex.get();
}
