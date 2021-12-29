#include <RFormat.h>
#include <render/RContext.h>
#include <RGame.h>

#include <SDL2/SDL.h>

using namespace Redopera;

int main()
{
    fmt::print("======== OpenGL Context Create Test ========\n\n");

    RGame game;
    RWindow w(1, 1, "Context", SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);

    fmt::print("Try Create OpenGL3.3 Context...\n");
    RContext::Format format;
    format.versionMajor = 3;
    format.versionMinor = 3;
    RContext context(w, format);

    if(context)
    {
        fmt::print("OpenGl Version {}.{} created.\n", format.versionMajor, format.versionMinor);

        format.versionMajor = 4;
        fmt::print("\nTry create high version...\n");
        for(int i = 0; i <= 6; ++i)
        {
            format.versionMinor = i;

            if(context.setContext(w, format))
                fmt::print("OpenGl Version {}.{} created.\n", format.versionMajor, format.versionMinor);
            else
            {
                rError("OpenGl Version {}.{} create failed!\n", format.versionMajor, format.versionMinor);
                break;
            }
        }
    }
    else
        fmt::print("OpenGl Version 3.3 create failed!\n");

    fmt::print("\nTest end\n");
    return 0;
}
