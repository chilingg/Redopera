#ifndef RCOLOR_H
#define RCOLOR_H

#include "RDefine.h"

namespace Redopera {

class RColor
{
public:
    union ColorData {
        struct {
            uint8_t a, b, g, r;
        };
        uint32_t rgba;
    };

    explicit RColor(RGBA rgba = 0xffffffffu) noexcept { data_.rgba = rgba; }

    RColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xffu) noexcept:
        data_{ a, b, g, r }
    {}

    bool operator==(const RColor &color) const noexcept
    {
        return data_.rgba == color.data_.rgba;
    }

    bool operator!=(const RColor &color) const noexcept
    {
        return data_.rgba != color.data_.rgba;
    }

    uint8_t r() const noexcept { return data_.r; }
    uint8_t g() const noexcept { return data_.g; }
    uint8_t b() const noexcept { return data_.b; }
    uint8_t a() const noexcept { return data_.a; }

    float rf() const noexcept { return data_.r / 255.f; }
    float gf() const noexcept { return data_.g / 255.f; }
    float bf() const noexcept { return data_.b / 255.f; }
    float af() const noexcept { return data_.a / 255.f; }

    RGBA rgba() const noexcept { return data_.rgba; }
    RGBA rgb() const noexcept { return data_.rgba >> 8; }
    RGBA bgr() const noexcept { return ColorData { data_.r, data_.g, data_.b, 0 }.rgba; }
    RGBA abgr() const noexcept { return ColorData { data_.r, data_.g, data_.b, data_.a }.rgba; }

    ColorData data() const noexcept { return data_; }

    void setR(uint8_t r) noexcept { data_.r = r; }
    void setG(uint8_t g) noexcept { data_.g = g; }
    void setB(uint8_t b) noexcept { data_.b = b; }
    void setA(uint8_t a) noexcept { data_.a = a; }
    void setRGBA(RGBA rgba) noexcept { data_.rgba = rgba; }
    void setRGB(RGBA rgb) noexcept { data_.rgba &= 0xff; data_.rgba |= (rgb << 8); }

    void setRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept { data_ = { a, b, g, r }; }

    void setRGB(uint8_t r, uint8_t g, uint8_t b) noexcept { data_ = { data_.a, b, g, r }; }

private:
    ColorData data_;
};

} // ns Redopera

#endif // RCOLOR_H
