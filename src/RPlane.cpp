#include <RPlane.h>
#include <RRect.h>
#include <rsc/RImage.h>

using namespace Redopera;

RPlane::RPlane():
    RPlane(0, 0, 0, 0, 0, RTexture())
{

}

RPlane::RPlane(int width, int height, int x, int y, int z, const RTexture &tex):
    RPlane(RSize(width, height), RPoint(x, y, z), tex)
{

}

RPlane::RPlane(int width, int height, const RPoint &pos, const RTexture &tex):
    RPlane(RSize(width, height), pos, tex)
{

}

RPlane::RPlane(const RSize &size, const RPoint &pos, const RTexture &tex):
    size_(size),
    pos_(pos),
    model_(1),
    texture_(tex)
{

}

RPlane::RPlane(const RRect &rect, int z, const RTexture &tex):
    RPlane(rect.size(), RPoint(rect.bottomLeft(), z), tex)
{

}

RPlane::RPlane(const RPlane &plane):
    dirty_(plane.dirty_),
    flip_(plane.flip_),
    rotate_(plane.rotate_),
    size_(plane.size_),
    pos_(plane.pos_),
    model_(plane.model_),
    texture_(plane.texture_)
{

}

RPlane::RPlane(const RPlane &&plane):
    dirty_(plane.dirty_),
    flip_(plane.flip_),
    rotate_(plane.rotate_),
    size_(plane.size_),
    pos_(plane.pos_),
    model_(std::move(plane.model_)),
    texture_(std::move(plane.texture_))
{

}

RPlane &RPlane::operator=(const RPlane &plane)
{
    dirty_ = plane.dirty_;
    flip_ = plane.flip_;
    rotate_ = plane.rotate_;
    size_ = plane.size_;
    pos_ = plane.pos_;
    model_ = plane.model_;
    texture_ = plane.texture_;

    return *this;
}

RPlane &RPlane::operator=(const RPlane &&plane)
{
    dirty_ = plane.dirty_;
    flip_ = plane.flip_;
    rotate_ = plane.rotate_;
    size_ = plane.size_;
    pos_ = plane.pos_;
    model_ = std::move(plane.model_);
    texture_ = std::move(plane.texture_);

    return *this;
}

const glm::mat4 &RPlane::model()
{
    if(dirty_) updataMat();
    return model_;
}

bool RPlane::isDirty() const
{
    return dirty_;
}

const RPoint &RPlane::pos() const
{
    return pos_;
}

const RSize &RPlane::size() const
{
    return size_;
}

RRect RPlane::rect() const
{
    return RRect(size_, pos_);
}

const RTexture &RPlane::texture() const
{
    return texture_;
}

const float *RPlane::rotate() const
{
    return &rotate_.x;
}

RPoint &RPlane::rPos()
{
    dirty_ = true;
    return pos_;
}

RSize &RPlane::rSize()
{
    dirty_ = true;
    return size_;
}

void RPlane::setRotate(float x, float y, float z)
{
    rotate_ = { x, y, z };
}

void RPlane::setRotate(float *xyz)
{
    rotate_ = { xyz[0], xyz[1], xyz[2] };
}

void RPlane::setTexture(const RTexture &tex)
{
    texture_ = tex;
}

void RPlane::flipH()
{
    dirty_ = true;
    flip_.h = !flip_.h;
}

void RPlane::flipV()
{
    dirty_ = true;
    flip_.v = !flip_.v;
}

void RPlane::updataMat()
{
    model_ = glm::translate(glm::mat4(1), { pos_.x() + size_.width()/2, pos_.y() + size_.height()/2, 0 });
    model_ *= glm::mat4_cast(glm::qua<float>(glm::vec3{ rotate_.x, rotate_.y, rotate_.z }));
    model_ = glm::scale(model_, { size_.width(), size_.height(), 0.0f });

    if(flip_.h)
    {
        model_[0][0] *= -1;
        model_[0][1] *= -1;
        model_[0][2] *= -1;
        model_[0][3] *= -1;
    }
    if(flip_.v)
    {
        model_[1][0] *= -1;
        model_[1][1] *= -1;
        model_[1][2] *= -1;
        model_[1][3] *= -1;
    }
    dirty_ = false;
}
