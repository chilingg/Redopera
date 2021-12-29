#include <RWindow.h>
#include <rsc/RImage.h>
#include <RFormat.h>
#include <SDL2/SDL.h>

using namespace Redopera;

RWindow::RWindow():
    RWindow(800, 540, "Title")
{

}

RWindow::RWindow(SDL_Window *handle):
    window_(handle)
{

}

RWindow::RWindow(int width, int height, const char *title, uint32_t flag):
    window_(SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flag))
{
    if(!window_)
        rError("Window could not be created! SDL_Error: {}\n", SDL_GetError());
}

RWindow::~RWindow()
{
    if(window_)
        SDL_DestroyWindow(window_);
}

Redopera::RSize RWindow::size() const
{
    Redopera::RSize s;
    SDL_GetWindowSize(window_, &s.rWidth(), &s.rHeight());
    return s;
}

int RWindow::width() const
{
    return size().width();
}

int RWindow::height() const
{
    return size().height();
}

void RWindow::setIcon(const RImage &icon)
{
    if(icon.isValid())
    {
        SDL_Surface *s =
                SDL_CreateRGBSurfaceWithFormatFrom(icon.data(),
                                                   icon.width(),
                                                   icon.height(),
                                                   icon.channel() * 8,
                                                   icon.width() * icon.channel(),
                                                   icon.channel() == 4 ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24);
        SDL_SetWindowIcon(window_, s);
        SDL_FreeSurface(s);
    }
}

void RWindow::swapBuffers()
{
    SDL_GL_SwapWindow(window_);
}

void Redopera::RWindow::showCursor(bool enable)
{
    SDL_ShowCursor(enable ? SDL_ENABLE : SDL_DISABLE);
}

int RWindow::queryCursorStatus()
{
    return SDL_ShowCursor(SDL_QUERY);
}
