#ifndef RCOLOR_H
#define RCOLOR_H

#include <cstdint>

namespace Redopera {

using R_RGBA = uint32_t;
using R_RGB = uint32_t;

class RColor
{
public:
    explicit RColor(R_RGBA rgba = 0x000000ffu) noexcept
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

    bool operator==(const RColor &color)
    {
        return rgba_ == color.rgba_;
    }

    bool operator!=(const RColor &color)
    {
        return rgba_ != color.rgba_;
    }

    unsigned char r() const { return rgba_ >> 24; }
    unsigned char g() const { return rgba_ >> 16 & 0xffu; }
    unsigned char b() const { return rgba_ >> 8 & 0xffu; }
    unsigned char a() const { return rgba_ & 0xffu; }

    R_RGBA rgba() const { return rgba_; }
    R_RGB rgb() const { return rgba_ >> 8; }

    void setR(unsigned r) { rgba_ = (rgba_ & 0xffffff) | r << 24; }
    void setG(unsigned g) { rgba_ = (rgba_ & 0xff00ffff) | (0xffu & g) << 16; }
    void setB(unsigned b) { rgba_ = (rgba_ & 0xffff00ff) | (0xffu & b) << 8; }
    void setA(unsigned a) { rgba_ = (rgba_ & 0xffffff00) | (a & 0xffu); }
    void setRGBA(R_RGBA rgba) { rgba_ = rgba; }
    void setRGB(R_RGB rgb) { rgba_ &= 0x000000ff; rgba_ |= (rgb << 8); }

private:
    R_RGBA rgba_ = 0x000000ffu;
};

} // ns Redopera

#endif // RCOLOR_H
