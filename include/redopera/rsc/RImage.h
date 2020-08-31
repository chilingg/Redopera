#ifndef RIMAGE_H
#define RIMAGE_H

#include "RResource.h"
#include <memory>

namespace Redopera {

class RSize;
class RColor;

class RImage
{
public:
    static const RImage& redoperaIcon();

    RImage() = default;
    explicit RImage(const std::string &path, bool flip = false);
    RImage(const RData *data, size_t size, bool flip = false);
    RImage(const RData *data, int width, int height, int channel);
    RImage(const RImage &img);
    RImage(const RImage &&img);
    RImage& operator=(RImage img);
    ~RImage() = default;
    void swap(RImage &img);

    bool isValid() const;
    int width() const;
    int height() const;
    RSize size() const;
    int channel() const;
    RData* data() const;

    bool load(std::string path, bool flip = false);
    bool load(const RData *buf, size_t size, bool flip = false);
    bool load(const RData *data, int width, int height, int channel);
    void flipV();
    void flipH();
    void rotate90();
    void fill(RData r, RData g = 0, RData b = 0, RData a = 0);
    void release();

private:
    static const RData REDOPERA_ICON[];

    void copyOnWrite();

    std::shared_ptr<RData> data_;
    int width_ = 0;
    int height_ = 0;
    int channel_ = 0;
};

} // Redopera

void swap(Redopera::RImage &img1, Redopera::RImage &img2);

#endif // RIMAGE_H
