#ifndef RPLANE_H
#define RPLANE_H

#include <RTransform.h>
#include <RTextsLoader.h>

#include <functional>

namespace Redopera {

class RSprite;

class RPlane : public RTransform
{
public:
    RPlane(const RTransform &transform);

    RPlane() = default;
    RPlane(const RPlane &plane) = default;
    RPlane& operator=(const RPlane &plane) = default;
    ~RPlane() = default;

    bool isSingleTex() const;
    const RTexture& texture() const;

    void setTexture(const std::function<const RTexture &()> &func, bool singleTex = false);
    void setTexture(const RTexture& tex, bool singleTex = false);
    void setTexture(const RSprite &sprite, bool singleTex = false);

private:
    bool singleTex_ = false;
    std::function<const RTexture&()> texFunc_;
};

}

#endif // RPLANE_H
