#include <RTransform.h>

using namespace Redopera;

RTransform::RTransform():
    RTransform(RPoint(0, 0, 0), RSize(0, 0))
{

}

bool RTransform::isDirty() const
{
    return dirty_;
}

RTransform::RTransform(int x, int y, int depth, int width, int height):
    RTransform(RPoint(x, y, depth), RSize(width, height))
{

}

RTransform::RTransform(const RPoint &pos, const RSize &size):
    RTransform(RRect(pos, size), pos.z())
{

}

RTransform::RTransform(const RRect &rect, int depth):
    depth_(depth),
    rect_(rect),
    rotate_(0.f, 0.f, 0.f)
{

}

const glm::mat4 &RTransform::model() const
{
    if(dirty_)
        updateModel();
    return model_;
}

const RPoint2 &RTransform::pos() const
{
    return rect_.pos();
}

int RTransform::depth() const
{
    return depth_;
}

const RSize &RTransform::size() const
{
    return rect_.size();
}

const RRect &RTransform::rect() const
{
    return rect_;
}

const glm::vec3 &RTransform::rotate() const
{
    return rotate_;
}

bool RTransform::isFlipH() const
{
    return flip_.h;
}

bool RTransform::isFlipV() const
{
    return flip_.v;
}

RRect &RTransform::rRect()
{
    dirty_ = true;
    return rect_;
}

void RTransform::setRect(const RRect &rect)
{
    dirty_ = true;
    rect_ = rect;
}

void RTransform::setDepth(int depth)
{
    dirty_ = true;
    depth_ = depth;
}

void RTransform::setSize(const RSize &size)
{
    dirty_ = true;
    rect_.setSize(size);
}

void RTransform::setSize(int width, int height)
{
    dirty_ = true;
    rect_.setSize(width, height);
}

void RTransform::setPos(const RPoint &pos)
{
    dirty_ = true;
    rect_.setPos(pos);
}

void RTransform::setPos(int x, int y)
{
    dirty_ = true;
    rect_.setPos(x, y);
}

void RTransform::setRotate(float x, float y, float z)
{
    dirty_ = true;
    rotate_ = { x, y, z };
}

void RTransform::setRotate(const glm::vec3 &vec)
{
    dirty_ = true;
    rotate_ = vec;
}

void RTransform::setFlipH()
{
    if(dirty_)
    {
        flip_.h = !flip_.h;
    }
    else
    {
        model_[0][0] *= -1;
        model_[0][1] *= -1;
        model_[0][2] *= -1;
        model_[0][3] *= -1;
    }
}

void RTransform::setFlipH(bool b)
{
    if(dirty_)
    {
        flip_.h = b;
    }
    else if(flip_.h != b)
    {
        model_[0][0] *= -1;
        model_[0][1] *= -1;
        model_[0][2] *= -1;
        model_[0][3] *= -1;
    }
}

void RTransform::setFlipV()
{
    if(dirty_)
    {
        flip_.v = !flip_.v;
    }
    else
    {
        model_[1][0] *= -1;
        model_[1][1] *= -1;
        model_[1][2] *= -1;
        model_[1][3] *= -1;
    }
}

void RTransform::setFlipV(bool b)
{
    if(dirty_)
        flip_.v = b;
    else if(flip_.v != b)
    {
        model_[1][0] *= -1;
        model_[1][1] *= -1;
        model_[1][2] *= -1;
        model_[1][3] *= -1;
    }
}

std::string RTransform::toString() const
{
    return "(" + std::to_string(rect_.pos().x()) + ", " + std::to_string(rect_.pos().y()) + ", " + std::to_string(depth_)
            + " | w:" + std::to_string(rect_.size().width()) + " h:" + std::to_string(rect_.size().height())
            + " | fh:" + std::to_string(flip_.h) + " fv:" + std::to_string(flip_.v) + ") ";
}

void RTransform::updateModel() const
{
    model_ = glm::translate(glm::mat4(1), { rect_.pos().x() + rect_.size().width()/2, rect_.pos().y() + rect_.size().height()/2, 0 });
    model_ *= glm::mat4_cast(glm::qua<float>(glm::vec3{ rotate_.x, rotate_.y, rotate_.z }));
    model_ = glm::scale(model_, { rect_.size().width(), rect_.size().height(), 0.0f });

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
