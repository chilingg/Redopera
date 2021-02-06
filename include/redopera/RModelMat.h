#ifndef RMODELMAT_H
#define RMODELMAT_H

#include <RRect.h>
#include <RMath.h>

namespace Redopera {

class RModelMat
{
public:
    RModelMat();

    RModelMat(float x, float y, float z, float width, float height);
    RModelMat(const RPointF &pos, const RSizeF &size);
    RModelMat(const RRectF &rect, float depth = 0);

    const glm::mat4& model() const;

    float top() const;
    float bottom() const;
    float left() const;
    float right() const;
    float depth() const;
    float x() const;
    float y() const;
    float centerX() const;
    float centerY() const;
    RPoint2F center() const;
    RPointF pos() const;

    float width() const;
    float height() const;
    RSizeF size() const;

    RRectF rect() const;

    bool isFlipH() const;
    bool isFlipV() const;

    void move(float x, float y, float z = 0);
    void move(const RPoint2F &pos, float depth = 0);

    void setPos(float x, float y);
    void setPos(float x, float y, float z);
    void setPos(const RPoint2F &pos);
    void setPos(const RPoint2F &pos, float depth);
    void setDepth(float depth);
    void setCenterX(float x);
    void setCenterY(float y);
    void setCenter(float x, float y);
    void setCenter(const RPoint2F &pos);
    void setLeft(float left);
    void setRight(float right);
    void setTop(float top);
    void setBottom(float bottom);

    void setWidth(float width);
    void setHeight(float height);
    void setSize(float width, float height);
    void setSize(const RSizeF &size);

    void flipH();
    void flipV();

    void setModel(float x, float y, float z, float width, float height);
    void setModel(const RPoint2F &pos, const RSizeF &size);
    void setModel(const RPoint2F &pos, const RSizeF &size, float depth);
    void setModel(const RRectF &rect);
    void setModel(const RRectF &rect, float depth);
    void setModel(const glm::mat4 &model);

    std::string toString() const;

private:
    glm::mat4 model_;
};

} // ns Redopera

#endif // RMODELMAT_H
