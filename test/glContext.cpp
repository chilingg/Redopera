#include <RDebug.h>
#include <RContext.h>
#include <RGame.h>

using namespace Redopera;

int main()
{
    rDebug << "======== OpenGL Context Create Test ========\n";

    // Initilation GLFW
    RGame game;

    rDebug << "Try Create OpenGL3.3 Context...";
    RContext::Format format;
    format.versionMajor = 3;
    format.versionMinor = 3;
    RContext context(format);

    if(context)
    {
        rDebug << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created.";

        format.versionMajor = 4;
        rDebug << "\nTry create high version...";
        for(int i = 0; i <= 6; ++i)
        {
            format.versionMinor = i;

            if(context.setContext(format))
                rDebug << "OpenGl Version " << GLVersion.major << '.' << GLVersion.minor << " created.";
            else
            {
                rDebug << "OpenGl Version " << format.versionMajor << '.' << format.versionMinor << " create failed!";
                break;
            }
        }
    }
    else
        rDebug << "OpenGl Version 3.3 create failed!";

    rDebug << "\nTest end";
    return 0;
}
