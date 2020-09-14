#ifndef RCOLOR_H
#define RCOLOR_H

#include <cstdint>

namespace Redopera {

using RGBA = uint32_t;

class RColor
{
public:
    explicit RColor(RGBA rgba = 0xffffffffu) noexcept
    {
        rgba_ = rgba;
    }

    RColor(unsigned r, unsigned g, unsigned b, unsigned a = 0xffu) noexcept
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
    RGBA bgr() const { return (((b() << 8) | g()) << 8) | r(); }

    void setR(unsigned r) { rgba_ = (rgba_ & 0xffffff) | r << 24; }
    void setG(unsigned g) { rgba_ = (rgba_ & 0xff00ffff) | (0xffu & g) << 16; }
    void setB(unsigned b) { rgba_ = (rgba_ & 0xffff00ff) | (0xffu & b) << 8; }
    void setA(unsigned a) { rgba_ = (rgba_ & 0xffffff00) | (a & 0xffu); }
    void setRGBA(RGBA rgba) { rgba_ = rgba; }
    void setRGB(RGBA rgb) { rgba_ &= 0x000000ff; rgba_ |= (rgb << 8); }

private:
    RGBA rgba_ = 0xffffffffu;
};

} // ns Redopera

#endif // RCOLOR_H
