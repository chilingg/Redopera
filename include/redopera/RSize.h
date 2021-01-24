#ifndef RSIZE_H
#define RSIZE_H

#include <string>

namespace Redopera {

namespace RValue {

template <typename Value>
class RSizeValue
{
public:
    RSizeValue() noexcept: RSizeValue<Value>(0, 0) {}
    RSizeValue(Value width, Value height) noexcept : width_(width), height_(height) {}

    template<typename T>
    operator RSizeValue<T>() const { return RSizeValue<T>(width_, height_); }

    bool operator==(const RSizeValue<Value> &size) const { return width_ == size.width_ && height_ == size.height_; }
    bool operator!=(const RSizeValue<Value> &size) const { return width_ != size.width_ || height_ != size.height_; }

    template<typename T>
    RSizeValue<Value>& operator*=(T value) { width_ *= value; height_ *= value; return *this; }
    template<typename T>
    RSizeValue<Value>& operator/=(T value) { width_ /= value; height_ /= value; return *this; }

    template<typename T>
    RSizeValue<Value> operator*(T value) const { return RSizeValue<Value>(width_ * value, height_ * value); }
    template<typename T>
    RSizeValue<Value> operator/(T value) const { return RSizeValue<Value>(width_ / value, height_ / value); }

    Value width() const { return width_; }
    Value height() const { return height_; }

    bool isEmpty() const { return (width_ == 0) && (height_ == 0); }
    bool isInvalid() const { return (width_ <= 0) || (height_ <= 0); }
    bool isValid() const { return (width_ > 0) && (height_ > 0); }

    void setWidth(Value width) { width_ = width; }
    void setHeight(Value height) { height_ = height; }
    void setSize(Value width, Value height) { width_ = width; height_ = height; }
    void setSize(const RSizeValue<Value> &size) { setSize(size.width(), size.height()); }

    void expand(Value width, Value height) { width_ += width; height_ += height; }

    std::string toString() const
    {
        return "(w:" + std::to_string(width_) + " h:" + std::to_string(height_) + ") ";
    }

protected:
    Value width_;
    Value height_;
};

extern template class RSizeValue<int>;
extern template class RSizeValue<float>;

} // sn RValue

using RSize = RValue::RSizeValue<int>;
using RSizeF = RValue::RSizeValue<float>;

} // ns Redopera

#endif // RSIZE_H
