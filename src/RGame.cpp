#include <RGame.h>
#include <RWindow.h>
#include <RController.h>
#include <REvent.h>
#include <ROpenGL.h>
#include <RInputModule.h>
#include <rsc/RResource.h>
#include <RDebug.h>

#include <stdexcept>
#include <string>

using namespace Redopera;

bool RGame::once = false;

std::string RResource::rscPath_;

void glfwErrorCallback(int error, const char* description)
{
    prError("GLFW Error " + std::to_string(error) + ": " + description);
}

RGame::RGame()
{
    if (once)
        throw std::runtime_error("RGame is a singleton!");
    else
        once = true;

    // glfw错误回调
    glfwSetErrorCallback(glfwErrorCallback);

    if (glfwInit() == GLFW_TRUE)
        std::runtime_error("Failed to initialize GLFW");

}

RGame::~RGame()
{
    once = false;
    glfwTerminate();
}

int RGame::exec(RWindow *window)
{
    window->setAsMainWindow();

    // 需手动检测一次手柄连接，检测之前已连接的手柄
    for(int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
    {
        if(glfwJoystickIsGamepad(i))
        {
            RInputModule::addGamepad(RInputModule::toJoystickID(i));
            window->joyPresented.emit(RInputModule::toJoystickID(i), JoystickPresent::CONNECTED);
        }
    }
    // 手柄连接回调
    glfwSetJoystickCallback(joystickPresentCallback);

    return window->ctrl()->exec();
}

void RGame::joystickPresentCallback(int jid, int event)
{
    if(event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid))
    {
        RInputModule::addGamepad(RInputModule::toJoystickID(jid));
        RWindow::getMainWindow()->joyPresented.emit(RInputModule::toJoystickID(jid), JoystickPresent::CONNECTED);
    }
    else if(event == GLFW_DISCONNECTED)
    {
        RInputModule::deleteGamepad(RInputModule::toJoystickID(jid));
        RWindow::getMainWindow()->joyPresented.emit(RInputModule::toJoystickID(jid), JoystickPresent::DISCONNECTED);
    }
}

bool RGame::updateGamepadMappings(const char *path)
{
    RFile file = RResource::loadFile(path);
    if (!file.size)
        rDebug << "Failed to load gamepad mappings file: " << path;

    if(glfwUpdateGamepadMappings(reinterpret_cast<char*>(file.data.get())) == GLFW_FALSE)
    {
        prError("Failed to update gamepad mapping! In path: " + std::string(path) + '\n' +
                   "To https://github.com/gabomdq/SDL_GameControllerDB download gamecontrollerdb.txt file.");
        rDebug << "Updata to default gameoad maooings is " << (updateGamepadMappings() ? "success." : "failed!");
        return false;
    } else
        return true;
}

bool RGame::updateGamepadMappings()
{
    // 加载手柄映射
    std::string mappingCode = std::string() + RInputModule::gamepadMappingCode0
            + RInputModule::gamepadMappingCode1 + RInputModule::gamepadMappingCode2;

    return glfwUpdateGamepadMappings(mappingCode.c_str()) == GLFW_TRUE;
}
