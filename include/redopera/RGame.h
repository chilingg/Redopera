#ifndef RGAME_H
#define RGAME_H

#include "ROpenGL.h"
#include "RDebug.h"
#include "RWindow.h"

namespace Redopera {

class RGame
{
public:
    RGame()
    {
        if(init)
            throw "GLFW has been initialized!";

        glfwSetErrorCallback(errorCallback);
        init = glfwInit() == GLFW_TRUE ? true : false;
    }

    ~RGame() { init = false; };

    explicit operator bool() const { return init; }

    RGame(const RGame&) = delete;
    RGame& operator=(const RGame&) = delete;

private:
    static void errorCallback(int error, const char* description)
    {
        rPrError("GLFW Error " + std::to_string(error) + ": " + description);
    }

    static bool init;
};

} // ns Redopera

#endif //RGAME_H
