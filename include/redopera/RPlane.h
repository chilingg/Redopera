#ifndef RPLANE_H
#define RPLANE_H

#include "RMath.h"
#include "RPoint.h"
#include "RSize.h"
#include "rsc/RTexture.h"

namespace Redopera {

class RPlane
{
public:
    RPlane();
    RPlane(int width, int height, int x, int y, int z = 0, const RTexture &tex = RTexture());
    RPlane(int width, int height, const RPoint &pos, const RTexture &tex = RTexture());
    RPlane(const RSize &size, const RPoint &pos, const RTexture &tex = RTexture());
    explicit RPlane(const RRect &rect, int z = 0, const RTexture &tex = RTexture());
    RPlane(const RPlane &plane);
    RPlane(const RPlane &&plane);
    RPlane& operator=(const RPlane &plane);
    RPlane& operator=(const RPlane &&plane);
    ~RPlane() = default;

    const glm::mat4& model();
    bool isDirty() const;
    const RPoint& pos() const;
    const RSize& size() const;
    RRect rect() const;
    const RTexture& texture() const;
    const float* rotate() const;

    RPoint& rPos();
    RSize& rSize();

    void setRotate(float x, float y, float z);
    void setRotate(float *xyz);
    void setTexture(const RTexture &tex);

    void flipH();
    void flipV();

    void updataMat();

private:
    bool dirty_ = true;
    struct {
        bool h, v;
    } flip_ { false, false };
    struct {
        float x, y, z;
    } rotate_ { 0.f, 0.f, 0.f };

    RSize size_;
    RPoint pos_;
    glm::mat4 model_;
    RTexture texture_;
};

} // Redopera

#endif // RPLANE_H
