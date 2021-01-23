#ifndef RPOINT_H
#define RPOINT_H

#include <string>

namespace Redopera {

class RPoint2
{
public:
    constexpr static int INVALID_POINT = ~0u >> 1;

    RPoint2() noexcept: x_(INVALID_POINT), y_(INVALID_POINT) {}
    explicit RPoint2(int x, int y = 0) noexcept: x_(x), y_(y) {}

    bool operator==(const RPoint2 &pos) const { return x_ == pos.x_ && y_ == pos.y_; }
    bool operator!=(const RPoint2 &pos) const { return x_ != pos.x_ || y_ != pos.y_; }

    RPoint2 operator+(const RPoint2 &pos) const { return RPoint2(x_ + pos.x_, y_ + pos.y_); }
    RPoint2 operator-(const RPoint2 &pos) const { return RPoint2(x_ - pos.x_, y_ - pos.y_); }

    RPoint2& operator+=(const RPoint2 &pos) { x_ += pos.x_; y_ += pos.y_; return *this; }
    RPoint2& operator-=(const RPoint2 &pos) { x_ -= pos.x_; y_ -= pos.y_; return *this; }

    int x() const { return x_; }
    int y() const { return y_; }
    int& rx() { return x_; }
    int& ry() { return y_; }
    bool isValid() const { return x_ != INVALID_POINT && y_ != INVALID_POINT; }
    bool isOrigin() const { return x_ == 0 && y_ == 0; }

    void setX(int x) { x_ = x; }
    void setY(int y) { y_ = y; }
    void set(int x, int y) { x_ = x; y_ = y; }

    void mirrorH(int axis = 0) { x_ = (axis-x_)*2 + x_; }
    void mirrorV(int axis = 0) { y_ = (axis-y_)*2 + y_; }

    void move(int x, int y) { x_ += x; y_ += y; }
    void move(const RPoint2 &pos) { x_ += pos.x_; y_ += pos.y_; }

    std::string toString() const
    {
        return '(' + std::to_string(x_) + ", " + std::to_string(y_) + ") ";
    }

protected:
    int x_;
    int y_;

}; // Point2

class RPoint3 : public RPoint2
{
public:
    RPoint3() noexcept: RPoint2(), z_(0) {}
    explicit RPoint3(int x, int y = 0, int z = 0) noexcept: RPoint2(x, y), z_(z) {}
    RPoint3(const RPoint2 &p2, int z = 0) noexcept: RPoint2(p2), z_(z) {}

    bool operator==(const RPoint3 &pos) const { return x_ == pos.x_ && y_ == pos.y_ && z_ == pos.z_; }
    bool operator!=(const RPoint3 &pos) const { return x_ != pos.x_ || y_ != pos.y_ || z_ != pos.z_; }

    RPoint3 operator+(const RPoint3 &pos) const { return RPoint3(x_ + pos.x_, y_ + pos.y_, z_ + pos.z_); }
    RPoint3 operator-(const RPoint3 &pos) const { return RPoint3(x_ - pos.x_, y_ - pos.y_, z_ - pos.z_); }

    RPoint3& operator+=(const RPoint3 &pos) { x_ += pos.x_; y_ += pos.y_; z_ += pos.z_; return *this; }
    RPoint3& operator-=(const RPoint3 &pos) { x_ -= pos.x_; y_ -= pos.y_; z_ -= pos.z_; return *this; }

    int z() const { return z_; }
    int& rz() { return z_; }

    bool isValid() const { return RPoint2::isValid() && z_ != INVALID_POINT; }
    bool isOrigin() const { return RPoint2::isOrigin() && z_ == 0;}

    void setZ(int z) { z_ = z; }
    void set(int x, int y, int z) { x_ = x, y_ = y; z_ = z; }
    void set(const RPoint2 &pos, int z) { x_ = pos.x(), y_ = pos.y(); z_ = z; }

    void move(int x, int y, int z = 0) { RPoint2::move(x, y); z_ += z; }
    void move(const RPoint3 &pos) { RPoint2::move(pos); z_ += pos.z_; };

    std::string toString() const
    {
        return '(' + std::to_string(x_) + ", " + std::to_string(y_) + ", " + std::to_string(z_) + ") ";
    }

protected:
    int z_;

}; // Point2

using RPoint = RPoint3;

} // ns Redopera

#endif // RPOINT_H
