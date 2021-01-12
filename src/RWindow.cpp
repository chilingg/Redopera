#include <RWindow.h>
#include <REvent.h>
#include <RDebug.h>
#include <RColor.h>
#include <rsc/RCursor.h>
#include <rsc/RImage.h>

using namespace Redopera;

const RWindow::Format RWindow::windowFormat;
RWindow *RWindow::mainWindowP = nullptr;

RWindow *RWindow::mainWindow()
{
    return mainWindowP;
}

RWindow *RWindow::getWindowUserCtrl(GLFWwindow *window)
{
    return static_cast<RWindow*>(glfwGetWindowUserPointer(window));
}

RWindow::RWindow():
    RWindow(800, 540, "Redopera", windowFormat)
{

}

RWindow::RWindow(int width, int height, const std::string title, const RWindow::Format &format):
    poolFunc([]{}),
    ctrl_("Window", this),
    input_(this),
    format_(format),
    window_(nullptr, glfwDestroyWindow),
    vOffset_(0),
    size_(width, height), // 真正的尺寸在循环时决定
    focused_(false)
{
    // Debug Context 需要OpenGL4.3以上版本
    if(format_.versionMajor * 10 + format_.versionMinor < 43)
        format_.debug = false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, format_.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, format_.versionMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, format_.forward);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, format_.debug);
    glfwWindowHint(GLFW_RESIZABLE, format_.fix ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, format_.decorate ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, format_.maximization ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    // 默认初始窗口不可见，需主动调用show()
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWmonitor *monitor = nullptr;
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    if(format_.fullScreen)
        window_.reset(glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, format_.shared));
    else
        window_.reset(glfwCreateWindow(width, height, title.c_str(), nullptr, format_.shared));

    if(!window_)
        throw std::runtime_error("Fainled to create GLFW window");

    if(glfwGetCurrentContext())
        throw std::runtime_error("A thread can only have one window context");
    glfwMakeContextCurrent(window_.get());

    //绑定上下文与this指针
    glfwSetWindowUserPointer(window_.get(), this);

    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Failed to initialize GLAD");

    // 申请到的OpenGL版本
    format_.versionMajor = GLVersion.major;
    format_.versionMinor = GLVersion.minor;

    //若启用 OpenGL Debug
    if(format_.debug && GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        rDebug << EscCtl::green << EscCtl::bold << "Window " << title << ": "
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

    setVSync(format_.vSync);
    //设置混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //默认背景色
    RColor color(format_.background);
    glClearColor(color.r()/255.0f, color.g()/255.0f, color.b()/255.0f, 1.0f);
    //禁用字节对齐限制
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(format_.depth)
        enableDepthTest();

    RImage img = RImage::redoperaIcon();
    GLFWimage icon{ img.width(), img.height(), img.data() };
    glfwSetWindowIcon(window_.get(), 1, &icon);

    ctrl_.setCloseFunc([this](CloseEvent *e){ if(e->stop) glfwSetWindowShouldClose(window_.get(), GLFW_FALSE); });
    ctrl_.setExecFunc(std::bind(&RWindow::defaultExec, this));
}

void RWindow::setAsMainWindow()
{
    mainWindowP = this;
    poolFunc = glfwPollEvents;
}

void RWindow::setWindowSize(int width, int height)
{
    glfwSetWindowSize(window_.get(), width, height);
}

void RWindow::setWindowMinimumSize(int minW, int minH)
{
    glfwSetWindowSizeLimits(window_.get(), minW, minH, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void RWindow::setWindowMaximumSize(int maxW, int maxH)
{
    glfwSetWindowSizeLimits(window_.get(), GLFW_DONT_CARE, GLFW_DONT_CARE, maxW, maxH);
}

void RWindow::setWindowFixedSize(bool b)
{
    glfwSetWindowAttrib(window_.get(), GLFW_RESIZABLE, b ? GLFW_FALSE : GLFW_TRUE);
}

void RWindow::setWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(window_.get(), title.c_str());
}

void RWindow::setWindowDecrate(bool b)
{
    glfwSetWindowAttrib(window_.get(), GLFW_DECORATED, b ? GLFW_TRUE: GLFW_FALSE);
}

void RWindow::setWindowFloatOnTop(bool b)
{
    glfwSetWindowAttrib(window_.get(), GLFW_FLOATING, b ? GLFW_TRUE: GLFW_FALSE);
}

void RWindow::setWindowIcon(const RImage &img)
{
    GLFWimage icon{ img.width(), img.height(), img.data() };
    glfwSetWindowIcon(window_.get(), 1, &icon);
}

void RWindow::setMaximizaWindow()
{
    glfwMaximizeWindow(window_.get());
}

void RWindow::setFullScreenWindow(bool b)
{
    format_.fullScreen = b;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

    if(b)
    {
        glfwSetWindowMonitor(window_.get(), monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        // 全屏时GLFW似乎会取消垂直同步
        setVSync(format_.vSync);
        // Windows下需手动调用resize回调
        resizeCallback(window_.get(), vidmode->width, vidmode->height);
    }
    else {
        glfwSetWindowMonitor(window_.get(), nullptr, (vidmode->width - format_.defaultWidth)/2,
                             (vidmode->height - format_.defaultHeight)/2, format_.defaultWidth,
                             format_.defaultHeight, vidmode->refreshRate);
    }

}

void RWindow::setVSync(bool enable)
{
    format_.vSync = enable; // 参数为1锁60帧
    glfwSwapInterval(enable ? 1 : 0);
}

void RWindow::setCursor(const RCursor *cursor)
{
    glfwSetCursor(window_.get(), cursor ? cursor->data() : nullptr);
}

void RWindow::setCursorModel(RWindow::CursorMode mode)
{
    format_.cMode = mode;
    glfwSetInputMode(window_.get(), GLFW_CURSOR, static_cast<int>(mode));
}

void RWindow::setWindowFocus()
{
    glfwFocusWindow(window_.get());
}

void RWindow::restoreWindow()
{
    glfwRestoreWindow(window_.get());
}

void RWindow::setBackColor(unsigned r, unsigned g, unsigned b)
{
    setBackColor(RColor(r, g, b, 255));
}

void RWindow::setBackColor(const RColor &color)
{
    format_.background = color.rgba();
    glClearColor(color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, 1.0f);
}

void RWindow::setBackColor(RGBA rgba)
{
    setBackColor(RColor(rgba));
}

void RWindow::setViewportSize(int width, int height)
{
    size_.set(width, height);
    resizeCallback(window_.get(), windowWidth(), windowHeight());
}

void RWindow::setViewportRatio(double ratio)
{
    format_.vRatio_ = ratio;
    resizeCallback(window_.get(), windowWidth(), windowHeight());
}

void RWindow::setViewportPattern(RWindow::Viewport pattern)
{
    format_.viewport = pattern;
    resizeCallback(window_.get(), windowWidth(), windowHeight());
}

void RWindow::enableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
    clearMask |= static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
}

void RWindow::disableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
    clearMask |= !static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
}

GLFWwindow *RWindow::getWindowHandle() const
{
    return window_.get();
}

const RWindow::Format &RWindow::format() const
{
    return format_;
}

int RWindow::width() const
{
    return size_.width();
}

int RWindow::height() const
{
    return size_.height();
}

RSize RWindow::size() const
{
    return size_;
}

RSize RWindow::windowSize() const
{
    int w, h;
    glfwGetWindowSize(window_.get(), &w, &h);
    return RSize(w, h);
}

int RWindow::windowWidth() const
{
    int w, h;
    glfwGetWindowSize(window_.get(), &w, &h);
    return w;
}

int RWindow::windowHeight() const
{
    int w, h;
    glfwGetWindowSize(window_.get(), &w, &h);
    return h;
}

RWindow::CursorMode RWindow::cursorMode() const
{
    return format_.cMode;
}

bool RWindow::isFocus() const
{
    return focused_;
}

bool RWindow::isShouldCloused() const
{
    return glfwWindowShouldClose(window_.get()) == GLFW_TRUE;
}

bool RWindow::isFullScreen() const
{
    return format_.fullScreen;
}

RController *RWindow::ctrl()
{
    return &ctrl_;
}

const RController *RWindow::ctrl() const
{
    return &ctrl_;
}

const RPoint2 &RWindow::posOffset() const
{
    return vOffset_;
}

const RInputModule *RWindow::input() const
{
    return &input_;
}

void RWindow::closeWindow()
{
    glfwSetWindowShouldClose(window_.get(), GLFW_TRUE);
}

void RWindow::show()
{
    glfwShowWindow(window_.get());
}

void RWindow::hide()
{
    glfwHideWindow(window_.get());
}

void RWindow::openglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei , const GLchar *message, const void *)
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
        std::runtime_error("OpenGL high error");
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

void RWindow::resizeCallback(GLFWwindow *window, int width, int height)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    RSize resize(width, height);

    switch(wctrl->format_.viewport)
    {
    case Viewport::Scale:
    {
        double ratio = static_cast<double>(resize.width()) / resize.height();
        int n;
        if(ratio > wctrl->format_.vRatio_)
        {
            n = static_cast<int>(resize.height() * wctrl->format_.vRatio_);
            glViewport((resize.width() - n) / 2, 0, n, resize.height());
            wctrl->vOffset_.set((resize.width() - n) / 2, 0);
            wctrl->size_.setWidth(n);
            wctrl->size_.setHeight(resize.height());
        }
        else
        {
            n = static_cast<int>(resize.width() / wctrl->format_.vRatio_);
            glViewport(0, (resize.height() - n) / 2, resize.width(), n);
            wctrl->vOffset_.set(0, (resize.height() - n) / 2);
            wctrl->size_.setWidth(resize.width());
            wctrl->size_.setHeight(n);
        }
        break;
    }
    case Viewport::Full:
    {
        glViewport(0, 0, resize.width(), resize.height());
        wctrl->vOffset_.set(0, 0);
        wctrl->size_.set(resize.width(), resize.height());
        break;
    }
    case Viewport::Fix:
    {
        glViewport((resize.width() - wctrl->size_.width()) / 2.0,
                   (resize.height() - wctrl->size_.height()) / 2.0,
                   wctrl->size_.width(),
                   wctrl->size_.height());
        wctrl->vOffset_.set((resize.width() - wctrl->size_.width()) / 2.0,
                            (resize.height() - wctrl->size_.height()) / 2.0);
        break;
    }
    }

    // 传递Translation info
    TransEvent info{ &wctrl->ctrl_, { wctrl->size_.width(), wctrl->size_.height() } };
    wctrl->ctrl_.translation(&info);
}

void RWindow::mouseScrollCallback(GLFWwindow *window, double , double y)
{
    getWindowUserCtrl(window)->input_.mouseWheel(static_cast<int>(y));
}

void RWindow::windowFocusCallback(GLFWwindow *window, int focused)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    wctrl->focused_ = focused ? true : false;
}

void RWindow::windowCloseCallback(GLFWwindow *window)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    wctrl->ctrl_.breakLoop();
}

void RWindow::keyboardCollback(GLFWwindow *window, int key, int, int action, int)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    if (action == GLFW_RELEASE)
        wctrl->input_.keyUp(RInputModule::toKey(key));
    else if (action == GLFW_PRESS)
        wctrl->input_.keyDown(RInputModule::toKey(key));
}

void RWindow::mouseButtonCollback(GLFWwindow *window, int btn, int action, int)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    if (action == GLFW_RELEASE)
        wctrl->input_.mouseUp(RInputModule::toMouseButtons(btn));
    else if (action == GLFW_PRESS)
        wctrl->input_.mouseDown(RInputModule::toMouseButtons(btn));
}

int RWindow::defaultExec()
{
    // 不在构造函数时设置回调，防止多线程中在未构造完成时被调用
    glfwSetWindowFocusCallback(window_.get(), windowFocusCallback);
    glfwSetFramebufferSizeCallback(window_.get(), resizeCallback);
    glfwSetScrollCallback(window_.get(), mouseScrollCallback);
    glfwSetWindowCloseCallback(window_.get(), windowCloseCallback);
    glfwSetKeyCallback(window_.get(), keyboardCollback);
    glfwSetMouseButtonCallback(window_.get(), mouseButtonCollback);

    StartEvent sEvent(&ctrl_);
    ctrl_.dispatchEvent(&sEvent);

    RSize size = windowSize();
    resizeCallback(window_.get(), size.width(), size.height());

    while(ctrl_.loopingCheck() == RController::Status::Looping)
    {
        // 清屏 清除颜色缓冲和深度缓冲
        glClear(clearMask);

        poolFunc();

        // 发起处理
        ProcessEvent instruct(&ctrl_, &input_);
        ctrl_.process(&instruct);

        // 清空输入
        input_.updataInputCache();

        ctrl_.updataAll();
        glfwSwapBuffers(window_.get());

        if(glfwWindowShouldClose(window_.get()))
            ctrl_.breakLoop();
    }

    FinishEvent fEvent(&ctrl_);
    ctrl_.dispatchEvent(&fEvent);
    ctrl_.closed.emit();

    if(check(ctrl_.status() == RController::Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;

}
