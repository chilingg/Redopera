#include "RSprite.h"

using namespace Redopera;

RSprite::RSprite(int width, int height, int x, int y, int z, const RArea::Format &area):
    RSprite(RSize(width, height), RPoint(x, y, z), area)
{

}

RSprite::RSprite(int width, int height, const RPoint &pos, const RArea::Format &area):
    RSprite(RSize(width, height), pos, area)
{

}

RSprite::RSprite(const RSize &size, const RPoint &pos, const RArea::Format &area):
    RPlane(size, pos, area)
{
    renderControl = std::bind(&RSprite::spriteControl, this, std::placeholders::_1, std::placeholders::_2);
}

RSprite::RSprite(const RRect &rect, int z, const RArea::Format &area):
    RSprite(rect.size(), RPoint(rect.bottomLeft(), z), area)
{

}

RSprite::RSprite(const RSprite &sprite):
    RPlane(sprite),
    frames_(sprite.frames_),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_),
    playing_(sprite.playing_)
{

}

RSprite::RSprite(const RSprite &&sprite):
    RPlane(std::move(sprite)),
    frames_(std::move(sprite.frames_)),
    interval_(sprite.interval_),
    delta_(sprite.delta_),
    index_(sprite.index_),
    playing_(sprite.playing_)
{

}

size_t RSprite::frameCount()
{
    return frames_.size();
}

size_t RSprite::currentIndex()
{
    return index_;
}

bool RSprite::isPlaying()
{
    return playing_;
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

void RSprite::play()
{
    playing_ = true;
}

void RSprite::stop()
{
    playing_ = false;
}

void RSprite::spriteControl(const RShaderProg &shaders, GLuint mLoc)
{
    if(frames_.empty())
        RPlane::renderControl(shaders, mLoc);
    else
    {
        if(playing_)
        {
            if(delta_ < interval_)
                ++delta_;
            else {
                delta_ = 0;
                index_ = ++index_ % frames_.size();
            }
        }

        frames_[index_].bind();

        auto inter = shaders.useInterface();
        inter.setUniformMatrix(mLoc, modelMat());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
