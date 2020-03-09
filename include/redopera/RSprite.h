#ifndef RSPRITE_H
#define RSPRITE_H

#include "RPlane.h"
#include <initializer_list>

namespace Redopera {

class RSprite : public RPlane
{
public:
    RSprite();
    RSprite(int width, int height, int x, int y, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RSprite(int width, int height, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    RSprite(const RSize &size, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    explicit RSprite(const RRect &rect, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RSprite(const RSprite &sprite);
    RSprite(const RSprite &&sprite);
    ~RSprite() = default;

    size_t frameCount() const;
    size_t currentIndex() const;

    void setInterval(int interval = 20);
    void setFrame(size_t n);
    void clear();

    void add(const RTexture &frame);
    void add(const std::vector<RTexture> &texs);
    void add(std::initializer_list<RTexture> texs);

protected:
    void spriteControl(const RShaderProg &shaders, GLuint mLoc);

private:
    std::vector<RTexture> frames_;
    int interval_ = 20;
    int delta_ = 0;
    size_t index_ = 0;
};

} // Redopera

#endif // RSPRITE_H
