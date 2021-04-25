#ifndef RSPRITE_H
#define RSPRITE_H

#include "rsc/RTexture.h"

#include <initializer_list>
#include <vector>

namespace Redopera {

class RSprite
{
public:
    RSprite() = default;
    ~RSprite() = default;

    RSprite(const RSprite &sprite);
    RSprite(const RSprite &&sprite);
    RSprite& operator=(const RSprite &sprite);
    RSprite& operator=(const RSprite &&sprite);

    const RTexture& texture() const;

    int delta() const;
    size_t frameCount() const;
    size_t currentIndex() const;
    const RTexture& frame(size_t index) const;

    void setInterval(int interval);

    void add(const RTexture &frame);
    void add(const std::vector<RTexture> &texs);
    void remove(size_t index);

    void clear();

private:
    std::vector<RTexture> frames_;
    int interval_ = 20;
    mutable int delta_ = 0;
    mutable size_t index_ = 0;
};

} // Redopera

#endif // RSPRITE_H
