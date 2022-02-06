#ifndef RSPRITE_H
#define RSPRITE_H

#include <initializer_list>
#include <vector>

namespace Redopera {

class RTexture;

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
    const std::vector<RTexture>& textureList() const;
    int delta() const;
    size_t frameCount() const;
    size_t currentIndex() const;
    int interval() const;
    const RTexture& frame() const;
    const RTexture& frame(size_t index) const;

    void play();
    void setInterval(int interval);
    void setIndex(size_t index);
    void add(const RTexture &frame);
    void add(const std::initializer_list<RTexture> &list);
    void remove(size_t index);

    void clear();

private:
    int interval_ = 10;
    int delta_ = 0;
    size_t index_ = 0;
    std::vector<RTexture> frames_;
};

} // Redopera

#endif // RSPRITE_H
