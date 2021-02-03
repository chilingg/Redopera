#ifndef RPLANE_H
#define RPLANE_H

#include <RTransform.h>
#include <RTextsLoader.h>
#include <RSprite.h>

#include <functional>

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
    const RTexture& texture() const { return texFunc_(); }

    void setTexture(const std::function<const RTexture &()> &func, bool singleTex = false)
    {
        texFunc_ = func;
        singleTex_ = singleTex;
    }

    void setTexture(const RTexture& tex, bool singleTex = false)
    {
        texFunc_ = [tex]()->const RTexture& { return tex; };
        singleTex_ = singleTex;
    }

    void setTexture(const RSprite &sprite, bool singleTex = false)
    {
        texFunc_ = [sprite](){ return sprite.texture(); };
        singleTex_ = singleTex;
    }

private:
    bool singleTex_ = false;
    std::function<const RTexture&()> texFunc_;
};

extern template class RPlaneValue<int>;
extern template class RPlaneValue<float>;

} // ns RValue

using RPlane = RValue::RPlaneValue<int>;
using RPlaneF = RValue::RPlaneValue<float>;

} // ns Redopera

#endif // RPLANE_H
