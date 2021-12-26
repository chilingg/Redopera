#include <SDL2/SDL.h>
#include <RGame.h>
#include <RFormat.h>

using namespace Redopera;

bool RGame::init = false;

bool RGame::setHint(const char *name, const char *value)
{
    bool b = SDL_SetHint(name, value);
    if(b)
        rError("SDL could not set hint <{}>: {}\n", name, SDL_GetError());

    return b;
}

RGame::RGame():
    RGame(SDL_INIT_EVERYTHING)
{

}

RGame::RGame(__UINT32_TYPE__ flag)
{
    if(!init)
    {
        init = SDL_Init(flag) < 0 ? false : true;

        if(!init)
            rError("SDL could not initialize! SDL_Error: {}\n", SDL_GetError());
    }
}

RGame::~RGame()
{
    if(init)
        SDL_Quit();
    init = false;
}
