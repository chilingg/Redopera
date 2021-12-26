#ifndef RCONTEXT_H
#define RCONTEXT_H

#include "../RWindow.h"
#include "ROpenGL.h"

namespace Redopera {

using Context = void*;

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
        bool rasterizer     = true;     // 光栅化
        int versionMajor    = 4;        // OpenGL主版本号
        int versionMinor    = 3;        // OpenGL副版本号
    };

    // SDL_GLattr
    static int setAttribute(unsigned arrt, int value);
    static int getAttribute(unsigned arrt);
    static void resetAttribute();
    static void setCapability(GLenum cap, bool enable);

    RContext(const RWindow &window);
    RContext(const RWindow &window, const Format &fmt);
    RContext(Context context);

    RContext(RContext &) = delete;
    RContext& operator=(RContext &) = delete;

    ~RContext();

    explicit operator bool() const;

    Context handle() const;

    bool setContext(const RWindow &window, const Format &fmt);
    bool setContext(const RWindow &window);

    bool setSwapInterval(int interval);
    void setRasterizer(bool b);

    void free();

private:
    Context context_ = nullptr;
};

} // ns Redopera

#endif // RCONTEXT_H
