#include <RContext.h>
#include <RDebug.h>
#include <stdexcept>

using namespace Redopera;

RContext::Format RContext::defaultFormat;
thread_local bool onethread = false;

RContext::RContext(const RContext::Format &fmt):
    context_(nullptr, glfwDestroyWindow)
{
    if(onethread)
        throw std::runtime_error("A thread can only have one context");
    onethread = true;

    setContext(fmt);
}

RContext::RContext(GLFWwindow *context):
    context_(context, glfwDestroyWindow)
{

}

RContext::~RContext()
{
    onethread = false;
}

Redopera::RContext::operator bool() const
{
    return context_ != nullptr;
}

GLFWwindow *RContext::getHandle() const
{
    return context_.get();
}

const RContext::Format &RContext::format() const
{
    return format_;
}

bool RContext::setContext(const RContext::Format &fmt)
{
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, fmt.debug);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, fmt.forward);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, fmt.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, fmt.versionMinor);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(1, 1, "", nullptr, fmt.shared);

    return setContext(window, fmt);
}

bool RContext::setContext(GLFWwindow *context, const RContext::Format &fmt)
{
    glfwMakeContextCurrent(context);
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(context);
        return false;
    }

    // 申请到的OpenGL版本
    format_.versionMajor = GLVersion.major;
    format_.versionMinor = GLVersion.minor;

    // Debug Context 需要OpenGL4.3以上版本
    if(format_.versionMajor * 10 + format_.versionMinor < 43)
        format_.debug = false;
    //若启用 OpenGL Debug
    if(format_.debug && GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        rDebug << EscCtl::green << EscCtl::bold << "OpenGL Context: "
               << reinterpret_cast<const char*>(glGetString(GL_VERSION))
               << "\nEnable OpenGL debug output" << EscCtl::non;
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openglDebugMessageCallback, nullptr);
        //过滤着色器编译成功消息通知
        glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_OTHER,
                              GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER,
                              GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    }

    setRasterizer(fmt.rasterizer);
    glfwSwapInterval(fmt.vSync ? 1 : 0);

    format_ = fmt;
    context_.reset(context);
    return context != nullptr;
}

void RContext::setRasterizer(bool b)
{
    b ? glDisable(GL_RASTERIZER_DISCARD) : glEnable(GL_RASTERIZER_DISCARD);
}

void RContext::openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei , const GLchar *message, const void *)
{
    std::string sourceStr;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        sourceStr = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceStr = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceStr = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceStr = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceStr = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER:
        sourceStr = "Other"; break;
    }

    std::string typeStr;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        typeStr = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeStr = "Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeStr = "Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeStr = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeStr = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER:
        typeStr = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeStr = "Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeStr = "Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:
        typeStr = "Other"; break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        fprintf(stderr, "(%d)%s-%s-high: %s\n", id, sourceStr.c_str(), typeStr.c_str(), message);
        throw std::runtime_error("OpenGL high error");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        rDebug << EscCtl::yellow << EscCtl::bold << '(' << id << ')' << sourceStr << typeStr << "-medium: "
               << message << EscCtl::non;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        rDebug << EscCtl::yellow << EscCtl::bold << '(' << id << ')' << sourceStr << typeStr << "-low "
               << message << EscCtl::non;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        rDebug << EscCtl::green << '(' << id << ')' << sourceStr << typeStr << "-notification "
               << message << EscCtl::non;
        break;
    }
}
