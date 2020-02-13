#ifndef RIMAGE_H
#define RIMAGE_H

#include "RResource.h"
#include "../RColor.h"
#include "../RSize.h"
#include "../dependents/stb_image.h"

namespace Redopera {

class RImage : public RResource
{
    friend void swap(RImage &img1, RImage &img2);

public:
    static const RImage& redoperaIcon();

    RImage();
    explicit RImage(const std::string &path, const std::string &name = "Image", bool flip = false);
    RImage(const RData *data, size_t size, const std::string &name = "Image", bool flip = false);
    RImage(const RData *data, int width, int height, int channel, const std::string &name = "Image");
    RImage(std::shared_ptr<RData> data, int width, int height, int channel, const std::string &name = "Image");
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

    bool load(const std::string path, bool flip = false);
    bool load(const RData *buf, size_t size, bool flip = false);
    bool load(const RData *data, int width, int height, int channel);
    bool load(std::shared_ptr<RData> data, int width, int height, int channel);
    void flipVertical();
    void flipHorizontal();
    void full(const RColor &color);
    void release();

protected:
    void copyOnWrite();

private:
    static const RData REDOPERA_ICON[];

    std::shared_ptr<RData> data_;
    int width_ = 0;
    int height_ = 0;
    int channel_ = 0;
};

} // Redopera

void swap(Redopera::RImage &img1, Redopera::RImage &img2);

#endif // RIMAGE_H
