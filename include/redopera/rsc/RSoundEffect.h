#ifndef RSOUNDEFFECT_H
#define RSOUNDEFFECT_H

#include "RResource.h"
#include "../RDefine.h"
#include <SDL2/SDL_mixer.h>

namespace Redopera {

class RSoundEffect : public basic::ResourceBasic<Mix_Chunk, void(*)(Mix_Chunk *)>
{
    using RSC = basic::ResourceBasic<Mix_Chunk, void(*)(Mix_Chunk *)>;

public:
    static int channel();
    static int setChannelVolume(int volume);
    static int channelVolume();

    RSoundEffect() = default;

    explicit RSoundEffect(const std::string &path);
    explicit RSoundEffect(const char *path);
    RSoundEffect(const RData *data, size_t size);
    RSoundEffect(const RSoundEffect &chunk);
    RSoundEffect(const RSoundEffect &&chunk);
    RSoundEffect& operator=(RSoundEffect chunk);
    void swap(RSoundEffect &chunk);
    ~RSoundEffect() = default;

    int volume() const;

    bool load(const std::string &path);
    bool load(const char *path);
    bool load(const RData *data, size_t size);

    int play(int loop = 1);
    int setVolume(int volume);
};

} // Redopera

void swap(Redopera::RSoundEffect &chunk1, Redopera::RSoundEffect &chunk2);

#endif // RSOUNDEFFECT_H
