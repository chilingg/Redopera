#ifndef RPOINT_H
#define RPOINT_H

#include <string>

namespace Redopera {

namespace RValue {

template<typename Value>
class RPointValue2
{
public:
    RPointValue2() noexcept: x_(0), y_(0) {}
    RPointValue2(Value x, Value y) noexcept: x_(x), y_(y) {}

    template<typename T>
    operator RPointValue2<T>() const { return RPointValue2<T>(x_, y_); }

    bool operator==(const RPointValue2<Value> &pos) const { return x_ == pos.x_ && y_ == pos.y_; }
    bool operator!=(const RPointValue2<Value> &pos) const { return x_ != pos.x_ || y_ != pos.y_; }

    RPointValue2<Value> operator+(const RPointValue2<Value> &pos) const { return RPointValue2<Value>(x_ + pos.x_, y_ + pos.y_); }
    RPointValue2<Value> operator-(const RPointValue2<Value> &pos) const { return RPointValue2<Value>(x_ - pos.x_, y_ - pos.y_); }

    RPointValue2<Value>& operator+=(const RPointValue2<Value> &pos) { x_ += pos.x_; y_ += pos.y_; return *this; }
    RPointValue2<Value>& operator-=(const RPointValue2<Value> &pos) { x_ -= pos.x_; y_ -= pos.y_; return *this; }

    template<typename T>
    RPointValue2<Value>& operator*=(T value) { x_ *= value; y_ *= value; return *this; }
    template<typename T>
    RPointValue2<Value>& operator/=(T value) { x_ /= value; y_ /= value; return *this; }

    template<typename T>
    RPointValue2<Value> operator*(T value) const { return RPointValue2<Value>(x_ * value, y_ * value); }
    template<typename T>
    RPointValue2<Value> operator/(T value) const { return RPointValue2<Value>(x_ / value, y_ / value); }

    Value x() const { return x_; }
    Value y() const { return y_; }
    Value& rx() { return x_; }
    Value& ry() { return y_; }
    bool isOrigin() const { return x_ == 0 && y_ == 0; }

    void setX(Value x) { x_ = x; }
    void setY(Value y) { y_ = y; }
    void setPos(Value x, Value y) { x_ = x; y_ = y; }
    void setPos(const RPointValue2<Value> &pos) { setPos(pos.x(), pos.y()); }

    void mirrorH(Value axis = 0) { x_ = 2 * axis - x_; }
    void mirrorV(Value axis = 0) { y_ = 2 * axis - y_; }
    void mirror(const RPointValue2<Value> &origin = RPointValue2<Value>()) { mirrorH(origin.x_); mirrorV(origin.y_); }

    void move(Value x, Value y) { x_ += x; y_ += y; }
    void move(const RPointValue2<Value> &pos) { x_ += pos.x_; y_ += pos.y_; }

    std::string toString() const
    {
        return '(' + std::to_string(x_) + ", " + std::to_string(y_) + ") ";
    }

protected:
    Value x_;
    Value y_;

}; // Point2

extern template class RValue::RPointValue2<int>;
extern template class RValue::RPointValue2<float>;

template<typename Value>
class RPointValue3 : public RPointValue2<Value>
{
public:
    RPointValue3() noexcept: RPointValue2<Value>(), z_(0) {}
    RPointValue3(Value x, Value y, Value z = 0) noexcept: RPointValue2<Value>(x, y), z_(z) {}
    RPointValue3(const RPointValue2<Value> &p2, Value z = 0) noexcept: RPointValue2<Value>(p2), z_(z) {}

    template<typename T>
    operator RPointValue3<T>() const { return RPointValue3<T>(RPointValue2<Value>::x(), RPointValue2<Value>::y(), z_); }

    bool operator==(const RPointValue3<Value> &pos) const { return RPointValue2<Value>::x_ == pos.x_ && RPointValue2<Value>::y_ == pos.y_ && z_ == pos.z_; }
    bool operator!=(const RPointValue3<Value> &pos) const { return RPointValue2<Value>::x_ != pos.x_ || RPointValue2<Value>::y_ != pos.y_ || z_ != pos.z_; }

    RPointValue3<Value> operator+(const RPointValue3<Value> &pos) const
    {
        return RPointValue3<Value>(RPointValue2<Value>::x_ + pos.x_, RPointValue2<Value>::y_ + pos.y_, z_ + pos.z_);
    }

    RPointValue3<Value> operator-(const RPointValue3<Value> &pos) const
    {
        return RPointValue3<Value>(RPointValue2<Value>::x_ - pos.x_, RPointValue2<Value>::y_ - pos.y_, z_ - pos.z_);
    }


    RPointValue3<Value>& operator+=(const RPointValue3<Value> &pos) { RPointValue2<Value>::x_ += pos.x_; RPointValue2<Value>::y_ += pos.y_; z_ += pos.z_; return *this; }
    RPointValue3<Value>& operator-=(const RPointValue3<Value> &pos) { RPointValue2<Value>::x_ -= pos.x_; RPointValue2<Value>::y_ -= pos.y_; z_ -= pos.z_; return *this; }

    Value z() const { return z_; }
    Value& rz() { return z_; }

    bool isOrigin() const { return RPointValue2<Value>::isOrigin() && z_ == 0; }

    void setZ(Value z) { z_ = z; }
    void setPos(Value x, Value y, Value z) { RPointValue2<Value>::x_ = x, RPointValue2<Value>::y_ = y; z_ = z; }
    void setPos(const RPointValue2<Value> &pos, Value z) { RPointValue2<Value>::x_ = pos.x(), RPointValue2<Value>::y_ = pos.y(); z_ = z; }

    void move(Value x, Value y, Value z = 0) { RPointValue2<Value>::move(x, y); z_ += z; }
    void move(const RPointValue3<Value> &pos) { RPointValue2<Value>::move(pos); z_ += pos.z_; };

    std::string toString() const
    {
        return '(' + std::to_string(RPointValue2<Value>::x_) + ", " + std::to_string(RPointValue2<Value>::y_) + ", " + std::to_string(z_) + ") ";
    }

protected:
    Value z_;

}; // Point3

extern template class RValue::RPointValue3<int>;
extern template class RValue::RPointValue3<float>;

} // ns RValue

using RPoint2 = RValue::RPointValue2<int>;
using RPoint2F = RValue::RPointValue2<float>;

using RPoint = RValue::RPointValue3<int>;
using RPointF = RValue::RPointValue3<float>;

} // ns Redopera

#endif // RPOINT_H
