#ifndef RCOLOR_H
#define RCOLOR_H

#include <cstdint>

namespace Redopera {

using R_RGBA = uint32_t;

class RColor
{
public:
    explicit RColor(R_RGBA color = 0xff000000u) { rgba_ = color; }
    RColor(unsigned r, unsigned g, unsigned b, unsigned a = 0xffu)
    {
        rgba_ = r & 0xffu;
        rgba_ |= (0xffu & g) << 8;
        rgba_ |= (0xffu & b) << 16;
        rgba_ |= a << 24;
    }

    bool operator==(const RColor &color) { return rgba_ == color.rgba_; }
    bool operator!=(const RColor &color) { return rgba_ != color.rgba_; }

    unsigned char a() const { return rgba_ >> 24; }
    unsigned char b() const { return rgba_ >> 16 & 0xffu; }
    unsigned char g() const { return rgba_ >> 8 & 0xffu; }
    unsigned char r() const { return rgba_ & 0xffu; }

    R_RGBA rgba() const { return rgba_; }
    R_RGBA abgr() const { return (static_cast<R_RGBA>(r()) << 24) |
                (static_cast<R_RGBA>(g()) << 16) |
                (static_cast<R_RGBA>(b()) << 8) | a(); }

    void setA(unsigned a) { rgba_ = (rgba_ & 0xffffff) | a << 24; }
    void setB(unsigned b) { rgba_ = (rgba_ & 0xff00ffff) | (0xffu & b) << 16; }
    void setG(unsigned g) { rgba_ = (rgba_ & 0xffff00ff) | (0xffu & g) << 8; }
    void setR(unsigned r) { rgba_ = (rgba_ & 0xffffff00) | (r & 0xffu); }
    void setRGBA(R_RGBA rgba) { rgba_ = rgba; }

private:
    R_RGBA rgba_ = 0u;
};

} // Redopera

#endif // RCOLOR_H
