#include <RSprite.h>
#include <render/RTexture.h>

using namespace Redopera;

RSprite::RSprite(const RSprite &sprite):
    interval_(sprite.interval_),
    frames_(sprite.frames_)
{

}

RSprite::RSprite(const RSprite &&sprite):
    interval_(sprite.interval_),
    frames_(std::move(sprite.frames_))
{

}

RSprite &RSprite::operator=(const RSprite &sprite)
{
    frames_ = sprite.frames_;
    interval_ = sprite.interval_;

    return *this;
}

RSprite& RSprite::operator=(const RSprite &&sprite)
{
    frames_ = std::move(sprite.frames_);
    interval_ = sprite.interval_;

    return *this;
}

const RTexture &RSprite::texture() const
{
    return frames_[index_];
}

int RSprite::delta() const
{
    return delta_;
}

size_t RSprite::frameCount() const
{
    return frames_.size();
}

size_t RSprite::currentIndex() const
{
    return index_;
}

const RTexture &RSprite::frame() const
{
    return frame(index_);
}

const RTexture &RSprite::frame(size_t index) const
{
    return frames_.at(index);
}

void RSprite::play()
{
    if(delta_ < interval_)
        ++delta_;
    else
    {
        delta_ = 0;
        index_ = (index_ + 1) % frames_.size();
    }
}

void RSprite::setInterval(int interval)
{
    interval_ = interval;
}

void RSprite::setIndex(size_t index)
{
    index_ = index % frames_.size();
}

void RSprite::add(const RTexture &frame)
{
    frames_.push_back(frame);
}

void RSprite::add(const std::initializer_list<RTexture> &list)
{
    frames_.insert(frames_.end(), list);
}

void RSprite::remove(size_t index)
{
    frames_.erase(frames_.begin() + index);
    index_ = 0;
}

void RSprite::clear()
{
    frames_.clear();
    index_ = 0;
}
