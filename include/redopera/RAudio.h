#ifndef RAUDIO_H
#define RAUDIO_H

#include <SDL2/SDL_mixer.h>
#include <RFormat.h>

namespace Redopera {

namespace basic
{

class Audio
{
public:
    static void initAudio() { static Audio audio; }

private:
    Audio()
    {
        // 44100, CD quality
        rCheck(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0,
                             "SDL_Mixer could not initialize! Error: {}", Mix_GetError());
    }

    ~Audio() { Mix_CloseAudio(); Mix_Quit(); }
};

} // basic

} // ns Redopera

#endif // RAUDIO_H
