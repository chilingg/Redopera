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
    RPlaneContainer(const RTexture &tex):
        texFunc(&RPlaneContainer::getTexForTexture),
        texData_(tex)
    {}

    RPlaneContainer(const RSprite &sprite):
        texFunc(&RPlaneContainer::getTexForSprite),
        texData_(sprite)
    {}

    RPlaneContainer() = default;
    RPlaneContainer(const RPlaneContainer &plane) = default;
    RPlaneContainer& operator=(const RPlaneContainer &plane) = default;
    ~RPlaneContainer() = default;

    bool isSprite() const { return texFunc == &RPlaneContainer::getTexForSprite; }
    int channel() const
    {
        if(isSprite())
            return sprite().frame().format().inChannel;
        else
            return texture().format().inChannel;
    }

    const RTexture& texture() const { return (this->*texFunc)(); }
    const RSprite& sprite() const { return std::get<RSprite>(texData_); }

    RSprite& rSprite() { return std::get<RSprite>(texData_); }

    void setTexture(const RTexture& tex)
    {
        texData_ = tex;
        texFunc = &RPlaneContainer::getTexForTexture;
    }

    void setTexture(const RSprite &sprite)
    {
        texData_ = sprite;
        texFunc = &RPlaneContainer::getTexForSprite;
    }

private:
    const RTexture& getTexForTexture() const { return std::get<RTexture>(texData_); }
    const RTexture& getTexForSprite() const { return std::get<RSprite>(texData_).texture(); }

    const RTexture& (RPlaneContainer::*texFunc)() const;
    std::variant<RTexture, RSprite> texData_;
};

}

class RPlane : public RValue::RPlaneContainer, public RModelMat
{
public:
    using RModelMat::RModelMat;
    RPlane() = default;

    RPlane(const RModelMat &model, const RTexture &tex):
        RPlaneContainer(tex),
        RModelMat(model)
    {}

    RPlane(const RModelMat &model, const RSprite &sprite):
        RPlaneContainer(sprite),
        RModelMat(model)
    {}
};

class RPlaneT : public RValue::RPlaneContainer, public RTransform
{
public:
    using RTransform::RTransform;
    RPlaneT() = default;

    RPlaneT(const RTransform &trans, const RTexture &tex):
        RPlaneContainer(tex),
        RTransform(trans)
    {}

    RPlaneT(const RTransform &trans, const RSprite &sprite):
        RPlaneContainer(sprite),
        RTransform(trans)
    {}
};

class RPlaneTF : public RValue::RPlaneContainer, public RTransformF
{
public:
    using RTransformF::RTransformF;
    RPlaneTF() = default;

    RPlaneTF(const RTransformF &trans, const RTexture &tex):
        RPlaneContainer(tex),
        RTransformF(trans)
    {}

    RPlaneTF(const RTransformF &trans, const RSprite &sprite):
        RPlaneContainer(sprite),
        RTransformF(trans)
    {}
};

} // ns Redopera

#endif // RPLANE_H
