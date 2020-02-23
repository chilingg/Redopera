#ifndef RCOLOR_H
#define RCOLOR_H

#include <cstdint>

namespace Redopera {

using R_ARGB = uint32_t;
using R_RGB = uint32_t;

class RColor
{
public:
    explicit RColor(R_ARGB argb = 0xff000000u) noexcept
    {
        argb_ = argb;
    }

    RColor(unsigned r, unsigned g, unsigned b, unsigned a = 0xffu) noexcept
    {
        argb_ = b & 0xffu;
        argb_ |= (0xffu & g) << 8;
        argb_ |= (0xffu & r) << 16;
        argb_ |= a << 24;
    }

    bool operator==(const RColor &color)
    {
        return argb_ == color.argb_;
    }

    bool operator!=(const RColor &color)
    {
        return argb_ != color.argb_;
    }

    unsigned char a() const { return argb_ >> 24; }
    unsigned char r() const { return argb_ >> 16 & 0xffu; }
    unsigned char g() const { return argb_ >> 8 & 0xffu; }
    unsigned char b() const { return argb_ & 0xffu; }

    R_ARGB argb() const { return argb_; }
    R_RGB rgb() const { return argb_ & 0xffffff; }

    void setA(unsigned a) { argb_ = (argb_ & 0xffffff) | a << 24; }
    void setR(unsigned r) { argb_ = (argb_ & 0xff00ffff) | (0xffu & r) << 16; }
    void setG(unsigned g) { argb_ = (argb_ & 0xffff00ff) | (0xffu & g) << 8; }
    void setB(unsigned b) { argb_ = (argb_ & 0xffffff00) | (b & 0xffu); }
    void setRGBA(R_ARGB argb) { argb_ = argb; }
    void setRGB(R_RGB rgb) { argb_ &= 0xff000000; argb_ &= rgb; }

private:
    R_ARGB argb_ = 0xff000000u;
};

} // ns Redopera

#endif // RCOLOR_H
