#include <render/RContext.h>
#include <render/ROpenGL.h>
#include <RFormat.h>
#include <SDL2/SDL.h>

using namespace Redopera;

namespace  {

void openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei , const GLchar *message, const void *)
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
        rError("({}){}-{}-high: {}\n", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        rWarning("({}){}-{}-medium: {}\n", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        rWarning("({}){}-{}-low: {}\n", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        rMessage("({}){}-{}-notification: {}\n", id, sourceStr, typeStr, message);
        break;
    }
}

} // ns

int RContext::setAttribute(unsigned arrt, int value)
{
    return rCheck(SDL_GL_SetAttribute(static_cast<SDL_GLattr>(arrt), value),
                  "Error set contex arrtibute {} as {}: {}\n",
                  static_cast<unsigned>(arrt), value, SDL_GetError());
}

int RContext::getAttribute(unsigned arrt)
{
    int n = -1;
    rCheck(SDL_GL_GetAttribute(static_cast<SDL_GLattr>(arrt), &n), "Failed to get GL arrtibute {}: {}\n",
           static_cast<unsigned>(arrt), SDL_GetError());
    return n;
}

void RContext::resetAttribute()
{
    SDL_GL_ResetAttributes();
}

void RContext::setCapability(GLenum cap, bool enable)
{
    enable ? glEnable(cap) : glDisable(cap);
}

RContext::RContext(const RWindow &window)
{
    setContext(window);
}

RContext::RContext(const RWindow &window, const RContext::Format &fmt)
{
    setContext(window, fmt);
}

RContext::RContext(Context context):
    context_(context)
{

}

RContext::~RContext()
{
    if(context_)
        SDL_GL_DeleteContext(context_);
}

Redopera::RContext::operator bool() const
{
    return context_ != nullptr;
}

SDL_GLContext RContext::handle() const
{
    return context_;
}

bool RContext::setContext(const RWindow &window, const RContext::Format &fmt)
{
    SDL_GL_ResetAttributes();
    if(fmt.debug)
        setAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    if(fmt.forward)
        setAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    setAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, fmt.versionMajor);
    setAttribute(SDL_GL_CONTEXT_MINOR_VERSION, fmt.versionMinor);

    setAttribute(SDL_GL_STENCIL_SIZE, 8);
    setAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if(setContext(window))
    {
        if(fmt.vSync)
            setSwapInterval(1);
        setRasterizer(fmt.rasterizer);

        return true;
    }
    else
        return false;
}

bool RContext::setContext(const RWindow &window)
{
    std::unique_ptr<void, void(*)(void*)> context(SDL_GL_CreateContext(window.handle()), SDL_GL_DeleteContext);
    if(!context)
    {
        rError("OpenGL context could not be created! SDL Error: {}\n", SDL_GetError());
        return false;
    }
    // !gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress);
    else if(!gladLoadGL())
    {
        rError("Error initializing OpenGL from glad!\n");
        return false;
    }
    SDL_GL_MakeCurrent(window.handle(), context.get());

    // Debug Context 需要OpenGL4.3以上版本
    if(GLVersion.major * 10 + GLVersion.minor > 43 && GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        rMessage("OpenGL Context: {}\nEnable OpenGL debug output\n", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openglDebugMessageCallback, nullptr);
        //过滤着色器编译成功消息通知
        //glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_OTHER,
                              //GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        //glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER,
                              //GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    }
    else
        rWarning("Unable to set OpenGL Debug Context!\n");

    context_ = context.release();
    return true;
}

bool RContext::setSwapInterval(int interval)
{
    return rCheck(SDL_GL_SetSwapInterval(interval) < 0, "Unable to set VSync: {}\n", SDL_GetError());
}

void RContext::setRasterizer(bool b)
{
    b ? glDisable(GL_RASTERIZER_DISCARD) : glEnable(GL_RASTERIZER_DISCARD);
}
