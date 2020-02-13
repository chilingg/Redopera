#ifndef RSPRITE_H
#define RSPRITE_H

#include "RPlane.h"
#include <vector>

namespace Redopera {

class RSprite : public RPlane
{
public:
    RSprite() = default;
    RSprite(int width, int height, int x, int y, int z = 0, const std::vector<RTexture> &texs = {});
    RSprite(int width, int height, const RPoint &pos, const std::vector<RTexture> &texs = {});
    RSprite(const RSize &size, const RPoint &pos, const std::vector<RTexture> &texs = {});
    explicit RSprite(const RRect &rect, int z = 0, const std::vector<RTexture> &texs = {});
    explicit RSprite(const Format &area, const std::vector<RTexture> &texs = {});
    RSprite(const RSprite &sprite);
    RSprite(const RSprite &&sprite);
    ~RSprite() override = default;

    size_t frameCount();
    size_t currentIndex();
    bool isPlaying();

    void setInterval(int interval = 20);
    void setFrame(size_t n);

    void clear();
    void add(const RTexture &frame);
    void add(const std::vector<RTexture> &texs);
    void play();
    void stop();

protected:
    void renderControl(const RShaderProg &shaders, GLuint mLoc) override;

private:
    std::vector<RTexture> frames_;
    int interval_ = 20;
    int delta_ = 0;
    size_t index_ = 0;
    bool playing_ = false;
};

} // Redopera

#endif // RSPRITE_H
