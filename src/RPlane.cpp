#include <RPlane.h>
#include <RSprite.h>

using namespace Redopera;

RPlane::RPlane(const RTransform &transform):
    RTransform(transform)
{

}

bool RPlane::isSingleTex() const
{
    return singleTex_;
}

const RTexture &RPlane::texture() const
{
    return texFunc_();
}

void RPlane::setTexture(const std::function<const RTexture &()> &func, bool singleTex)
{
    texFunc_ = func;
    singleTex_ = singleTex;
}

void RPlane::setTexture(const RTexture &tex, bool singleTex)
{
    texFunc_ = [tex]()->const RTexture& { return tex; };
    singleTex_ = singleTex;
}

void RPlane::setTexture(const RSprite &sprite, bool singleTex)
{
    texFunc_ = [sprite](){ return sprite.texture(); };
    singleTex_ = singleTex;
}
