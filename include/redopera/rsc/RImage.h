#ifndef RIMAGE_H
#define RIMAGE_H

#include "RResource.h"
#include "../RDefine.h"
#include "../RSize.h"

namespace Redopera {

class RImage : public basic::ResourceBasic<RData, void(*)(void*)>
{
    using RSC = basic::ResourceBasic<RData, void(*)(void*)>;

public:
    static const RImage& redoperaIcon();

    RImage() = default;

    explicit RImage(const std::string &path, bool flip = false);
    explicit RImage(const char *path, bool flip = false);
    RImage(const RData *data, size_t size, bool flip = false);
    RImage(const RData *data, int width, int height, int channel);
    RImage(const RData *data, const RSize &size, int channel);
    RImage(const RImage &img);
    RImage& operator=(RImage img);
    RImage(RImage &&img);
    RImage& operator=(RImage &&img);
    ~RImage() = default;
    void swap(RImage &img);

    int width() const;
    int height() const;
    RSize size() const;
    int channel() const;

    bool load(const std::string &path, bool flip = false);
    bool load(const char *path, bool flip = false);
    bool load(const RData *buf, size_t size, bool flip = false);
    bool load(const RData *data, int width, int height, int channel);
    bool load(const RData *data, const RSize &size, int channel);
    RImage& flipV();
    RImage& flipH();
    void rotate90();
    void fill(RGBA rgba);
    void release();

private:
    static const RData REDOPERA_ICON[];

    void copyOnWrite();

    int width_ = 0;
    int height_ = 0;
    int channel_ = 0;
};

} // Redopera

void swap(Redopera::RImage &img1, Redopera::RImage &img2);

#endif // RIMAGE_H
