#ifndef RRECT_H
#define RRECT_H

#include "RPoint.h"
#include "RSize.h"

namespace Redopera {

class RRect
{
public:
    RRect() noexcept;
    RRect(int x, int y, int width, int height) noexcept;
    RRect(const RPoint2 &pos, const RSize &size) noexcept;
    RRect(const RPoint2 &bottomLeft, const RPoint2 &topRight) noexcept;

    bool operator==(const RRect &rect) const;
    bool operator!=(const RRect &rect) const;

    RRect operator+(const RPoint2 &pos) const;
    RRect operator-(const RPoint2 &pos) const;

    RRect operator+=(const RPoint2 &pos);
    RRect operator-=(const RPoint2 &pos);

    void setSize(const RSize &size);
    void setSize(int width, int height);
    void setWidth(int width);
    void setHeight(int height);
    void setPos(const RPoint2 &pos);
    void setPos(int x, int y);
    void setPosX(int x);
    void setPosY(int y);
    void setBottomLeft(const RPoint2 &pos);
    void setBottomLeft(int x, int y);
    void setTopRight(const RPoint2 &pos);
    void setTopRIght(int x, int y);
    void setBottom(int bottom);
    void setLeft(int left);
    void setTop(int top);
    void setRight(int right);

    void setCenter(const RPoint2 &pos);
    void setCenter(int x, int y);
    void setCenterX(int x);
    void setCenterY(int y);
    void set(int x, int y, int width, int height);
    void set(const RPoint2 &pos, const RSize &size);

    void move(const RPoint2 &pos);
    void move(int x, int y);

    RSize& rSize();
    RPoint2& rPos();

    int top() const;
    int bottom() const;
    int left() const;
    int right() const;
    const RPoint2& pos() const;
    RPoint2 bottomLeft() const;
    RPoint2 bottomRight() const;
    RPoint2 topLeft() const;
    RPoint2 topRIght() const;
    RPoint2 center() const;
    int centerX() const;
    int centerY() const;
    int width() const;
    int height() const;
    const RSize& size() const;

    bool isValid() const;
    bool isInvalid() const;
    bool isEmpty() const;

    bool contains(const RRect &rect) const;
    bool contains(const RPoint2 &pos) const;
    bool contains(int x, int y) const;

    bool overlap(const RRect &rect) const;

    std::string toString() const
    {
        return "(" + std::to_string(pos_.x()) + ", " + std::to_string(pos_.y())
                + " | w:" + std::to_string(size_.width()) + " h:" + std::to_string(size_.height()) + ") ";
    }

private:
    RPoint2 pos_;
    RSize size_;
};

inline RRect::RRect() noexcept:
    RRect(RPoint2(0), RSize())
{}

inline RRect::RRect(int x, int y, int width, int height) noexcept:
    pos_(x, y), size_(width, height)
{}

inline RRect::RRect(const RPoint2 &pos, const RSize &size) noexcept:
    pos_(pos), size_(size)
{}

inline RRect::RRect(const RPoint2 &bottomLeft, const RPoint2 &topRight) noexcept:
    pos_(bottomLeft),
    size_(topRight.x() - bottomLeft.x(), topRight.y() - bottomLeft.y())
{}

inline bool RRect::operator==(const RRect &rect) const
{
    return size_ == rect.size_ && pos_ == rect.pos_;
}

inline bool RRect::operator!=(const RRect &rect) const
{
    return size_ != rect.size_ || pos_ != rect.pos_;
}

inline RRect RRect::operator+(const RPoint2 &pos) const
{
    return RRect(pos_ + pos, size_);
}

inline RRect RRect::operator-(const RPoint2 &pos) const
{
    return RRect(pos_ - pos, size_);
}

inline RRect RRect::operator+=(const RPoint2 &pos)
{
    pos_ += pos;
    return *this;
}

inline RRect RRect::operator-=(const RPoint2 &pos)
{
    pos_ -= pos;
    return *this;
}

inline void RRect::setSize(const RSize &size)
{
    size_ = size;
}

inline void RRect::setSize(int width, int height)
{
    size_.set(width, height);
}

inline void RRect::setWidth(int width)
{
    size_.setWidth(width);
}

inline void RRect::setHeight(int height)
{
    size_.setHeight(height);
}

inline void RRect::setPos(const RPoint2 &pos)
{
    pos_ = pos;
}

inline void RRect::setPos(int x, int y)
{
    pos_.set(x, y);
}

inline void RRect::setPosX(int x)
{
    pos_.setX(x);
}

inline void RRect::setPosY(int y)
{
    pos_.setY(y);
}

inline void RRect::setBottomLeft(const RPoint2 &pos)
{
    pos_ = pos;
}

inline void RRect::setBottomLeft(int x, int y)
{
    pos_.set(x, y);
}

inline void RRect::setTopRight(const RPoint2 &pos)
{
    pos_.set(pos.x() - size_.width(), pos.y() - size_.height());
}

inline void RRect::setTopRIght(int x, int y)
{
    pos_.set(x - size_.width(), y - size_.height());
}

inline void RRect::setBottom(int bottom)
{
    pos_.setY(bottom);
}

inline void RRect::setLeft(int left)
{
    pos_.setX(left);
}

inline void RRect::setTop(int top)
{
    pos_.setY(top - size_.height());
}

inline void RRect::setRight(int right)
{
    pos_.setX(right - pos_.x());
}

inline void RRect::setCenter(const RPoint2 &pos)
{
    pos_.set(pos.x() - size_.width()/2, pos.y() - size_.height()/2);
}

inline void RRect::setCenter(int x, int y)
{
    pos_.set(x - size_.width()/2, y - size_.height()/2);
}

inline void RRect::setCenterX(int x)
{
    pos_.setX(x - size_.width()/2);
}

inline void RRect::setCenterY(int y)
{
    pos_.setY(y - size_.height()/2);
}

inline void RRect::set(int x, int y, int width, int height)
{
    size_.set(width, height);
    pos_.set(x, y);
}

inline void RRect::set(const RPoint2 &pos, const RSize &size)
{
    size_ = size;
    pos_ = pos;
}

inline void RRect::move(const RPoint2 &pos)
{
    pos_ += pos;
}

inline void RRect::move(int x, int y)
{
    pos_.move(x, y);
}

inline RSize &RRect::rSize()
{
    return size_;
}

inline RPoint2 &RRect::rPos()
{
    return pos_;
}

inline int RRect::top() const
{
    return pos_.y() + size_.height();
}

inline int RRect::bottom() const
{
    return pos_.y();
}

inline int RRect::left() const
{
    return pos_.x();
}

inline int RRect::right() const
{
    return pos_.x() + size_.width();
}

inline const RPoint2 &RRect::pos() const
{
    return pos_;
}

inline RPoint2 RRect::bottomLeft() const
{
    return pos_;
}

inline RPoint2 RRect::bottomRight() const
{
    return RPoint2(pos_.x() + size_.width(), pos_.y());
}

inline RPoint2 RRect::topLeft() const
{
    return RPoint2(pos_.x(), pos_.y() + size_.height());
}

inline RPoint2 RRect::topRIght() const
{
    return RPoint2(pos_.x() + size_.width(), pos_.y() + size_.height());
}

inline RPoint2 RRect::center() const
{
    return RPoint2(pos_.x() + size_.width()/2, pos_.y() + size_.height()/2);
}

inline int RRect::centerX() const
{
    return pos_.x() + size_.width() / 2;
}

inline int RRect::centerY() const
{
    return pos_.y() + size_.height() / 2;
}

inline int RRect::width() const
{
    return size_.width();
}

inline int RRect::height() const
{
    return size_.height();
}

inline const RSize& RRect::size() const
{
    return size_;
}

inline bool RRect::isValid() const
{
    return size_.isValid();
}

inline bool RRect::isInvalid() const
{
    return size_.isInvalid();
}

inline bool RRect::isEmpty() const
{
    return size_.isEmpty();
}

inline bool RRect::contains(const RRect &rect) const
{
    return rect.pos_.x() >= pos_.x() && rect.pos_.y() >= pos_.y()
            && rect.top() <= top() && rect.right() <= right();
}

inline bool RRect::contains(const RPoint2 &pos) const
{
    return pos.x() >= pos_.x() && pos.y() >= pos_.y() && pos.x() <= right() && pos.y() <= top();
}

inline bool RRect::contains(int x, int y) const
{
    return x >= pos_.x() && y >= pos_.y() && x <= right() && y <= top();
}

inline bool RRect::overlap(const RRect &rect) const
{
    return rect.bottom() < top() && rect.top() > bottom() && rect.left() < right() && rect.right() > left();
}

} // ns Redopera

#endif // RRECT_H
