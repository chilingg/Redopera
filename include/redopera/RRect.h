#ifndef RRECT_H
#define RRECT_H

#include "RPoint.h"
#include "RSize.h"

namespace Redopera {

namespace RValue {

template<typename Value>
class RRectValue : public RPointValue2<Value>, public RSizeValue<Value>
{
public:
    RRectValue() noexcept = default;

    RRectValue(Value x, Value y, Value width, Value height) noexcept:
        RPointValue2<Value>(x, y),
        RSizeValue<Value>(width, height)
    {}

    RRectValue(const RPointValue2<Value> &pos, const RSizeValue<Value> &size) noexcept:
        RPointValue2<Value>(pos),
        RSizeValue<Value>(size)
    {}

    RRectValue(const RPointValue2<Value> &bottomLeft, const RPointValue2<Value> &topRight) noexcept:
        RPointValue2<Value>(bottomLeft),
        RSizeValue<Value>(topRight.x() - bottomLeft.x(), topRight.y() - bottomLeft.y())
    {}

    template<typename T>
    operator RRectValue<T>() const
    {
        return RRectValue<T>(pos(), size());
    }

    bool operator==(const RRectValue &rect) const
    {
        return RPointValue2<Value>::operator==(rect) & RSizeValue<Value>::operator==(rect);
    }

    bool operator!=(const RRectValue &rect) const
    {
        return RPointValue2<Value>::operator!=(rect) & RSizeValue<Value>::operator!=(rect);
    }

    RRectValue operator+(const RPointValue2<Value> &pos) const
    {
        return RRectValue(RPointValue2<Value>::operator+(pos), size());
    }

    RRectValue operator-(const RPointValue2<Value> &pos) const
    {
        return RRectValue(RPointValue2<Value>::operator-(pos), size());
    }


    RRectValue& operator+=(const RPointValue2<Value> &pos)
    {
        RPointValue2<Value>::operator+=(pos);
        return *this;
    }

    RRectValue operator-=(const RPointValue2<Value> &pos)
    {
        RPointValue2<Value>::operator-=(pos);
        return *this;
    }

    void setBottomLeft(const RPointValue2<Value> &pos)
    {
        RPointValue2<Value>::setPos(pos.x(), pos.y());
    }

    void setBottomLeft(Value x, Value y)
    {
        RPointValue2<Value>::setPos(x, y);
    }

    void setTopRight(const RPointValue2<Value> &pos)
    {
        RPointValue2<Value>::setPos(pos.x() - RSizeValue<Value>::width(), pos.y() - RSizeValue<Value>::height());
    }

    void setTopRIght(Value x, Value y)
    {
        RPointValue2<Value>::setPos(x - RSizeValue<Value>::width(), y - RSizeValue<Value>::height());
    }

    void setBottom(Value bottom)
    {
        RPointValue2<Value>::setY(bottom);
    }

    void setLeft(Value left)
    {
        RPointValue2<Value>::setX(left);
    }

    void setTop(Value top)
    {
        RPointValue2<Value>::setY(top - RSizeValue<Value>::height());
    }

    void setRight(Value right)
    {
        RPointValue2<Value>::setX(right - RPointValue2<Value>::x());
    }

    void setCenter(const RPointValue2<Value> &pos)
    {
        RPointValue2<Value>::setPos(pos.x() - RSizeValue<Value>::width()/2, pos.y() - RSizeValue<Value>::height()/2);
    }

    void setCenter(int x, int y)
    {
        RPointValue2<Value>::setPos(x - RSizeValue<Value>::width()/2, y - RSizeValue<Value>::height()/2);
    }

    void setCenterX(int x)
    {
        RPointValue2<Value>::setX(x - RSizeValue<Value>::width()/2);
    }

    void setCenterY(int y)
    {
        RPointValue2<Value>::setY(y - RSizeValue<Value>::height()/2);
    }

    void setRect(int x, int y, int width, int height)
    {
        RSizeValue<Value>::setSize(width, height);
        RPointValue2<Value>::setPos(x, y);
    }

    void setRect(const RPointValue2<Value> &pos, const RSize &size)
    {
        RSizeValue<Value>::setSize(size.width(), size.height());
        RPointValue2<Value>::setPos(pos.x(), pos.y());
    }

    Value top() const { return RPointValue2<Value>::y() + RSizeValue<Value>::height(); }

    Value bottom() const { return RPointValue2<Value>::y(); }

    Value left() const { return RPointValue2<Value>::x(); }

    Value right() const { return RPointValue2<Value>::x() + RSizeValue<Value>::width(); }

    const RPointValue2<Value>& pos() const { return *this; }

    RPointValue2<Value> bottomLeft() const { return *this; }

    RPointValue2<Value> bottomRight() const { return RPointValue2<Value>(RPointValue2<Value>::x() + RSizeValue<Value>::width(), RPointValue2<Value>::y()); }

    RPointValue2<Value> topLeft() const { return RPointValue2<Value>(RPointValue2<Value>::x(), RPointValue2<Value>::y() + RSizeValue<Value>::height()); }

    RPointValue2<Value> topRIght() const
    {
        return RPointValue2<Value>(RPointValue2<Value>::x() + RSizeValue<Value>::width(), RPointValue2<Value>::y() + RSizeValue<Value>::height());
    }

    RPointValue2<Value> center() const
    {
        return RPointValue2<Value>(RPointValue2<Value>::x() + RSizeValue<Value>::width()/2, RPointValue2<Value>::y() + RSizeValue<Value>::height()/2);
    }

    Value centerX() const { return RPointValue2<Value>::x() + RSizeValue<Value>::width()/2; }

    Value centerY() const { return RPointValue2<Value>::y() + RSizeValue<Value>::height()/2; }

    const RSizeValue<Value>& size() const { return *this; }

    bool isOrigin() const { return center().isOrigin(); };

    bool contains(const RRectValue<Value> &rect) const
    {
        return rect.x() >= RPointValue2<Value>::x() && rect.y() >= RPointValue2<Value>::y() && rect.top() <= top() && rect.right() <= right();
    }

    bool contains(const RPointValue2<Value> &pos) const
    {
        return pos.x() >= RPointValue2<Value>::x() && pos.y() >= RPointValue2<Value>::y() && pos.x() <= right() && pos.y() <= top();
    }

    bool contains(int x, int y) const
    {
        return x >= RPointValue2<Value>::x() && y >= RPointValue2<Value>::y() && x <= right() && y <= top();
    }


    bool overlap(const RRectValue &rect) const
    {
        return rect.bottom() < top() && rect.top() > bottom() && rect.left() < right() && rect.right() > left();
    }

    void mirrorH(Value axis = 0) { setCenterX(axis * 2 - centerX()); }

    void mirrorV(Value axis = 0) { setCenterY(axis * 2 - centerY()); }

    void mirrorV(RPointValue2<Value> origin = RPointValue2<Value>()) { setCenter(origin * 2 - center()); }

    std::string toString() const
    {
        using std::to_string;

        return "(" + to_string(RPointValue2<Value>::x()) + ", " + to_string(RPointValue2<Value>::y())
                + " | w:" + to_string(RSizeValue<Value>::width()) + " h:" + to_string(RSizeValue<Value>::height()) + ") ";
    }
};

extern template class RRectValue<int>;
extern template class RRectValue<float>;

} // ns RValue

using RRect = RValue::RRectValue<int>;
using RRectF = RValue::RRectValue<float>;

} // ns Redopera

#endif // RRECT_H
