#ifndef RRECT_H
#define RRECT_H

#include "RPoint.h"
#include "RSize.h"

namespace Redopera {

class RRect
{
public:
    RRect();
    RRect(int width, int height, int x = 0, int y = 0);
    RRect(int width, int height, const RPoint2 &bottomLeft);
    RRect(const RSize &size, const RPoint2 &bottomLeft);
    RRect(const RPoint2 &bottomLeft, const RPoint2 &topRight);

    bool operator==(const RRect &rect);
    bool operator!=(const RRect &rect);

    RRect operator+(const RPoint2 &pos);
    RRect operator-(const RPoint2 &pos);

    RRect operator+=(const RPoint2 &pos);
    RRect operator-=(const RPoint2 &pos);

    void setSize(const RSize &size);
    void setSize(int width, int height);
    void setWidth(int width);
    void setHeight(int height);
    void setBottomLeft(const RPoint2 &pos);
    void setBottomLeft(int x, int y);
    void setTopRight(const RPoint2 &pos);
    void setTopRIght(int x, int y);
    void setBottom(int bottom);
    void setLeft(int left);
    void setTop(int top);
    void setRight(int right);

    void move(RPoint2 pos);
    void move(int x, int y);

    void setCenter(const RPoint2 &pos);
    void setCenter(int x, int y);
    void set(int width, int height, int x, int y);
    void set(const RSize &size, const RPoint2 &bottomLeft);

    int top() const;
    int bottom() const;
    int left() const;
    int right() const;
    RPoint2 bottomLeft() const;
    RPoint2 bottomRight() const;
    RPoint2 topLeft() const;
    RPoint2 topRIght() const;
    RPoint2 center() const;
    int width() const;
    int height() const;
    RSize size() const;

    bool isValid();
    bool isInvalid();
    bool isEmpty();

    bool contains(const RRect &rect);
    bool contains(const RPoint2 &pos);
    bool contains(int x, int y);

    bool overlap(const RRect &rect);

    RRect mirrorH(int axis = 0);
    RRect mirrorV(int axis = 0);

private:
    RSize size_;
    RPoint2 pos_;
};

inline RRect::RRect():
    RRect(RSize(), RPoint2(0))
{}

inline RRect::RRect(int width, int height, int x, int y):
    size_(width, height), pos_(x, y)
{}

inline RRect::RRect(int width, int height, const RPoint2 &bottomLeft):
    size_(width, height), pos_(bottomLeft)
{

}

inline RRect::RRect(const RSize &size, const RPoint2 &bottomLeft):
    size_(size), pos_(bottomLeft)
{}

inline RRect::RRect(const RPoint2 &bottomLeft, const RPoint2 &topRight):
    size_(bottomLeft.x() - topRight.x(), topRight.y() - bottomLeft.y()),
    pos_(bottomLeft)
{}

inline bool RRect::operator==(const RRect &rect)
{
    return size_ == rect.size_ && pos_ == rect.pos_;
}

inline bool RRect::operator!=(const RRect &rect)
{
    return size_ != rect.size_ || pos_ != rect.pos_;
}

inline RRect RRect::operator+(const RPoint2 &pos)
{
    return RRect(size_, pos_ + pos);
}

inline RRect RRect::operator-(const RPoint2 &pos)
{
    return RRect(size_, pos_ - pos);
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

inline void RRect::setBottomLeft(const RPoint2 &pos)
{
    RPoint2 p = pos_ - pos;
    pos_ = pos;
    size_.expand(p.x(), p.y());
}

inline void RRect::setBottomLeft(int x, int y)
{
    size_.expand(pos_.x() - x, pos_.y() - y);
    pos_.set(x, y);
}

inline void RRect::setTopRight(const RPoint2 &pos)
{
    size_.set(pos.x() - pos_.x(),  pos.y() - pos_.y());
}

inline void RRect::setTopRIght(int x, int y)
{
    size_.set(x - pos_.x(), y - pos_.y());
}

inline void RRect::setBottom(int bottom)
{
    size_.expand(0, pos_.y() - bottom);
    pos_.setY(bottom);
}

inline void RRect::setLeft(int left)
{
    size_.expand(pos_.x() - left, 0);
}

inline void RRect::setTop(int top)
{
    size_.expand(0, top - size_.height() - pos_.y());
}

inline void RRect::setRight(int right)
{
    size_.expand(pos_.x() - right, 0);
    pos_.setX(right);
}

inline void RRect::move(RPoint2 pos)
{
    pos_ += pos;
}

inline void RRect::move(int x, int y)
{
    pos_.rx() += x;
    pos_.ry() += y;
}

inline void RRect::setCenter(const RPoint2 &pos)
{
    pos_.set(pos.x() - size_.width()/2, pos.y() - size_.height()/2);
}

inline void RRect::setCenter(int x, int y)
{
    pos_.set(x - size_.width()/2, y - size_.height()/2);
}

inline void RRect::set(int width, int height, int x, int y)
{
    size_.set(width, height);
    pos_.set(x, y);
}

inline void RRect::set(const RSize &size, const RPoint2 &bottomLeft)
{
    size_ = size;
    pos_ = bottomLeft;
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

inline int RRect::width() const
{
    return size_.width();
}

inline int RRect::height() const
{
    return size_.height();
}

inline RSize RRect::size() const
{
    return size_;
}

inline bool RRect::isValid()
{
    return size_.isValid();
}

inline bool RRect::isInvalid()
{
    return size_.isInvalid();
}

inline bool RRect::isEmpty()
{
    return size_.isEmpty();
}

inline bool RRect::contains(const RRect &rect)
{
    return rect.pos_.x() >= pos_.x() && rect.pos_.y() >= pos_.y()
            && rect.top() <= top() && rect.right() <= right();
}

inline bool RRect::contains(const RPoint2 &pos)
{
    return pos.x() >= pos_.x() && pos.y() >= pos_.y() && pos.x() <= right() && pos.y() <= top();
}

inline bool RRect::contains(int x, int y)
{
    return x >= pos_.x() && y >= pos_.y() && x <= right() && y <= top();
}

inline bool RRect::overlap(const RRect &rect)
{
    return rect.bottom() < top() && rect.top() > bottom() && rect.left() < right() && rect.right() > left();
}

inline RRect RRect::mirrorH(int axis)
{
    int x = (axis - pos_.x()) * 2 + pos_.x() - size_.width();
    return RRect(size_, RPoint2(x, pos_.y()));
}

inline RRect RRect::mirrorV(int axis)
{
    int y = (axis - pos_.y()) * 2 + pos_.y() - size_.height();
    return RRect(size_, RPoint2(pos_.x(), y));
}

} // Redopera

#endif // RRECT_H
