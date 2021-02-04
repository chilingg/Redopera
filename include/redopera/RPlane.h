#ifndef RPLANE_H
#define RPLANE_H

#include <RTransform.h>
#include <RTextsLoader.h>
#include <RSprite.h>

#include <functional>
#include <variant>

namespace Redopera {

namespace RValue {

template<typename Value>
class RPlaneValue : public RTransformValue<Value>
{
public:
    RPlaneValue(const RTransformValue<Value> &transform):
        RTransformValue<Value>(transform)
    {}

    RPlaneValue() = default;
    RPlaneValue(const RPlaneValue &plane) = default;
    RPlaneValue& operator=(const RPlaneValue &plane) = default;
    ~RPlaneValue() = default;

    bool isSingleTex() const { return singleTex_; }
    const RTexture& texture() const { return (this->*texFunc)(); }
    const RSprite& sprite() const { return std::get<RSprite>(texData_); }

    RSprite& rSprite() { return std::get<RSprite>(texData_); }

    void setTexture(const RTexture& tex, bool singleTex = false)
    {
        texData_ = tex;
        texFunc = &RPlaneValue<Value>::getTexForTexture;
        singleTex_ = singleTex;
    }

    void setTexture(const RSprite &sprite, bool singleTex = false)
    {
        texData_ = sprite;
        texFunc = &RPlaneValue<Value>::getTexForSprite;
        singleTex_ = singleTex;
    }

private:
    const RTexture& getTexForTexture() const { return std::get<RTexture>(texData_); }
    const RTexture& getTexForSprite() const { return std::get<RSprite>(texData_).texture(); }

    const RTexture& (RPlaneValue<Value>::*texFunc)() const;

    bool singleTex_ = false;
    std::variant<RTexture, RSprite> texData_;
};

extern template class RPlaneValue<int>;
extern template class RPlaneValue<float>;

} // ns RValue

using RPlane = RValue::RPlaneValue<int>;
using RPlaneF = RValue::RPlaneValue<float>;

} // ns Redopera

#endif // RPLANE_H
