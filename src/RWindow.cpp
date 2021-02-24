#include <RWindow.h>
#include <RDebug.h>
#include <RRect.h>
#include <RInput.h>
#include <rsc/RImage.h>
#include <rsc/RCursor.h>

using namespace Redopera;

RWindow::Format RWindow::defaultFormat;
std::atomic<RWindow*> RWindow::focusWindowP;

RWindow *RWindow::focusWindow()
{
    return focusWindowP;
}

RWindow *RWindow::getWindowUserCtrl(GLFWwindow *window)
{
    return static_cast<RWindow*>(glfwGetWindowUserPointer(window));
}

const RWindow::Format &RWindow::defaultWindowFormat()
{
    return defaultFormat;
}

void RWindow::setDefaultWindowFormat(RWindow::Format fmt)
{
    defaultFormat = fmt;
}

RWindow::RWindow():
    RWindow(800, 540, "Redopera")
{

}

RWindow::RWindow(int width, int height, const std::string &title, const RWindow::Format &format):
    node("Window", this),
    format_(format),
    context_(nullptr),
    vOffset_(0, 0),
    size_(width, height)
{
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, format_.debug);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, format_.forward);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, format_.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, format_.versionMinor);
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

    GLFWwindow *window;
    if(format_.fullScreen)
        window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, format_.shared);
    else
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, format_.shared);

    if(!window)
        throw std::runtime_error("Fainled to create GLFW window");

    format_.rasterizer = true;
    if(!context_.setContext(window, format_))
        throw std::runtime_error("Failed to initialize GLAD");

    // 绑定上下文与this指针
    glfwSetWindowUserPointer(context_.getHandle(), this);

    // 设置混合（未开启）
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 默认背景色
    RColor color(format_.background);
    glClearColor(color.r()/255.0f, color.g()/255.0f, color.b()/255.0f, 1.0f);
    // 禁用字节对齐限制（字体图片1位缓存）
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    RImage img = RImage::redoperaIcon();
    GLFWimage icon{ img.width(), img.height(), img.data() };
    glfwSetWindowIcon(context_.getHandle(), 1, &icon);

    node.setExecFunc([this]{ return defaultExec(); });

    if(format_.versionMajor > 3)
        renderSys_ = std::make_unique<RRenderSys>(RRenderSys::createSimpleShaders());

    if(!focusWindowP)
        focusWindowP = this;

    RInput::enableGamepad();
}

void RWindow::setWindowSize(int width, int height)
{
    glfwSetWindowSize(context_.getHandle(), width, height);
}

void RWindow::setWindowMinimumSize(int minW, int minH)
{
    glfwSetWindowSizeLimits(context_.getHandle(), minW, minH, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void RWindow::setWindowMaximumSize(int maxW, int maxH)
{
    glfwSetWindowSizeLimits(context_.getHandle(), GLFW_DONT_CARE, GLFW_DONT_CARE, maxW, maxH);
}

void RWindow::setWindowFixedSize(bool b)
{
    glfwSetWindowAttrib(context_.getHandle(), GLFW_RESIZABLE, b ? GLFW_FALSE : GLFW_TRUE);
}

void RWindow::setWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(context_.getHandle(), title.c_str());
}

void RWindow::setWindowDecrate(bool b)
{
    glfwSetWindowAttrib(context_.getHandle(), GLFW_DECORATED, b ? GLFW_TRUE: GLFW_FALSE);
}

void RWindow::setWindowFloatOnTop(bool b)
{
    glfwSetWindowAttrib(context_.getHandle(), GLFW_FLOATING, b ? GLFW_TRUE: GLFW_FALSE);
}

void RWindow::setWindowIcon(const RImage &img)
{
    GLFWimage icon{ img.width(), img.height(), img.data() };
    glfwSetWindowIcon(context_.getHandle(), 1, &icon);
}

void RWindow::setMaximizaWindow()
{
    glfwMaximizeWindow(context_.getHandle());
}

void RWindow::setFullScreenWindow(bool b)
{
    format_.fullScreen = b;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

    if(b)
    {
        glfwSetWindowMonitor(context_.getHandle(), monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        // 全屏时GLFW似乎会取消垂直同步
        setVSync(format_.vSync);
        // Windows下需手动调用resize回调
        resizeCallback(context_.getHandle(), vidmode->width, vidmode->height);
    }
    else {
        glfwSetWindowMonitor(context_.getHandle(), nullptr, (vidmode->width - format_.defaultWidth)/2,
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
    glfwSetCursor(context_.getHandle(), cursor ? cursor->data() : nullptr);
}

void RWindow::setCursorModel(RWindow::CursorMode mode)
{
    format_.cMode = mode;
    glfwSetInputMode(context_.getHandle(), GLFW_CURSOR, static_cast<int>(mode));
}

void RWindow::setWindowFocus()
{
    glfwFocusWindow(context_.getHandle());
}

void RWindow::restoreWindow()
{
    glfwRestoreWindow(context_.getHandle());
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
    size_.setSize(width, height);
    resizeCallback(context_.getHandle(), windowWidth(), windowHeight());
}

void RWindow::setViewportRatio(double ratio)
{
    format_.vRatio_ = ratio;
    resizeCallback(context_.getHandle(), windowWidth(), windowHeight());
}

void RWindow::setViewportPattern(RWindow::Viewport pattern)
{
    format_.viewport = pattern;
    resizeCallback(context_.getHandle(), windowWidth(), windowHeight());
}

void RWindow::enableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
    clearMask_ |= static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
}

void RWindow::disableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
    clearMask_ |= !static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
}

void RWindow::enableCapability(GLenum cap)
{
    glEnable(cap);
}

void RWindow::disableCapability(GLenum cap)
{
    glDisable(cap);
}

RRenderSys *RWindow::renderSys() const
{
    return renderSys_.get();
}

GLFWwindow *RWindow::getHandle() const
{
    return context_.getHandle();
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
    glfwGetWindowSize(context_.getHandle(), &w, &h);
    return RSize(w, h);
}

int RWindow::windowWidth() const
{
    int w, h;
    glfwGetWindowSize(context_.getHandle(), &w, &h);
    return w;
}

int RWindow::windowHeight() const
{
    int w, h;
    glfwGetWindowSize(context_.getHandle(), &w, &h);
    return h;
}

RWindow::CursorMode RWindow::cursorMode() const
{
    return format_.cMode;
}

bool RWindow::isFocus() const
{
    return focusWindowP == this;
}

bool RWindow::isShouldCloused() const
{
    return glfwWindowShouldClose(context_.getHandle()) == GLFW_TRUE;
}

bool RWindow::isFullScreen() const
{
    return format_.fullScreen;
}

const RPoint2 &RWindow::posOffset() const
{
    return vOffset_;
}

GLbitfield RWindow::clearMask() const
{
    return clearMask_;
}

void RWindow::closeWindow()
{
    glfwSetWindowShouldClose(context_.getHandle(), GLFW_TRUE);
}

void RWindow::show()
{
    glfwShowWindow(context_.getHandle());
}

void RWindow::hide()
{
    glfwHideWindow(context_.getHandle());
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
            wctrl->vOffset_.setPos((resize.width() - n) / 2, 0);
            wctrl->size_.setWidth(n);
            wctrl->size_.setHeight(resize.height());
        }
        else
        {
            n = static_cast<int>(resize.width() / wctrl->format_.vRatio_);
            glViewport(0, (resize.height() - n) / 2, resize.width(), n);
            wctrl->vOffset_.setPos(0, (resize.height() - n) / 2);
            wctrl->size_.setWidth(resize.width());
            wctrl->size_.setHeight(n);
        }
        break;
    }
    case Viewport::Full:
    {
        glViewport(0, 0, resize.width(), resize.height());
        wctrl->vOffset_.setPos(0, 0);
        wctrl->size_.setSize(resize.width(), resize.height());
        break;
    }
    case Viewport::Fix:
    {
        glViewport((resize.width() - wctrl->size_.width()) / 2.0,
                   (resize.height() - wctrl->size_.height()) / 2.0,
                   wctrl->size_.width(),
                   wctrl->size_.height());
        wctrl->vOffset_.setPos((resize.width() - wctrl->size_.width()) / 2.0,
                            (resize.height() - wctrl->size_.height()) / 2.0);
        break;
    }
    }

    // 传递Translation info
    wctrl->node.transform(&wctrl->node, RRect(RPoint(), wctrl->size_));
}

void RWindow::mouseScrollCallback(GLFWwindow *, double x, double y)
{
    RInput::mouseWheel(x, y);
}

void RWindow::windowFocusCallback(GLFWwindow *window, int focused)
{
    if(focused == GLFW_TRUE)
    {
        RWindow *wctrl = getWindowUserCtrl(window);
        focusWindowP = wctrl;
    }
}

void RWindow::windowCloseCallback(GLFWwindow *window)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    bool stop = false;
    wctrl->closed(stop);

    if(stop)
        glfwSetWindowShouldClose(window, GLFW_FALSE);
    else
        wctrl->node.breakLooping();
}

void RWindow::keyboardCollback(GLFWwindow *, int key, int , int action, int )
{
    if (action == GLFW_RELEASE)
        RInput::keyUp(RInput::toKey(key));
    else if (action == GLFW_PRESS)
        RInput::keyDown(RInput::toKey(key));
    else if (action == GLFW_REPEAT)
        RInput::keyRepeat(RInput::toKey(key));
}

void RWindow::mouseButtonCollback(GLFWwindow *, int btn, int action, int )
{
    if (action == GLFW_RELEASE)
        RInput::mouseUp(RInput::toMouseButtons(btn));
    else if (action == GLFW_PRESS)
        RInput::mouseDown(RInput::toMouseButtons(btn));
}

void RWindow::cursorPosCollback(GLFWwindow *, double, double)
{
    RInput::setCursorMove();
}

void RWindow::charInputCollback(GLFWwindow *, unsigned code)
{
    RInput::charInput(code);
}

int RWindow::defaultExec()
{
    // 不在构造函数时设置回调，防止多线程中在未构造完成时被调用
    glfwSetWindowFocusCallback(context_.getHandle(), windowFocusCallback);
    glfwSetFramebufferSizeCallback(context_.getHandle(), resizeCallback);
    glfwSetScrollCallback(context_.getHandle(), mouseScrollCallback);
    glfwSetWindowCloseCallback(context_.getHandle(), windowCloseCallback);
    glfwSetKeyCallback(context_.getHandle(), keyboardCollback);
    glfwSetMouseButtonCallback(context_.getHandle(), mouseButtonCollback);
    glfwSetCursorPosCallback(context_.getHandle(), cursorPosCollback);
    glfwSetCharCallback(context_.getHandle(), charInputCollback);

    node.dispatchStartEvent();

    RSize size = windowSize();
    resizeCallback(context_.getHandle(), size.width(), size.height());

    RNode::Instructs instructs;
    while(node.isLooping())
    {
        // 清理指令
        instructs.clear();
        // 清空输入
        RInput::updataInput();
        // 清屏 清除颜色缓冲[深度缓冲、模板缓冲]
        glClear(clearMask_);

        if(isFocus())
        {
            // GLFW event process
            glfwPollEvents();
            // 发起处理event
            node.process(&node, &instructs);
        }
        // 发起更新
        node.update(renderSys_.get());

        glfwSwapBuffers(context_.getHandle());
    }

    node.dispatchFinishEvent();

    if(!node.isNormal())
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
