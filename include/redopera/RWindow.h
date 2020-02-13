#ifndef RWINDOW_H
#define RWINDOW_H

#include "RController.h"
#include "RContext.h"
#include "rsc/RCursor.h"

#include <atomic>

namespace Redopera {

class RWindow : public RController
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
        bool keysSigal      = false;    // 键盘响应信号(entered)
        Viewport viewport   = Viewport::Full;  // 视口模式
        R_RGBA background   = 0x121212; // 背景色
        int initWidth       = 800;      // 初始窗口大小
        int initHeight      = 540;      // 初始窗口大小
        double vRatio_      = 16.0/9.0; // 视口比例 (Scale 模式)
        CursorMode cMode    = CursorMode::Normal;
    };

    constexpr static const char* defaultName() { return "Window"; }

    static RWindow* getMainWindow();

    static void setDefaultWindowFormat(const Format &format);
    static bool updateGamepadMappings(const std::string &path);

    explicit RWindow(RController *parent = nullptr, const std::string &name = defaultName());
    explicit RWindow(const Format &format, RController *parent = nullptr, const std::string &name = defaultName());
    ~RWindow() override = default;

    void translation(const TranslationInfo &info) override;

    void setWindowSize(int width, int height);
    void setWindowMinimumSize(int minW, int minH);
    void setWindowMaximumSize(int maxW, int maxH);
    void setWindowFixedSize(bool b = true);

    void setWindowTitle(const std::string &title);
    void setWindowDecrate(bool b = true);
    void setWindowFloatOnTop(bool b = true);
    void setWindowIcon(const RImage &img);
    void setFullScreenWindow(bool b = true);
    void setVSync(bool enable = true);//垂直同步
    void setCursor(const RCursor *cursor);
    void setCursorModel(CursorMode mode);

    void setBackColor(unsigned r, unsigned g, unsigned b);
    void setBackColor(const RColor &color);
    void setBackColor(R_RGBA rgba);

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
    int windowWidth() const;
    int windowHeight() const;
    CursorMode cursorMode() const;
    bool isFocus() const;
    bool isShouldCloused() const;

    void closeWindow();
    // 调用showWindow()之后才会连接回调
    void show();
    void hide();
    int exec();

    RSignal<Keys, ButtonAction, Modifier> entered;
    RSignal<int> rolled;

protected:
    Status loopingCheck();

private:
    static void initMainWindow(RWindow *window);
    // GLFW错误回调
    static void glfwErrorCallback(int error, const char* description);
    // OpenGL Debug信息
    static void openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                           GLsizei length, const GLchar *message, const void *userParam);
    // 手柄连接回调
    static void joystickPresentCallback(int jid, int event);
    // 窗口大小变更回调
    static void resizeCallback(GLFWwindow *window, int width, int height);
    // 键盘回调参数 = key：激发的键值，scancode：键值的系统扫描码，
    // action：GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT，modes：ALT，CTRL，SHIFT，META等
    static void keyboardCollback(GLFWwindow *window, int key, int scancode, int action, int mods);
    // 鼠标滚轮回调
    static void mouseScrollCallback(GLFWwindow *window, double x, double y);
    // 窗口焦点回调
    static void windowFocusCallback(GLFWwindow *window, int focused);
    // 窗口关闭回调
    static void windowCloseCallback(GLFWwindow *window);

    static RWindow* getWindowUserCtrl(GLFWwindow *window);

    static Format windowFormat;
    static std::once_flag init;
    static RWindow* mainWindow;

    Format format_;
    std::function<void()> eventPool; //主线程中为glfwPoolEvent，其余线程中为空
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> window_;
    RPoint2 vOffset_;
    int width_;
    int height_;
    std::atomic_bool focused_;

    GLbitfield clearMask = GL_COLOR_BUFFER_BIT;

    _RSLOT_TAIL_
};

} // Redopera

#endif
