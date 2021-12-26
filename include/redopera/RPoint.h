#ifndef RPOINT_H
#define RPOINT_H

namespace Redopera {

namespace basic {

template<typename Value>
class RPoint2Basic
{
public:
    RPoint2Basic() noexcept: x_(0), y_(0) {}
    RPoint2Basic(Value x, Value y) noexcept: x_(x), y_(y) {}

    template<typename T>
    RPoint2Basic<T> convert() const noexcept { return RPoint2Basic<T>(x_, y_); }

    bool operator==(const RPoint2Basic<Value> &pos) const noexcept { return x_ == pos.x_ && y_ == pos.y_; }
    bool operator!=(const RPoint2Basic<Value> &pos) const noexcept { return x_ != pos.x_ || y_ != pos.y_; }

    RPoint2Basic<Value> operator+(const RPoint2Basic<Value> &pos) const noexcept { return RPoint2Basic<Value>(x_ + pos.x_, y_ + pos.y_); }
    RPoint2Basic<Value> operator-(const RPoint2Basic<Value> &pos) const noexcept { return RPoint2Basic<Value>(x_ - pos.x_, y_ - pos.y_); }

    RPoint2Basic<Value>& operator+=(const RPoint2Basic<Value> &pos) noexcept { x_ += pos.x_; y_ += pos.y_; return *this; }
    RPoint2Basic<Value>& operator-=(const RPoint2Basic<Value> &pos) noexcept { x_ -= pos.x_; y_ -= pos.y_; return *this; }

    template<typename T>
    RPoint2Basic<Value>& operator*=(T value) noexcept { x_ *= value; y_ *= value; return *this; }
    template<typename T>
    RPoint2Basic<Value>& operator/=(T value) noexcept { x_ /= value; y_ /= value; return *this; }

    template<typename T>
    RPoint2Basic<Value> operator*(T value) const noexcept { return RPoint2Basic<Value>(x_ * value, y_ * value); }
    template<typename T>
    RPoint2Basic<Value> operator/(T value) const noexcept { return RPoint2Basic<Value>(x_ / value, y_ / value); }

    Value x() const noexcept { return x_; }
    Value y() const noexcept { return y_; }
    Value& rx() noexcept { return x_; }
    Value& ry() noexcept { return y_; }
    bool isOrigin() const noexcept { return x_ == 0 && y_ == 0; }

    void setX(Value x) noexcept { x_ = x; }
    void setY(Value y) noexcept { y_ = y; }
    void setPos(Value x, Value y) noexcept { x_ = x; y_ = y; }
    void setPos(const RPoint2Basic<Value> &pos) noexcept { setPos(pos.x(), pos.y()); }

    void mirrorH(Value axis = 0) noexcept { x_ = 2 * axis - x_; }
    void mirrorV(Value axis = 0) noexcept { y_ = 2 * axis - y_; }
    void mirror(const RPoint2Basic<Value> &origin = RPoint2Basic<Value>()) noexcept { mirrorH(origin.x_); mirrorV(origin.y_); }

    void move(Value x, Value y) noexcept { x_ += x; y_ += y; }
    void move(const RPoint2Basic<Value> &pos) noexcept { x_ += pos.x_; y_ += pos.y_; }

protected:
    Value x_;
    Value y_;
}; // RPoint2Basic

extern template class basic::RPoint2Basic<int>;
extern template class basic::RPoint2Basic<float>;

template<typename Value>
class RPoint3Basic : public RPoint2Basic<Value>
{
public:
    RPoint3Basic() noexcept: RPoint2Basic<Value>(), z_(0) {}
    RPoint3Basic(Value x, Value y, Value z = 0) noexcept: RPoint2Basic<Value>(x, y), z_(z) {}
    RPoint3Basic(const RPoint2Basic<Value> &p2, Value z = 0) noexcept: RPoint2Basic<Value>(p2), z_(z) {}

    template<typename T>
    RPoint3Basic<T> convert() const noexcept { return RPoint3Basic<T>(RPoint2Basic<Value>::x(), RPoint2Basic<Value>::y(), z_); }

    bool operator==(const RPoint3Basic<Value> &pos) const noexcept
    { return RPoint2Basic<Value>::x_ == pos.x_ && RPoint2Basic<Value>::y_ == pos.y_ && z_ == pos.z_; }
    bool operator!=(const RPoint3Basic<Value> &pos) const noexcept
    { return RPoint2Basic<Value>::x_ != pos.x_ || RPoint2Basic<Value>::y_ != pos.y_ || z_ != pos.z_; }

    RPoint3Basic<Value> operator+(const RPoint3Basic<Value> &pos) const noexcept
    {
        return RPoint3Basic<Value>(RPoint2Basic<Value>::x_ + pos.x_, RPoint2Basic<Value>::y_ + pos.y_, z_ + pos.z_);
    }

    RPoint3Basic<Value> operator-(const RPoint3Basic<Value> &pos) const noexcept
    {
        return RPoint3Basic<Value>(RPoint2Basic<Value>::x_ - pos.x_, RPoint2Basic<Value>::y_ - pos.y_, z_ - pos.z_);
    }

    RPoint3Basic<Value>& operator+=(const RPoint3Basic<Value> &pos) noexcept
    { RPoint2Basic<Value>::x_ += pos.x_; RPoint2Basic<Value>::y_ += pos.y_; z_ += pos.z_; return *this; }
    RPoint3Basic<Value>& operator-=(const RPoint3Basic<Value> &pos) noexcept
    { RPoint2Basic<Value>::x_ -= pos.x_; RPoint2Basic<Value>::y_ -= pos.y_; z_ -= pos.z_; return *this; }

    Value z() const { return z_; }
    Value& rz() { return z_; }

    bool isOrigin() const { return RPoint2Basic<Value>::isOrigin() && z_ == 0; }

    void setZ(Value z) { z_ = z; }
    using RPoint2Basic<Value>::setPos;
    void setPos(Value x, Value y, Value z) { RPoint2Basic<Value>::x_ = x, RPoint2Basic<Value>::y_ = y; z_ = z; }
    void setPos(const RPoint2Basic<Value> &pos, Value z) { RPoint2Basic<Value>::x_ = pos.x(), RPoint2Basic<Value>::y_ = pos.y(); z_ = z; }

    void move(Value x, Value y, Value z = 0) { RPoint2Basic<Value>::move(x, y); z_ += z; }
    void move(const RPoint3Basic<Value> &pos) { RPoint2Basic<Value>::move(pos); z_ += pos.z_; };

protected:
    Value z_;
}; // Point3

extern template class basic::RPoint3Basic<int>;
extern template class basic::RPoint3Basic<float>;

} // ns basic

using RPoint2 = basic::RPoint2Basic<int>;
using RPoint2F = basic::RPoint2Basic<float>;

using RPoint = basic::RPoint3Basic<int>;
using RPointF = basic::RPoint3Basic<float>;

} // ns Redopera

#ifdef REDOPERA_DEFINE_FILE
template class Redopera::basic::RPoint2Basic<int>;
template class Redopera::basic::RPoint2Basic<float>;

template class Redopera::basic::RPoint3Basic<int>;
template class Redopera::basic::RPoint3Basic<float>;
#endif

#endif // RPOINT_H
