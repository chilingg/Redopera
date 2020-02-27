#include <RDebug.h>
#include <RContext.h>

using namespace Redopera;

// 使用RWindow的场合，初始化与结束由RWindow实例负责

int main()
{
    rDebug << "======== OpenGL Context Create Test ========\n";

    RContext contex;
    if(check(!contex, "Failure initialization OpenGL context!"))
        exit(EXIT_FAILURE);

    rDebug << "Try Create OpenGL3.3 Context...";
    RContext::Format format;
    format.versionMajor = 3;
    format.versionMinor = 3;

    if(contex.setContex(format))
        rDebug << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created";

    format.versionMajor = 4;
    rDebug << "\nTry create high version...";
    for(int i = 0; i <= 6; ++i)
    {
        format.versionMinor = i;

        if(contex.setContex(format))
            rDebug << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created";
        else
            break;
    }

    rDebug << "\nTest end";
    return 0;
}
