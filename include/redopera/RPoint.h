#ifndef RPOINT_H
#define RPOINT_H

#include <functional>

namespace Redopera {

class RPoint2
{
    friend std::hash<RPoint2>;

public:
    constexpr static int INVALID_POINT_VALUE = ~0u >> 1;

    explicit RPoint2(int x, int y = 0): x_(x), y_(y) {}

    RPoint2(): x_(INVALID_POINT_VALUE), y_(INVALID_POINT_VALUE) {}

    bool operator==(const RPoint2 &pos) const { return x_ == pos.x_ && y_ == pos.y_; }
    bool operator!=(const RPoint2 &pos) const { return x_ != pos.x_ || y_ != pos.y_; }

    RPoint2 operator+(const RPoint2 &pos) { return RPoint2(x_ += pos.x_, y_ += pos.y_); }
    RPoint2 operator-(const RPoint2 &pos) { return RPoint2(x_ -= pos.x_, y_ -= pos.y_); }

    RPoint2& operator+=(const RPoint2 &pos) { x_ += pos.x_; y_ += pos.y_; return *this; }
    RPoint2& operator-=(const RPoint2 &pos) { x_ -= pos.x_; y_ -= pos.y_; return *this; }

    int x() const { return x_; }
    int y() const { return y_; }
    int& rx() { return x_; }
    int& ry() { return y_; }
    bool isValid() const { return x_ != INVALID_POINT_VALUE && y_ != INVALID_POINT_VALUE; }
    bool isOrigin() const { return x_ == 0 && y_ == 0; }

    void setX(int x) { x_ = x; }
    void setY(int y) { y_ = y; }
    void set(int x, int y) { x_ = x; y_ = y; }

    RPoint2 mirrorH(int axis = 0) { return RPoint2((axis-x_)*2 + x_, y_); }
    RPoint2 mirrorV(int axis = 0) { return RPoint2(x_, (axis-y_)*2 + y_); }

protected:
    int x_;
    int y_;
};

class RPoint3 : public RPoint2
{
    friend std::hash<RPoint3>;

public:
    explicit RPoint3(int x, int y = 0, int z = 0):
        RPoint2(x, y), z_(z) {}

    RPoint3(RPoint2 p2, int z):
        RPoint2(p2), z_(z) {}

    RPoint3():
        RPoint2(), z_(0) {}

    bool operator==(const RPoint3 &pos) const { return x_ == pos.x_ && y_ == pos.y_ && z_ == pos.z_; }
    bool operator!=(const RPoint3 &pos) const { return x_ != pos.x_ || y_ != pos.y_ || z_ != pos.z_; }

    RPoint3 operator+(const RPoint3 &pos) { return RPoint3(x_ += pos.x_, y_ += pos.y_, z_ += pos.z_); }
    RPoint3 operator-(const RPoint3 &pos) { return RPoint3(x_ -= pos.x_, y_ -= pos.y_, z_ -= pos.z_); }

    RPoint3& operator+=(const RPoint3 &pos) { x_ += pos.x_; y_ += pos.y_; z_ += pos.z_; return *this; }
    RPoint3& operator-=(const RPoint3 &pos) { x_ -= pos.x_; y_ -= pos.y_; z_ -= pos.z_; return *this; }

    int z() const { return z_; }
    int& rz() { return z_; }

    bool isValid() const { return RPoint2::isValid() && z_ != INVALID_POINT_VALUE; }
    bool isOrigin() const { return RPoint2::isOrigin() && z_ == 0;}

    void setZ(int z) { z_ = z; }
    void set(int x, int y, int z) { x_ = x, y_ = y; z_ = z; }
    void set(const RPoint2 &pos, int z) { x_ = pos.x(), y_ = pos.y(); z_ = z; }

protected:
    int z_;
};

using RPoint = RPoint3;

} // Redopera

namespace std {

template <>
struct hash<Redopera::RPoint2>
{
    typedef int64_t result_type;
    typedef Redopera::RPoint2 argument_type;

    int64_t operator()(const Redopera::RPoint2 &pos) const
    {
        return pos.x_ ^ (static_cast<int64_t>(pos.y_) << 32);
    }
};

template <>
struct hash<Redopera::RPoint3>
{
    typedef int64_t result_type;
    typedef Redopera::RPoint3 argument_type;

    int64_t operator()(const Redopera::RPoint3 &pos) const
    {
        return pos.x_ ^ (static_cast<int64_t>(pos.y_) << 24) ^ (static_cast<int64_t>(pos.z_) << 48);
    }
};

} // std

#endif // RPOINT_H
