#ifndef RMUSIC_H
#define RMUSIC_H

#include "RResource.h"
#include "../RDefine.h"
#include <SDL2/SDL_mixer.h>

namespace Redopera {

class RMusic : public basic::ResourceBasic<Mix_Music, void(*)(Mix_Music *)>
{
    using RSC = basic::ResourceBasic<Mix_Music, void(*)(Mix_Music *)>;

public:
    static bool isPlaying();
    static bool isPaused();
    static void paused();
    static void resume();
    static void halt();
    static int volume();
    static int setVolume(int volume);

    RMusic() = default;

    explicit RMusic(const std::string &path);
    explicit RMusic(const char *path);
    RMusic(const RData *data, size_t size);
    RMusic(const RMusic &music);
    RMusic(const RMusic &&music);
    RMusic& operator=(RMusic music);
    void swap(RMusic &music);
    ~RMusic() = default;

    bool load(const std::string &path);
    bool load(const char *path);
    bool load(const RData *data, size_t size);

    bool play(int loop);
};

} // Redopera

void swap(Redopera::RMusic &music1, Redopera::RMusic &music2);

#endif // RMUSIC_H
