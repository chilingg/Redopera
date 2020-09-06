#include <RSprite.h>
#include <RRect.h>

using namespace Redopera;

RSprite::RSprite():
    RSprite(RSize(), RPoint(0))
{

}

RSprite::RSprite(int width, int height, int x, int y, int z):
    RSprite(RSize(width, height), RPoint(x, y, z))
{

}

RSprite::RSprite(int width, int height, const RPoint &pos):
    RSprite(RSize(width, height), pos)
{

}

RSprite::RSprite(const RSize &size, const RPoint &pos):
    RPlane(size, pos)
{

}

RSprite::RSprite(const RRect &rect, int z):
    RSprite(rect.size(), RPoint(rect.bottomLeft(), z))
{

}

RSprite::RSprite(const RSprite &sprite):
    RPlane(sprite),
    frames_(sprite.frames_),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_)
{

}

RSprite::RSprite(const RSprite &&sprite):
    RPlane(std::move(sprite)),
    frames_(std::move(sprite.frames_)),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_)
{

}

size_t RSprite::frameCount() const
{
    return frames_.size();
}

size_t RSprite::currentIndex() const
{
    return index_;
}

const RTexture RSprite::frame(size_t index) const
{
    return frames_[index];
}

void RSprite::setInterval(int interval)
{
    interval_ = interval;
}

void RSprite::setFrame(size_t n)
{
    n = std::min(n, frames_.size());
    index_ = n;
}

void RSprite::clear()
{
    frames_.clear();
}

void RSprite::add(const RTexture &frame)
{
    frames_.push_back(frame);
}

void RSprite::add(const std::vector<RTexture> &texs)
{
    frames_.insert(frames_.end(), texs.begin(), texs.end());
}

void RSprite::add(std::initializer_list<RTexture> texs)
{
    frames_.insert(frames_.end(), texs);
}

const RTexture &RSprite::texture() const
{
    if(delta_ < interval_)
        ++delta_;
    else {
        delta_ = 0;
        index_ = ++index_ % frames_.size();
    }

    return frames_[index_];
}
