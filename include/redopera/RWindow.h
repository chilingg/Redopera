#ifndef RWINDOW_H
#define RWINDOW_H

#include "RContext.h"
#include "RSize.h"
#include "RColor.h"
#include "RPoint.h"
#include "RNode.h"
#include "RSigslot.h"
#include "RRenderSys.h"

namespace Redopera {

class RCursor;
class RImage;

class RWindow
{
public:
    enum class Viewport
    {
        Full,   // 充满窗口
        Scale,  // 保持比例
        Fix     // 固定尺寸
    };

    enum class CursorMode
    {
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        Disabled = GLFW_CURSOR_DISABLED
    };

    struct Format : RContext::Format
    {
        bool fix            = false;    // 固定窗口尺寸
        bool decorate       = true;     // 窗口边框与标题栏
        bool fullScreen     = false;    // 全屏
        bool maximization   = false;
        int defaultWidth    = 800;      // 默认窗口大小
        int defaultHeight   = 540;
        Viewport viewport   = Viewport::Full;   // 视口模式
        RGBA background      = 0x121212ff;      // 背景色
        CursorMode cMode    = CursorMode::Normal;
        double vRatio_      = 16.0/9.0;         // 视口比例 (Scale 模式)
    };

    static RWindow* focusWindow();
    static RWindow* getWindowUserCtrl(GLFWwindow *window);

    static const Format& defaultWindowFormat();
    static void setDefaultWindowFormat(Format fmt);

    explicit RWindow();
    explicit RWindow(int width, int height, const std::string &title = "Redopera", const Format &format = defaultFormat);
    ~RWindow() = default;

    RWindow(RWindow &) = delete;
    RWindow& operator=(RWindow &) = delete;

    void setWindowSize(int width, int height);
    void setWindowMinimumSize(int minW, int minH);
    void setWindowMaximumSize(int maxW, int maxH);
    void setWindowFixedSize(bool b = true);

    void setWindowTitle(const std::string &title);
    void setWindowDecrate(bool b = true);
    void setWindowFloatOnTop(bool b = true);
    void setWindowIcon(const RImage &img);
    void setMaximizaWindow();
    void setFullScreenWindow(bool b = true);
    void setVSync(bool enable = true);//垂直同步
    void setCursor(const RCursor *cursor);
    void setCursorModel(CursorMode mode);
    void setWindowFocus();
    void setClearMask();
    void restoreWindow();

    void setBackColor(unsigned r, unsigned g, unsigned b);
    void setBackColor(const RColor &color);
    void setBackColor(RGBA rgb);

    void setViewportSize(int width, int height);
    void setViewportRatio(double ratio);
    void setViewportPattern(Viewport pattern);

    void setDepthTest(bool b);
    void enableCapability(GLenum cap);
    void disableCapability(GLenum cap);

    RRenderSys* renderSys() const;
    GLFWwindow* getHandle() const;
    const Format& format() const;
    int width() const;
    int height() const;
    RSize size() const;
    RSize windowSize() const;
    int windowWidth() const;
    int windowHeight() const;
    CursorMode cursorMode() const;
    bool isFocus() const;
    bool isShouldCloused() const;
    bool isFullScreen() const;
    const RPoint2& posOffset() const;
    GLbitfield clearMask() const;

    void closeWindow();
    void show();
    void hide();

    RNode node;
    RSignal<bool&> closed;

private:
    // 窗口大小变更回调
    static void resizeCallback(GLFWwindow *window, int width, int height);
    // 鼠标滚轮回调
    static void mouseScrollCallback(GLFWwindow *window, double x, double y);
    // 窗口焦点回调
    static void windowFocusCallback(GLFWwindow *window, int focused);
    // 窗口关闭回调
    static void windowCloseCallback(GLFWwindow *window);
    // 键盘回调参数 = key：激发的键值，scancode：键值的系统扫描码，
    // action：GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT，modes：ALT，CTRL，SHIFT，META等
    static void keyboardCollback(GLFWwindow *window, int key, int scancode, int action, int mods);
    // 鼠标按键回调
    static void mouseButtonCollback(GLFWwindow *window, int btn, int action, int mods);
    // 光标移动
    static void cursorPosCollback(GLFWwindow *window, double x, double y);
    // 字符输入
    static void charInputCollback(GLFWwindow *window, unsigned code);

    int defaultExec();

    static Format defaultFormat;
    static std::atomic<RWindow*> focusWindowP;

    Format format_;
    RContext context_;
    RPoint2 vOffset_;
    RSize size_;

    GLbitfield clearMask_ = GL_COLOR_BUFFER_BIT;
    std::unique_ptr<RRenderSys> renderSys_;
};

} // Redopera

#endif
