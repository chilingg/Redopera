#ifndef RSIZE_H
#define RSIZE_H

namespace Redopera {

class RSize
{
public:
    RSize() noexcept: RSize(0, 0) {}
    RSize(int width, int height) noexcept : width_(width), height_(height) {}

    bool operator==(const RSize &size) { return width_ == size.width_ && height_ == size.height_; }
    bool operator!=(const RSize &size) { return width_ != size.width_ || height_ != size.height_; }

    RSize& operator*=(double value) { width_ *= value; height_ *= value; return *this; }
    RSize& operator/=(double value) { width_ /= value; height_ /= value; return *this; }

    RSize& operator*=(float value) { width_ *= value; height_ *= value; return *this; }
    RSize& operator/=(float value) { width_ /= value; height_ /= value; return *this; }

    RSize& operator*=(int value) { width_ *= value; height_ *= value; return *this; }
    RSize& operator/=(int value) { width_ /= value; height_ /= value; return *this; }

    RSize operator*(double value) { return RSize(width_ * value, height_ * value); }
    RSize operator/(double value) { return RSize(width_ / value, height_ / value); }

    RSize operator*(float value) { return RSize(width_ * value, height_ * value); }
    RSize operator/(float value) { return RSize(width_ / value, height_ / value); }

    RSize operator*(int value) { return RSize(width_ * value, height_ * value); }
    RSize operator/(int value) { return RSize(width_ / value, height_ / value); }

    int width() const { return width_; }
    int height() const { return height_; }

    bool isEmpty() const { return (width_ == 0) && (height_ == 0); }
    bool isInvalid() const { return (width_ <= 0) || (height_ <= 0); }
    bool isValid() const { return (width_ > 0) && (height_ > 0); }

    void setWidth(int width) { width_ = width; }
    void setHeight(int height) { height_ = height; }
    void set(int width, int height) { width_ = width; height_ = height; }

    void expand(int width, int height) { width_ += width; height_ += height; }

private:
    int width_;
    int height_;
};

} // ns Redopera

#endif // RSIZE_H
