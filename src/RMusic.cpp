#include <rsc/RMusic.h>
#include <RAudio.h>

using namespace Redopera;

bool RMusic::isPlaying()
{
    return Mix_PlayingMusic();
}

bool RMusic::isPaused()
{
    return Mix_PausedMusic();
}

void RMusic::paused()
{
    Mix_PauseMusic();
}

void RMusic::resume()
{
    Mix_ResumeMusic();
}

void RMusic::halt()
{
    Mix_HaltMusic();
}

int RMusic::volume()
{
    return Mix_VolumeMusic(-1);
}

int RMusic::setVolume(int volume)
{
    return Mix_VolumeMusic(volume);
}

RMusic::RMusic(const std::string &path):
    RMusic(path.c_str())
{

}

RMusic::RMusic(const char *path)
{
    load(path);
}

RMusic::RMusic(const RData *data, size_t size)
{
    load(data, size);
}

RMusic::RMusic(const RMusic &music):
    RSC(music)
{

}

RMusic::RMusic(const RMusic &&music):
    RSC(music)
{

}

RMusic &RMusic::operator=(RMusic music)
{
    swap(music);
    return *this;
}

void RMusic::swap(RMusic &music)
{
    RSC::swap(music);
}

bool RMusic::load(const std::string &path)
{
    return load(path.c_str());
}

bool RMusic::load(const char *path)
{
    basic::Audio::initAudio();

    reset(Mix_LoadMUS(path), Mix_FreeMusic);

    return !rCheck(!isValid(), "Failed to load music file! in {}\n", path);
}

bool RMusic::load(const RData *data, size_t size)
{
    basic::Audio::initAudio();

    reset(Mix_LoadMUS_RW(SDL_RWFromConstMem(data, size), 0), Mix_FreeMusic);

    return !rCheck(!isValid(), "Failed to load music data in memory!\n");
}

bool RMusic::play(int loop)
{
    return rCheck(Mix_PlayMusic(data(), loop) < 0, "Error playing music: {}", Mix_GetError());
}

void swap(Redopera::RMusic &music1, Redopera::RMusic &music2)
{
    music1.swap(music2);
}
