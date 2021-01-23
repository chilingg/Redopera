#ifndef RTRANSFORM_H
#define RTRANSFORM_H

#include "RMath.h"
#include "RRect.h"

namespace Redopera {

class RTransform
{
public:
    RTransform();
    RTransform(int x, int y, int depth, int width, int height);
    RTransform(const RPoint &pos, const RSize &size);
    RTransform(const RRect &rect, int depth = 0);

    RTransform(const RTransform &trans) = default;
    RTransform& operator=(const RTransform &trans) = default;
    ~RTransform() = default;

    bool isDirty() const;
    const glm::mat4& model() const;
    const RPoint2& pos() const;
    int depth() const;
    const RSize& size() const;
    const RRect& rect() const;
    const glm::vec3& rotate() const;
    bool isFlipH() const;
    bool isFlipV() const;

    RRect& rRect();

    void setRect(const RRect &rect);
    void setDepth(int depth);
    void setSize(const RSize &size);
    void setSize(int width, int height);
    void setPos(const RPoint &pos);
    void setPos(int x, int y);
    void setRotate(float x, float y, float z);
    void setRotate(const glm::vec3& vec);

    void setFlipH();
    void setFlipH(bool b);
    void setFlipV();
    void setFlipV(bool b);

    std::string toString() const;

private:
    void updateModel() const;

    mutable bool dirty_ = true;
    struct {
        bool h, v;
    } flip_{ false, false };
    int depth_;
    RRect rect_;
    glm::vec3 rotate_;
    mutable glm::mat4 model_;
};

}

#endif // TRANSFORM_H
