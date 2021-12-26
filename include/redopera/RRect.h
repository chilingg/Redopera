#ifndef RRECT_H
#define RRECT_H

#include "RPoint.h"
#include "RSize.h"

namespace Redopera {

namespace basic {

template<typename Value>
class RRectBasic : public RPoint2Basic<Value>, public RSizeBasic<Value>
{
public:
    RRectBasic() noexcept = default;

    RRectBasic(Value x, Value y, Value width, Value height) noexcept:
        RPoint2Basic<Value>(x, y),
        RSizeBasic<Value>(width, height)
    {}

    RRectBasic(const RPoint2Basic<Value> &pos, const RSizeBasic<Value> &size) noexcept:
        RPoint2Basic<Value>(pos),
        RSizeBasic<Value>(size)
    {}

    RRectBasic(const RPoint2Basic<Value> &bottomLeft, const RPoint2Basic<Value> &topRight) noexcept:
        RPoint2Basic<Value>(bottomLeft),
        RSizeBasic<Value>(topRight.x() - bottomLeft.x(), topRight.y() - bottomLeft.y())
    {}

    template<typename T>
    RRectBasic<T> convert() const
    {
        return RRectBasic<T>(pos(), size());
    }

    bool operator==(const RRectBasic &rect) const
    {
        return RPoint2Basic<Value>::operator==(rect) & RSizeBasic<Value>::operator==(rect);
    }

    bool operator!=(const RRectBasic &rect) const
    {
        return RPoint2Basic<Value>::operator!=(rect) & RSizeBasic<Value>::operator!=(rect);
    }

    RRectBasic operator+(const RPoint2Basic<Value> &pos) const
    {
        return RRectBasic(RPoint2Basic<Value>::operator+(pos), size());
    }

    RRectBasic operator-(const RPoint2Basic<Value> &pos) const
    {
        return RRectBasic(RPoint2Basic<Value>::operator-(pos), size());
    }


    RRectBasic& operator+=(const RPoint2Basic<Value> &pos)
    {
        RPoint2Basic<Value>::operator+=(pos);
        return *this;
    }

    RRectBasic operator-=(const RPoint2Basic<Value> &pos)
    {
        RPoint2Basic<Value>::operator-=(pos);
        return *this;
    }

    void setBottomLeft(const RPoint2Basic<Value> &pos)
    {
        RPoint2Basic<Value>::setPos(pos.x(), pos.y());
    }

    void setBottomLeft(Value x, Value y)
    {
        RPoint2Basic<Value>::setPos(x, y);
    }

    void setTopRight(const RPoint2Basic<Value> &pos)
    {
        RPoint2Basic<Value>::setPos(pos.x() - RSizeBasic<Value>::width(), pos.y() - RSizeBasic<Value>::height());
    }

    void setTopRIght(Value x, Value y)
    {
        RPoint2Basic<Value>::setPos(x - RSizeBasic<Value>::width(), y - RSizeBasic<Value>::height());
    }

    void setBottom(Value bottom)
    {
        RPoint2Basic<Value>::setY(bottom);
    }

    void setLeft(Value left)
    {
        RPoint2Basic<Value>::setX(left);
    }

    void setTop(Value top)
    {
        RPoint2Basic<Value>::setY(top - RSizeBasic<Value>::height());
    }

    void setRight(Value right)
    {
        RPoint2Basic<Value>::setX(right - RPoint2Basic<Value>::x());
    }

    void setCenter(const RPoint2Basic<Value> &pos)
    {
        RPoint2Basic<Value>::setPos(pos.x() - RSizeBasic<Value>::width()/2, pos.y() - RSizeBasic<Value>::height()/2);
    }

    void setCenter(int x, int y)
    {
        RPoint2Basic<Value>::setPos(x - RSizeBasic<Value>::width()/2, y - RSizeBasic<Value>::height()/2);
    }

    void setCenterX(int x)
    {
        RPoint2Basic<Value>::setX(x - RSizeBasic<Value>::width()/2);
    }

    void setCenterY(int y)
    {
        RPoint2Basic<Value>::setY(y - RSizeBasic<Value>::height()/2);
    }

    void setRect(int x, int y, int width, int height)
    {
        RSizeBasic<Value>::setSize(width, height);
        RPoint2Basic<Value>::setPos(x, y);
    }

    void setRect(const RPoint2Basic<Value> &pos, const RSize &size)
    {
        RSizeBasic<Value>::setSize(size.width(), size.height());
        RPoint2Basic<Value>::setPos(pos.x(), pos.y());
    }

    RPoint2Basic<Value>& rPos() { return *this; }

    RSizeBasic<Value>& rSize() { return *this; }

    Value top() const { return RPoint2Basic<Value>::y() + RSizeBasic<Value>::height(); }

    Value bottom() const { return RPoint2Basic<Value>::y(); }

    Value left() const { return RPoint2Basic<Value>::x(); }

    Value right() const { return RPoint2Basic<Value>::x() + RSizeBasic<Value>::width(); }

    const RPoint2Basic<Value>& pos() const { return *this; }

    RPoint2Basic<Value> bottomLeft() const { return *this; }

    RPoint2Basic<Value> bottomRight() const { return RPoint2Basic<Value>(RPoint2Basic<Value>::x() + RSizeBasic<Value>::width(), RPoint2Basic<Value>::y()); }

    RPoint2Basic<Value> topLeft() const { return RPoint2Basic<Value>(RPoint2Basic<Value>::x(), RPoint2Basic<Value>::y() + RSizeBasic<Value>::height()); }

    RPoint2Basic<Value> topRIght() const
    {
        return RPoint2Basic<Value>(RPoint2Basic<Value>::x() + RSizeBasic<Value>::width(), RPoint2Basic<Value>::y() + RSizeBasic<Value>::height());
    }

    RPoint2Basic<Value> center() const
    {
        return RPoint2Basic<Value>(RPoint2Basic<Value>::x() + RSizeBasic<Value>::width()/2, RPoint2Basic<Value>::y() + RSizeBasic<Value>::height()/2);
    }

    Value centerX() const { return RPoint2Basic<Value>::x() + RSizeBasic<Value>::width()/2; }

    Value centerY() const { return RPoint2Basic<Value>::y() + RSizeBasic<Value>::height()/2; }

    const RSizeBasic<Value>& size() const { return *this; }

    bool isOrigin() const { return center().isOrigin(); };

    bool contains(const RRectBasic<Value> &rect) const
    {
        return rect.x() >= RPoint2Basic<Value>::x() && rect.y() >= RPoint2Basic<Value>::y() && rect.top() <= top() && rect.right() <= right();
    }

    bool contains(const RPoint2Basic<Value> &pos) const
    {
        return pos.x() >= RPoint2Basic<Value>::x() && pos.y() >= RPoint2Basic<Value>::y() && pos.x() <= right() && pos.y() <= top();
    }

    bool contains(int x, int y) const
    {
        return x >= RPoint2Basic<Value>::x() && y >= RPoint2Basic<Value>::y() && x <= right() && y <= top();
    }


    bool overlap(const RRectBasic &rect) const
    {
        return rect.bottom() < top() && rect.top() > bottom() && rect.left() < right() && rect.right() > left();
    }

    void mirrorH(Value axis = 0) { setCenterX(axis * 2 - centerX()); }

    void mirrorV(Value axis = 0) { setCenterY(axis * 2 - centerY()); }

    void mirrorV(RPoint2Basic<Value> origin = RPoint2Basic<Value>()) { setCenter(origin * 2 - center()); }
};

extern template class RRectBasic<int>;
extern template class RRectBasic<float>;

} // ns basic

using RRect = basic::RRectBasic<int>;
using RRectF = basic::RRectBasic<float>;

} // ns Redopera

#ifdef REDOPERA_DEFINE_FILE
template class Redopera::basic::RRectBasic<int>;
template class Redopera::basic::RRectBasic<float>;
#endif

#endif // RRECT_H
