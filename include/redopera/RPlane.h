#ifndef RPLANE_H
#define RPLANE_H

#include <RTransform.h>
#include <RModelMat.h>
#include <RSprite.h>

#include <functional>
#include <variant>

namespace Redopera {

namespace RValue {

class RPlaneContainer
{
public:
    RPlaneContainer(const RTexture &tex, bool singleTex = false):
        singleTex_(singleTex),
        texFunc(&RPlaneContainer::getTexForTexture),
        texData_(tex)
    {}

    RPlaneContainer(const RSprite &sprite, bool singleTex = false):
        singleTex_(singleTex),
        texFunc(&RPlaneContainer::getTexForSprite),
        texData_(sprite)
    {}

    RPlaneContainer() = default;
    RPlaneContainer(const RPlaneContainer &plane) = default;
    RPlaneContainer& operator=(const RPlaneContainer &plane) = default;
    ~RPlaneContainer() = default;

    bool isSingleTex() const { return singleTex_; }
    const RTexture& texture() const { return (this->*texFunc)(); }
    const RSprite& sprite() const { return std::get<RSprite>(texData_); }

    RSprite& rSprite() { return std::get<RSprite>(texData_); }

    void setTexture(const RTexture& tex, bool singleTex = false)
    {
        texData_ = tex;
        texFunc = &RPlaneContainer::getTexForTexture;
        singleTex_ = singleTex;
    }

    void setTexture(const RSprite &sprite, bool singleTex = false)
    {
        texData_ = sprite;
        texFunc = &RPlaneContainer::getTexForSprite;
        singleTex_ = singleTex;
    }

private:
    const RTexture& getTexForTexture() const { return std::get<RTexture>(texData_); }
    const RTexture& getTexForSprite() const { return std::get<RSprite>(texData_).texture(); }

    bool singleTex_ = false;
    const RTexture& (RPlaneContainer::*texFunc)() const;
    std::variant<RTexture, RSprite> texData_;
};

}

class RPlane : public RValue::RPlaneContainer, public RModelMat
{
public:
    using RModelMat::RModelMat;
    RPlane() = default;

    RPlane(const RModelMat &model, const RTexture &tex, bool singleTex = false):
        RPlaneContainer(tex, singleTex),
        RModelMat(model)
    {}

    RPlane(const RModelMat &model, const RSprite &sprite, bool singleTex = false):
        RPlaneContainer(sprite, singleTex),
        RModelMat(model)
    {}
};

class RPlaneT : public RValue::RPlaneContainer, public RTransform
{
public:
    using RTransform::RTransform;
    RPlaneT() = default;

    RPlaneT(const RTransform &trans, const RTexture &tex, bool singleTex = false):
        RPlaneContainer(tex, singleTex),
        RTransform(trans)
    {}

    RPlaneT(const RTransform &trans, const RSprite &sprite, bool singleTex = false):
        RPlaneContainer(sprite, singleTex),
        RTransform(trans)
    {}
};

class RPlaneTF : public RValue::RPlaneContainer, public RTransformF
{
public:
    using RTransformF::RTransformF;
    RPlaneTF() = default;

    RPlaneTF(const RTransformF &trans, const RTexture &tex, bool singleTex = false):
        RPlaneContainer(tex, singleTex),
        RTransformF(trans)
    {}

    RPlaneTF(const RTransformF &trans, const RSprite &sprite, bool singleTex = false):
        RPlaneContainer(sprite, singleTex),
        RTransformF(trans)
    {}
};

} // ns Redopera

#endif // RPLANE_H
