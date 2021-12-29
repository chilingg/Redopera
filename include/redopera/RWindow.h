#ifndef RWINDOW_H
#define RWINDOW_H

#include "RSize.h"
#include <cstdint>

class SDL_Window;

namespace Redopera {

class RImage;

class RWindow
{
public:
    static void showCursor(bool enable);
    static int queryCursorStatus();

    RWindow();
    RWindow(SDL_Window *handle);
    RWindow(int width, int height, const char *title, uint32_t flag = 0);
    ~RWindow();

    RWindow(RWindow &) = delete;
    RWindow& operator=(RWindow &) = delete;

    Redopera::RSize size() const;
    int width() const;
    int height() const;
    SDL_Window* handle() const { return window_; }

    void setIcon(const RImage &icon);

    void swapBuffers();

private:
    SDL_Window *window_ = nullptr;
};

} // Redopera

#endif // RWINDOW_H
