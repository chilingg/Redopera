#ifndef RTRANSFORM_H
#define RTRANSFORM_H

#include "RMath.h"
#include "RRect.h"

namespace Redopera {

namespace RValue {

template<typename Value>
class RTransformValue
{
public:
    RTransformValue() = default;

    RTransformValue(Value x, Value y, Value depth, Value width, Value height):
        depth_(depth),
        rect_(x, y, width, height)
    {}

    RTransformValue(const RPointValue3<Value> &pos, const RSizeValue<Value> &size):
        depth_(pos.z()),
        rect_(pos, size)
    {}

    RTransformValue(const RRectValue<Value> &rect, Value depth = 0):
        depth_(depth),
        rect_(rect)
    {}

    RTransformValue(const RTransformValue<Value> &trans) = default;
    RTransformValue& operator=(const RTransformValue<Value> &trans) = default;
    ~RTransformValue() = default;

    bool isDirty() const { return dirty_; }
    Value depth() const { return depth_; }
    const RRectValue<Value>& rect() const { return rect_; }
    const RPointValue2<Value>& pos() const { return rect_.pos(); }
    const RSizeValue<Value>& size() const { return rect_.size(); }
    const glm::vec3& rotate() const { return rotate_; }
    bool isFlipH() const { return flip_.h; }
    bool isFlipV() const { return flip_.v; }

    const glm::mat4& model() const
    {
        if(dirty_)
            updateModel();
        return model_;
    }

    RRectValue<Value>& rRect() { dirty_ = true; return rect_; }

    void setRect(const RRectValue<Value> &rect) { dirty_ = true; rect_ = rect; }
    void setSize(Value width, Value height) { dirty_ = true; rect_.setSize(width, height); }
    void setSize(const RSizeValue<Value> &size) { dirty_ = true; rect_.setSize(size); }
    void setPos(Value x, Value y) { dirty_ = true; rect_.setPos(x, y); }
    void setPos(const RPointValue2<Value> &pos) { dirty_ = true; rect_.setPos(pos); }

    void setDepth(Value depth) { dirty_ = true; depth_ = depth; }
    void setRotate(float x, float y, float z) { dirty_ = true; rotate_ = {x, y, z}; }
    void setRotate(const glm::vec3& vec) { dirty_ = true; rotate_ = vec; }

    void setFlipH()
    {
        if(dirty_)
            flip_.h = !flip_.h;
        else
        {
            model_[0][0] *= -1;
            model_[0][1] *= -1;
            model_[0][2] *= -1;
            model_[0][3] *= -1;
        }
    }

    void setFlipH(bool b)
    {
        if(dirty_)
            flip_.h = b;
        else if(flip_.h != b)
        {
            model_[0][0] *= -1;
            model_[0][1] *= -1;
            model_[0][2] *= -1;
            model_[0][3] *= -1;
        }
    }

    void setFlipV()
    {
        if(dirty_)
            flip_.v = !flip_.v;
        else
        {
            model_[1][0] *= -1;
            model_[1][1] *= -1;
            model_[1][2] *= -1;
            model_[1][3] *= -1;
        }
    }

    void setFlipV(bool b)
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

    std::string toString() const
    {
        using std::to_string;

        return "(" + to_string(rect_.x()) + ", " + to_string(rect_.y()) + ", " + to_string(depth_)
                + " | w:" + to_string(rect_.width()) + " h:" + to_string(rect_.height())
                + " | fh:" + to_string(flip_.h) + " fv:" + to_string(flip_.v) + ") ";
    }

private:
    void updateModel() const
    {
        model_ = glm::translate(glm::mat4(1), { rect_.x() + rect_.width()/2, rect_.y() + rect_.height()/2, depth_ });
        model_ *= glm::mat4_cast(glm::qua<float>(glm::vec3{ rotate_.x, rotate_.y, rotate_.z }));
        model_ = glm::scale(model_, { rect_.width(), rect_.height(), 0.0f });

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

    mutable bool dirty_ = true;
    struct {
        bool h, v;
    } flip_{ false, false };
    Value depth_ = 0;
    RRectValue<Value> rect_;
    glm::vec3 rotate_ = { 0.f, 0.f, 0.f };
    mutable glm::mat4 model_;
};

extern template class RTransformValue<int>;
extern template class RTransformValue<float>;

} // ns RValue

using RTransform = RValue::RTransformValue<int>;
using RTransformF = RValue::RTransformValue<float>;

} // ns Redopera

#endif // TRANSFORM_H
