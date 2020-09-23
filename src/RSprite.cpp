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
    sequence_(sprite.sequence_),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_)
{

}

RSprite::RSprite(const RSprite &&sprite):
    RPlane(std::move(sprite)),
    frames_(std::move(sprite.frames_)),
    sequence_(std::move(sprite.sequence_)),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_)
{

}

RPlane &RSprite::operator=(const RSprite &sprite)
{
    RPlane::operator=(sprite);
    frames_ = sprite.frames_;
    sequence_ = sprite.sequence_;
    interval_ = sprite.interval_;
    delta_ = sprite.delta_;
    index_ = sprite.index_;

    return *this;
}

RPlane &RSprite::operator=(const RSprite &&sprite)
{
    RPlane::operator=(std::move(sprite));
    frames_ = std::move(sprite.frames_);
    sequence_ = std::move(sprite.sequence_);
    interval_ = sprite.interval_;
    delta_ = sprite.delta_;
    index_ = sprite.index_;

    return *this;
}

size_t RSprite::frameCount() const
{
    return frames_.size();
}

size_t RSprite::currentIndex() const
{
    return index_;
}

int RSprite::delta() const
{
    return delta_;
}

const RTexture RSprite::frame(size_t index) const
{
    return frames_[index];
}

void RSprite::setInterval(int interval)
{
    interval_ = interval;
}

void RSprite::setFrameSequence(const std::vector<size_t> &seque)
{
    sequence_ = seque;
    index_ = 0;
    delta_ = 0;
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
    index_ = 0;
    sequence_.clear();
    for (size_t i = 0; i < texs.size(); ++i)
        sequence_.push_back(i + frames_.size());

    frames_.insert(frames_.end(), texs.begin(), texs.end());
}

const RTexture &RSprite::texture() const
{
    if(delta_ < interval_)
        ++delta_;
    else {
        delta_ = 0;
        index_ = (index_ + 1) % sequence_.size();
    }

    return frames_[sequence_[index_]];
}
