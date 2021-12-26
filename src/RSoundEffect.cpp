#include <rsc/RSoundEffect.h>
#include <RAudio.h>

using namespace Redopera;

namespace  {

int channelVolumeValue = MIX_MAX_VOLUME;

} // ns

int RSoundEffect::channel()
{
    return Mix_AllocateChannels(-1);
}

int RSoundEffect::setChannelVolume(int volume)
{
    channelVolumeValue = volume;
    return Mix_Volume(-1, volume);
}

int RSoundEffect::channelVolume()
{
    return channelVolumeValue;
}

RSoundEffect::RSoundEffect(const std::string &path):
    RSoundEffect(path.c_str())
{

}

RSoundEffect::RSoundEffect(const char *path)
{
    load(path);
}

RSoundEffect::RSoundEffect(const RData *data, size_t size)
{
    load(data, size);
}

RSoundEffect::RSoundEffect(const RSoundEffect &chunk):
    RSC(chunk)
{

}

RSoundEffect::RSoundEffect(const RSoundEffect &&chunk):
    RSC(std::move(chunk))
{

}

RSoundEffect &RSoundEffect::operator=(RSoundEffect chunk)
{
    RSC::swap(chunk);
    return *this;
}

void RSoundEffect::swap(RSoundEffect &chunk)
{
    RSC::swap(chunk);
}

int RSoundEffect::volume() const
{
    return data()->volume;
}

bool RSoundEffect::load(const std::string &path)
{
    return load(path.c_str());
}

bool RSoundEffect::load(const char *path)
{
    basic::Audio::initAudio();

    reset(Mix_LoadWAV(path), Mix_FreeChunk);
    return !rCheck(!isValid(), "Failed to load music file! in {}\n", path);
}

bool RSoundEffect::load(const RData *data, size_t size)
{
    basic::Audio::initAudio();

    reset(Mix_LoadWAV_RW(SDL_RWFromConstMem(data, size), 0), Mix_FreeChunk);

    return !rCheck(!isValid(), "Failed to load music data in memory!\n");
}

int RSoundEffect::play(int loop)
{
    return Mix_PlayChannel(-1, data(), loop);
}

int RSoundEffect::setVolume(int volume)
{
    data()->volume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    return data()->volume;
}

void swap(Redopera::RSoundEffect &chunk1, Redopera::RSoundEffect &chunk2)
{
    chunk1.swap(chunk2);
}
