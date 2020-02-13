#include "RWindow.h"
#include "RDebug.h"

using namespace Redopera;

RWindow::Format RWindow::windowFormat;
std::once_flag RWindow::init;

RWindow* RWindow::mainWindow(nullptr);

RWindow *RWindow::getMainWindow()
{
    return mainWindow;
}

void RWindow::setDefaultWindowFormat(const Format &format)
{
    windowFormat = format;
}

bool RWindow::updateGamepadMappings(const std::string &path)
{
    std::string mappingCode = RResource::getTextFileContent(path);
    if(glfwUpdateGamepadMappings(mappingCode.c_str()) == GLFW_FALSE)
    {
        prError("Failed to update gamepad mapping! In path: " + path + '\n' +
                   "To https://github.com/gabomdq/SDL_GameControllerDB download gamecontrollerdb.txt file.");
        return false;
    } else
        return true;
}

RWindow::RWindow(RController *parent, const std::string &name):
    RWindow(windowFormat, parent, name)
{

}

RWindow::RWindow(const RWindow::Format &format, RController *parent, const std::string &name):
    RController(parent, name),
    format_(format),
    eventPool([]{}),
    window_(nullptr, glfwDestroyWindow),
    vOffset_(0),
    width_(format.initWidth),
    height_(format.initHeight),
    focused_(false)
{
    std::call_once(init, std::bind(initMainWindow, this));

    // 一个线程窗口只能有一个窗口
    if(check(RContext::contex != nullptr, "A thread can only have one context!"))
        exit(EXIT_FAILURE);

    // Debug Context 需要OpenGL4.3以上版本
    if(format_.versionMajor * 10 + format_.versionMinor < 43)
        format_.debug = false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, format_.versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, format_.versionMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, format_.forward);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, format_.debug);
    glfwWindowHint(GLFW_RESIZABLE, !format_.fix);
    glfwWindowHint(GLFW_DECORATED, format_.decorate);
    // 默认初始窗口不可见，需主动调用show()
    glfwWindowHint(GLFW_VISIBLE, false);

    window_.reset(glfwCreateWindow(format_.initWidth, format_.initHeight,
                               name.c_str(), nullptr, format_.shared));
    if(check(window_ == nullptr, "Fainled to create GLFW window!"))
        exit(EXIT_FAILURE);

    //绑定上下文与this指针
    glfwSetWindowUserPointer(window_.get(), this);
    glfwMakeContextCurrent(window_.get());

    if(check(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Failed to initialize GLAD"))
    {
        terminateTree(Status::Error);
        exit(EXIT_FAILURE);
    }

    // 申请到的OpenGL版本
    format_.versionMajor = GLVersion.major;
    format_.versionMinor = GLVersion.minor;

    //若启用 OpenGL Debug
    if(format_.debug && GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        rDebug << printFormat::green << printFormat::bold << name << ": " << glGetString(GL_VERSION) << printFormat::non;
        rDebug << printFormat::green << printFormat::bold << "Enable OpenGL debug output" << printFormat::non;
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openglDebugMessageCallback, nullptr);
        //过滤着色器编译成功消息通知
        glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_OTHER,
                              GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER,
                              GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    }

    glfwSwapInterval(format_.vSync ? 1 : 0);
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

void RWindow::setFullScreenWindow(bool b)
{
    static int w, h;
    w = windowWidth();
    h = windowHeight();

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
    if(b)
        glfwSetWindowMonitor(window_.get(), monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
    else
        glfwSetWindowMonitor(window_.get(), nullptr, (vidmode->width-w)/2, (vidmode->height-h)/2,
                             w, h, vidmode->refreshRate);
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

void RWindow::setBackColor(unsigned r, unsigned g, unsigned b)
{
    format_.background = RColor(r, g, b, 255).rgba();
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

void RWindow::setBackColor(const RColor &color)
{
    format_.background = color.rgba();
    glClearColor(color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, 1.0f);
}

void RWindow::setBackColor(R_RGBA rgba)
{
    RColor color(rgba);
    format_.background = rgba;
    glClearColor(color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, 1.0f);
}

void RWindow::setViewportSize(int width, int height)
{
    width_ = width;
    height_ = height;
    if(isLooping())
        resizeCallback(window_.get(), windowWidth(), windowHeight());
}

void RWindow::setViewportRatio(double ratio)
{
    format_.vRatio_ = ratio;
    if(isLooping())
        resizeCallback(window_.get(), windowWidth(), windowHeight());
}

void RWindow::setViewportPattern(RWindow::Viewport pattern)
{
    format_.viewport = pattern;
    if(isLooping())
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
    return width_;
}

int RWindow::height() const
{
    return height_;
}

RSize RWindow::size() const
{
    return RSize(width_, height_);
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

void RWindow::closeWindow()
{
    glfwSetWindowShouldClose(window_.get(), GLFW_TRUE);
}

void RWindow::show()
{
    // 不在构造函数时设置回调，防止多线程中在未构造完成时被调用
    glfwSetFramebufferSizeCallback(window_.get(), resizeCallback);
    glfwSetScrollCallback(window_.get(), mouseScrollCallback);
    glfwSetWindowFocusCallback(window_.get(), windowFocusCallback);
    glfwSetWindowCloseCallback(window_.get(), windowCloseCallback);
    // 若无需实时响应，则无需开启
    if(format_.keysSigal)
        glfwSetKeyCallback(window_.get(), keyboardCollback);
    // 主窗口关闭时所有窗口都会得到通知
    if(mainWindow != this)
       mainWindow->closed.connect(this, &RController::breakLoop);

    glfwShowWindow(window_.get());
}

void RWindow::hide()
{
    glfwHideWindow(window_.get());
}

int RWindow::exec()
{
    assert(!isLooping());

    RStartEvent sEvent(this);
    dispatchEvent(sEvent);

    while(loopingCheck() == Status::Looping)
    {
        eventPool(); //更新事件

        if(focused_) //更新输入
        {
            //更新手柄输入
            RInputModule::instance().updateGamepad();
            //更新键鼠输入
            RInputModule::instance().updateKeyboardInput(window_.get());
            RInputModule::instance().updateMouseInput(window_.get());
            //光标位置
            double xpos, ypos;
            glfwGetCursorPos(window_.get(), &xpos, &ypos);
            RInputModule::instance().updateCursorPos(static_cast<int>(xpos) - vOffset_.x(),
                                                     static_cast<int>(ypos) - vOffset_.y());
            //发布输入事件
            RInputEvent e(this);
            dispatchEvent(e);
        }

        //清屏 清除颜色缓冲和深度缓冲
        glClear(clearMask);

        activeOnce();

        glfwSwapBuffers(window_.get());
    }

    RFinishEvent fEvent(this);
    dispatchEvent(fEvent);
    closed.emit();

    if(check(status() == Status::Error, "The Loop has unexpectedly finished"))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void RWindow::translation(const RController::TranslationInfo &info)
{
    // RWindow不会转递上层发送的变换信息
    if(info.sender == this)
        RController::translation(info);
}

RController::Status RWindow::loopingCheck()
{
    if(status() == Status::Finished || isShouldCloused())
    {
        RCloseEvent e(this);
        dispatchEvent(e);
        if(e.stop)
        {
            setStatus(Status::Looping);
            glfwSetWindowShouldClose(window_.get(), GLFW_FALSE);
        }
    }

    return status();
}

void RWindow::initMainWindow(RWindow *window)
{
    // glfw错误回调
    glfwSetErrorCallback(glfwErrorCallback);
    // 初始化GLFW
    if(check(!RContext::initialization(), "Failed to initialize GLFW"))
        exit(EXIT_FAILURE);

    // 加载手柄映射
    std::string mappingCode = std::string() + RInputModule::gamepadMappingCode0
            + RInputModule::gamepadMappingCode1 + RInputModule::gamepadMappingCode2;
    check(glfwUpdateGamepadMappings(mappingCode.c_str()) == GLFW_FALSE, "Failed to load default gamepad mapping!\n"
          "To https://github.com/gabomdq/SDL_GameControllerDB download gamecontrollerdb.txt file.");

    // 手柄连接回调
    glfwSetJoystickCallback(joystickPresentCallback);
    // 需手动检测一次手柄连接，检测之前已连接的手柄
    for(int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
    {
        if(glfwJoystickIsGamepad(i))
            RInputModule::instance().addGamepad(RInputModule::toJoystickID(i));
    }

    // GLFW事件触发
    window->eventPool = &glfwPollEvents;
    mainWindow = window;
}

void RWindow::glfwErrorCallback(int error, const char *description)
{
    prError("Error " + std::to_string(error) + ": " + description);
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
        typeStr = "-Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeStr = "-Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeStr = "-Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeStr = "-Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeStr = "-Performance"; break;
    case GL_DEBUG_TYPE_MARKER:
        typeStr = "-Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeStr = "-Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeStr = "-Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:
        typeStr = "-Other"; break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << '(' << id << ')' << sourceStr << typeStr << "-high " << ">> "
                  << message << std::endl;
        terminateTree(Status::Error);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << '(' << id << ')' << sourceStr << typeStr << "-medium " << ">> "
                  << message << std::endl;
        terminateTree(Status::Error);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << printFormat::yellow << printFormat::bold << '(' << id << ')' << sourceStr << typeStr << "-low "
                  << ">> " << message << printFormat::non << std::endl;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << printFormat::green << printFormat::bold << '(' << id << ')' << sourceStr << typeStr << "-notification "
                  << ">> " << message << printFormat::non << std::endl;
        break;
    }
}

void RWindow::joystickPresentCallback(int jid, int event)
{
    if(event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid))
    {
        RInputModule::instance().addGamepad(RInputModule::toJoystickID(jid));
    }
    else if(event == GLFW_DISCONNECTED)
    {
        RInputModule::instance().deleteGamepad(RInputModule::toJoystickID(jid));
    }
}

void RWindow::resizeCallback(GLFWwindow *window, int width, int height)
{
    RWindow *wctrl = getWindowUserCtrl(window);

    switch(wctrl->format_.viewport)
    {
    case Viewport::Scale:
    {
        double ratio = static_cast<double>(width) / height;
        int n;
        if(ratio > wctrl->format_.vRatio_)
        {
            n = static_cast<int>(height * wctrl->format_.vRatio_);
            glViewport((width - n) / 2, 0, n, height);
            wctrl->vOffset_.set((width - n) / 2, 0);
            wctrl->width_ = n;
            wctrl->height_ = height;
        }
        else
        {
            n = static_cast<int>(width / wctrl->format_.vRatio_);
            glViewport(0, (height - n) / 2, width, n);
            wctrl->vOffset_.set(0, (height - n) / 2);
            wctrl->width_ = width;
            wctrl->height_ = n;
        }
        wctrl->width_ = width;
        wctrl->height_ =height;
        TranslationInfo info { wctrl, {width, height} };
        wctrl->translation(info);
        break;
    }
    case Viewport::Full:
    {
        glViewport(0, 0, width, height);
        wctrl->vOffset_.set(0, 0);
        wctrl->width_ = width;
        wctrl->height_ =height;
        TranslationInfo info { wctrl, {width, height} };
        wctrl->translation(info);
        break;
    }
    case Viewport::Fix:
    {
        glViewport((width - wctrl->width_) / 2.0, (height - wctrl->height_) / 2.0, wctrl->width_, wctrl->height_);
        wctrl->vOffset_.set((width - wctrl->width_) / 2.0, (height - wctrl->height_) / 2.0);
        TranslationInfo info { wctrl, {wctrl->width_, wctrl->height_} };
        wctrl->translation(info);
        break;
    }
    }
}

void RWindow::keyboardCollback(GLFWwindow *window, int key, int , int action, int mods)
{
    getWindowUserCtrl(window)->entered.emit(static_cast<Keys>(key), static_cast<ButtonAction>(action), static_cast<Modifier>(mods));
}

void RWindow::mouseScrollCallback(GLFWwindow *window, double , double y)
{
    getWindowUserCtrl(window)->rolled.emit(static_cast<int>(y));
}

void RWindow::windowFocusCallback(GLFWwindow *window, int focused)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    wctrl->focused_ = focused ? true : false;
}

void RWindow::windowCloseCallback(GLFWwindow *window)
{
    RWindow *wctrl = getWindowUserCtrl(window);
    wctrl->breakLoop();
}

RWindow *RWindow::getWindowUserCtrl(GLFWwindow *window)
{
    return static_cast<RWindow*>(glfwGetWindowUserPointer(window));
}
