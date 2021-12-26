#ifndef RSIZE_H
#define RSIZE_H

namespace Redopera {

namespace basic {

template <typename Value>
class RSizeBasic
{
public:
    RSizeBasic() noexcept: RSizeBasic<Value>(0, 0) {}
    RSizeBasic(Value width, Value height) noexcept : width_(width), height_(height) {}

    template<typename T>
    RSizeBasic<T> convert() const noexcept { return RSizeBasic<T>(width_, height_); }

    bool operator==(const RSizeBasic<Value> &size) const noexcept { return width_ == size.width_ && height_ == size.height_; }
    bool operator!=(const RSizeBasic<Value> &size) const noexcept { return width_ != size.width_ || height_ != size.height_; }

    template<typename T>
    RSizeBasic<Value>& operator*=(T value) noexcept { width_ *= value; height_ *= value; return *this; }
    template<typename T>
    RSizeBasic<Value>& operator/=(T value) noexcept { width_ /= value; height_ /= value; return *this; }

    template<typename T>
    RSizeBasic<Value> operator*(T value) const noexcept { return RSizeBasic<Value>(width_ * value, height_ * value); }
    template<typename T>
    RSizeBasic<Value> operator/(T value) const noexcept { return RSizeBasic<Value>(width_ / value, height_ / value); }

    Value width() const noexcept { return width_; }
    Value height() const noexcept { return height_; }

    bool isEmpty() const noexcept { return (width_ == 0) && (height_ == 0); }
    bool isInvalid() const noexcept { return (width_ <= 0) || (height_ <= 0); }
    bool isValid() const noexcept { return (width_ > 0) && (height_ > 0); }

    void setWidth(Value width) noexcept { width_ = width; }
    void setHeight(Value height) noexcept { height_ = height; }
    void setSize(Value width, Value height) noexcept { width_ = width; height_ = height; }
    void setSize(const RSizeBasic<Value> &size) noexcept { setSize(size.width(), size.height()); }

    Value& rWidth() noexcept { return width_; }
    Value& rHeight() noexcept { return height_; }

    void expand(Value width, Value height) noexcept { width_ += width; height_ += height; }

protected:
    Value width_;
    Value height_;
};

extern template class RSizeBasic<int>;
extern template class RSizeBasic<float>;

} // ns basic

using RSize = basic::RSizeBasic<int>;
using RSizeF = basic::RSizeBasic<float>;

} // Redopera

#ifdef REDOPERA_DEFINE_FILE
template class Redopera::basic::RSizeBasic<int>;
template class Redopera::basic::RSizeBasic<float>;
#endif

#endif // RSIZE_H
