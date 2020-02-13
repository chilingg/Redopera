#include <RDebug.h>
#include <RContext.h>

using namespace Redopera;

// 使用RWindow的场合，初始化与结束由RWindow实例负责

int main()
{
    rDebug << "======== OpenGL Context Create Test ========\n";

    if(check(!RContext::initialization(), "Failure initialization OpenGL context!"))
        exit(EXIT_FAILURE);

    rDebug << "Try Create OpenGL3.3 Context...";
    RContext::Format format;
    format.versionMajor = 3;
    format.versionMinor = 3;

    if(RContext::setContexAsThisThread(format))
        std::cout << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created" << std::endl;

    RContext::destroyContexAsThisThread();
    format.versionMajor = 4;
    rDebug << "\nTry create high version...";
    for(int i = 0; i <= 6; ++i)
    {
        format.versionMinor = i;

        if(RContext::setContexAsThisThread(format))
            std::cout << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created" << std::endl;
        else
            break;
    }

    rDebug << "\nTest end";
    return 0;
}
