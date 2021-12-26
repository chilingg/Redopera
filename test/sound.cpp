#include <RFormat.h>
#include <RGame.h>
#include <RWindow.h>
#include <rsc/RSoundEffect.h>
#include <SDL2/SDL.h>

using namespace Redopera;

int main( int argc, char* args[] )
{
    RGame g(SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    RWindow w(800, 540, "Sound");

    rCheck(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0,
           fmt::format("SDL_mixer could not initialize! Error: {}", Mix_GetError()));

    RSoundEffect se("TestFile/bicycle_bell.mp3");

    const char *info = "pressed 1 playing sound effect,\n"
                       "pressed ↑ ↓ change volume,\n";

    fmt::print("{}", info);

    int volume = MIX_MAX_VOLUME;
    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            else if(e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                case SDLK_1:
                    se.play();
                    break;
                case SDLK_DOWN:
                    volume -= 10;
                    Mix_Volume(-1, volume);
                    fmt::print("Channels: {}, volume: {}\n", Mix_AllocateChannels(-1), volume);
                    break;
                case SDLK_UP:
                    volume += 10;
                    Mix_Volume(-1, volume);
                    fmt::print("Channels: {}, volume: {}\n", Mix_AllocateChannels(-1), volume);
                    break;
                }
            }
        }
    }

    return 0;
}
