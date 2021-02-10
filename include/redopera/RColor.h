#ifndef RCOLOR_H
#define RCOLOR_H

#include <cstdint>
#include <string>

namespace Redopera {

using RGBA = uint32_t;

class RColor
{
public:
    explicit RColor(RGBA rgba = 0xffu) noexcept:
        rgba_(rgba) {}

    RColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 0xffu) noexcept
    {
        rgba_ = a & 0xffu;
        rgba_ |= (0xffu & b) << 8;
        rgba_ |= (0xffu & g) << 16;
        rgba_ |= r << 24;
    }

    bool operator==(const RColor &color) const
    {
        return rgba_ == color.rgba_;
    }

    bool operator!=(const RColor &color) const
    {
        return rgba_ != color.rgba_;
    }

    uint8_t r() const { return rgba_ >> 24; }
    uint8_t g() const { return rgba_ >> 16 & 0xffu; }
    uint8_t b() const { return rgba_ >> 8 & 0xffu; }
    uint8_t a() const { return rgba_ & 0xffu; }

    RGBA rgba() const { return rgba_; }
    RGBA rgb() const { return rgba_ >> 8; }
    RGBA bgr() const { return ((b() << 16) | (g() << 8) | r()); }
    RGBA abgr() const { return ((a() << 24) | (b() << 16) | (g() << 8) | r()); }

    void setR(uint32_t r) { rgba_ = (rgba_ & 0xffffff) | r << 24; }
    void setG(uint32_t g) { rgba_ = (rgba_ & 0xff00ffff) | (0xffu & g) << 16; }
    void setB(uint32_t b) { rgba_ = (rgba_ & 0xffff00ff) | (0xffu & b) << 8; }
    void setA(uint32_t a) { rgba_ = (rgba_ & 0xffffff00) | (a & 0xffu); }
    void setRGBA(RGBA rgba) { rgba_ = rgba; }
    void setRGB(RGBA rgb) { rgba_ &= 0x000000ff; rgba_ |= (rgb << 8); }

    void setRGBA(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
        rgba_ = (r << 24) | ((0xffu & g) << 16) | ((0xffu & b) << 8) | (a & 0xffu);
    }

    void setRGB(uint32_t r, uint32_t g, uint32_t b) {
        rgba_ = (r << 24) | ((0xffu & g) << 16) | ((0xffu & b) << 8) | (rgba_ & 0xffu);
    }

    std::string toString() const
    {
        return "(r:" + std::to_string(r()) + " g:" + std::to_string(g()) + " b:" + std::to_string(b()) + " a:" + std::to_string(a()) + ") ";
    }

private:
    RGBA rgba_;
};

} // ns Redopera

#endif // RCOLOR_H
