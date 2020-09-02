#ifndef RWINDOW_H
#define RWINDOW_H

#include "RContext.h"
#include "RSize.h"
#include "RPoint.h"
#include "RSigslot.h"
#include "RInputModule.h"
#include "RController.h"
#include "RTimer.h"

#include <atomic>

namespace Redopera {

class RCursor;
class RImage;
class RColor;

using RGB = uint32_t;

enum class Keys;
enum class BtnAct;

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
        int fps             = 55;       // 60帧有撕裂
        int defaultWidth    = 800;      // 初始窗口大小
        int defaultHeight   = 540;      // 初始窗口大小
        Viewport viewport   = Viewport::Full;  // 视口模式
        RGB background      = 0x121212; // 背景色
        CursorMode cMode    = CursorMode::Normal;
        double vRatio_      = 16.0/9.0; // 视口比例 (Scale 模式)
    };

    static RWindow* getWindowUserCtrl(GLFWwindow *window);

    explicit RWindow();
    explicit RWindow(int width, int height, const std::string title = "Redopera", const Format &format = windowFormat);
    ~RWindow() = default;

    RWindow(RWindow &) = delete;
    RWindow& operator=(RWindow &) = delete;

    void setAsMainWindow();

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
    void restoreWindow();

    void setBackColor(unsigned r, unsigned g, unsigned b);
    void setBackColor(const RColor &color);
    void setBackColor(RGB rgb);

    void setViewportSize(int width, int height);
    void setViewportRatio(double ratio);
    void setViewportPattern(Viewport pattern);

    void enableDepthTest();
    void disableDepthTest();

    GLFWwindow* getWindowHandle() const;
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
    RController* ctrl();
    const RController* ctrl() const;
    const RPoint2& posOffset() const;
    const RInputModule* inputModule() const;

    void closeWindow();
    // 调用showWindow()之后才会连接回调
    void show();
    void hide();

    RSignal<JoystickID, JoystickPresent> joyPresented;

private:
    // OpenGL Debug信息
    static void openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                           GLsizei length, const GLchar *message, const void *userParam);
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

    int defaultExec();
    int mainExecFunc();

    static const Format windowFormat;
    static GLFWwindow *mainWindow;

    RController ctrl_;
    RInputModule input_;
    Format format_;
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> window_;
    RPoint2 vOffset_;
    RSize size_;
    RTimer fTimer_;
    std::atomic_bool focused_;

    GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
};

} // Redopera

#endif
