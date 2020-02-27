#include "RContext.h"
#include <atomic>
#include <mutex>

using namespace Redopera;

static std::atomic_int count(0);
static std::once_flag flag;
bool RContext::init = false;

RContext::RContext():
    contex_(nullptr, glfwDestroyWindow)
{
    std::call_once(flag, initGLFW);
    ++count;
}

RContext::~RContext()
{
    contex_.reset();

    if(--count == 0)
        glfwTerminate();
}

GLFWwindow *RContext::getContex()
{
    return contex_.get();
}

GLFWwindow *RContext::setContex()
{
    return setContex(Format{});
}

GLFWwindow *RContext::setContex(const RContext::Format &format)
{
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, format.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, format.versionMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, format.forward);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, format.debug);
    glfwWindowHint(GLFW_VISIBLE, false);
    contex_.reset(glfwCreateWindow(1, 1, "", nullptr, format.shared));
    if(!contex_)
        return nullptr;

    glfwMakeContextCurrent(contex_.get());
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        return nullptr;

    glfwSwapInterval(format.vSync ? 1 : 0);

    return contex_.get();
}

void RContext::setContex(GLFWwindow *contex)
{
    contex_.reset(contex);
}

void RContext::release()
{
    contex_.reset();
}

// GLFW错误回调
void glfwErrorCallback(int error, const char* description)
{
    printf("GLFW Error %d: %s\n", error, description);
}

void RContext::initGLFW()
{
    // glfw错误回调
    glfwSetErrorCallback(glfwErrorCallback);
    RContext::init = glfwInit();
}
