#include <RArea.h>

using namespace Redopera;

RArea::Format RArea::areaFmt;

void RArea::setDefaultArea(RArea::Format fmt)
{
    areaFmt = fmt;
}

const RArea::Format &RArea::getDefaultArea()
{
    return areaFmt;
}

RArea::RArea():
    RArea(0, 0, 0, 0, 0)
{

}

RArea::RArea(int width, int height, int x, int y, int z, const RArea::Format &fmt):
    RArea(RSize(width, height), RPoint(x, y, z), fmt)
{

}

RArea::RArea(int width, int height, const RPoint &pos, const RArea::Format &fmt):
    RArea(RSize(width, height), pos, fmt)
{

}

RArea::RArea(const RSize &size, const RPoint &pos, const RArea::Format &fmt):
    size_(size),
    pos_(pos),
    format_(fmt)
{

}

RArea::RArea(const RRect &rect, int z, const RArea::Format &fmt):
    RArea(rect.size(), RPoint(rect.bottomLeft(), z), fmt)
{

}

RArea::RArea(const RArea &area):
    size_(area.size_),
    pos_(area.pos_),
    format_(area.format_),
    dirty_(area.dirty_)
{

}

RArea &RArea::operator=(const RArea &area)
{
    size_ = area.size_;
    pos_ = (area.pos_);
    format_ = (area.format_);
    dirty_ = (area.dirty_);
    return *this;
}

void RArea::setFormat(RArea::Format fmt)
{
    format_ = fmt;
    dirty_ = Move | Typeset | Rotate | Scale;
}

void RArea::setMinSize(int minw, int minh)
{
    minw = minw < 0 ? 0 : minw;
    minh = minh < 0 ? 0 : minh;
    format_.minW = minw;
    format_.minH = minh;
}

void RArea::setMinSize(const RSize &size)
{
    int minw = size.width() < 0 ? 0 : size.width();
    int minh = size.height() < 0 ? 0 : size.height();
    format_.minW = minw;
    format_.minH = minh;
}

void RArea::setMaxSize(int maxw, int maxh)
{
    if(maxw < 1 || maxh < 1)
        return;
    format_.maxW = maxw;
    format_.maxH = maxh;
}

void RArea::setMaxSize(const RSize &size)
{
    if(size.isInvalid())
        return;
    format_.maxW = size.width();
    format_.maxH = size.height();
}

int clamp(int v, int min, int max)
{
    if(v < min)
        return min;
    else if(v > max)
        return max;
    else
        return v;
}

void RArea::setSize(int width, int height)
{
    width = clamp(width, format_.minW, format_.maxW);
    height = clamp(height, format_.minH, format_.maxH);
    size_.set(width, height);

    addDirty(Scale);
}

void RArea::setSize(const RSize &size)
{
    int width = clamp(size.width(), format_.minW, format_.maxW);
    int height = clamp(size.height(), format_.minH, format_.maxH);
    size_.set(width, height);

    addDirty(Scale);
}

void RArea::setWidth(int width)
{
    width = clamp(width, format_.minW, format_.maxW);
    size_.setWidth(width);

    addDirty(Scale);
}

void RArea::setHeight(int height)
{
    height = clamp(height, format_.minH, format_.maxH);
    size_.setHeight(height);

    addDirty(Scale);
}

void RArea::setPos(int x, int y, int z)
{
    pos_.set(x, y, z);
    addDirty(Move);
}

void RArea::setPos(const RPoint &pos)
{
    pos_ = pos;
    addDirty(Move);
}

void RArea::setX(int x)
{
    pos_.setX(x);
    addDirty(Move);
}

void RArea::setY(int y)
{
    pos_.setY(y);
    addDirty(Move);
}

void RArea::setZ(int z)
{
    pos_.setZ(z);
    addDirty(Move);
}

void RArea::setOuterPos(const RPoint2 &pos)
{
    pos_.set(pos.x()+format_.margin.l, pos.y()+format_.margin.b, pos_.z());
    addDirty(Move);
}

void RArea::setOuterPos(int x, int y)
{
    pos_.set(x+format_.margin.l, y+format_.margin.b, pos_.z());
    addDirty(Move);
}

void RArea::setInnerPos(const RPoint2 &pos)
{
    pos_.set(pos.x()-format_.padding.l, pos.y()-format_.padding.b, pos_.z());
    addDirty(Move);
}

void RArea::setInnerPos(int x, int y)
{
    pos_.set(x-format_.padding.l, y-format_.padding.b, pos_.z());
    addDirty(Move);
}

void RArea::setCenterPos(const RPoint2 &pos)
{
    pos_.set(pos.x()-size_.width()/2, pos.y()-size_.height()/2, pos_.z());
    addDirty(Move);
}

void RArea::setCenterPos(int x, int y)
{
    pos_.set(x-size_.width()/2, y-size_.height()/2, pos_.z());
    addDirty(Move);
}

void RArea::setCenterPosX(int x)
{
    pos_.setX(x - size_.width()/2);
    addDirty(Move);
}

void RArea::setCenterPosY(int y)
{
    pos_.setY(y - size_.height()/2);
    addDirty(Move);
}

void RArea::move(int x, int y, int z)
{
    pos_ += RPoint(x, y, z);
    addDirty(Move);
}

void RArea::move(const RPoint &pos)
{
    pos_ += pos;
    addDirty(Move);
}

void RArea::setMargin(int top, int bottom, int left, int right)
{
    format_.margin = { top, bottom, left, right };
    addDirty(Typeset);
}

void RArea::setMargin(int value)
{
    format_.margin = { value, value, value, value };
    addDirty(Typeset);
}

void RArea::setPadding(int top, int bottom, int left, int right)
{
    format_.padding = { top, bottom, left, right };
    addDirty(Typeset);
}

void RArea::setPadding(int value)
{
    format_.padding = { value, value, value, value };
    addDirty(Typeset);
}

void RArea::setMode(RArea::Mode mode)
{
    format_.mode = mode;
    addDirty(Typeset);
}

void RArea::setAlign(RArea::Align v, RArea::Align h)
{
    format_.align = { v, h };
    addDirty(Typeset);
}

void RArea::addDirty(Dirty dirty)
{
    dirty_ |= dirty;
}

void RArea::setDirty(RArea::Dirty dirty)
{
    dirty_ = dirty;
}

void RArea::clearDirty()
{
    dirty_ = Clear;
}

void RArea::rotateX(float x)
{
    format_.rotate.x = x;
    addDirty(Rotate);
}

void RArea::rotateY(float y)
{
    format_.rotate.y = y;
    addDirty(Rotate);
}

void RArea::rotateZ(float z)
{
    format_.rotate.z = z;
    addDirty(Rotate);
}

void RArea::flipH()
{
    format_.flip.h = !format_.flip.h;
    addDirty(Scale);
}

void RArea::flipV()
{
    format_.flip.v = !format_.flip.v;
    addDirty(Scale);
}

RRect RArea::rect() const
{
    return { size_, pos_ };
}

RSize RArea::size() const
{
    return size_;
}

RPoint RArea::pos() const
{
    return pos_;
}

int RArea::width() const
{
    return size_.width();
}

int RArea::height() const
{
    return size_.height();
}

int RArea::x() const
{
    return pos_.x();
}

int RArea::y() const
{
    return pos_.y();
}

int RArea::z() const
{
    return pos_.z();
}

int &RArea::rx()
{
    addDirty(Move);
    return pos_.rx();
}

int &RArea::ry()
{
    addDirty(Move);
    return pos_.ry();
}

int &RArea::rz()
{
    addDirty(Move);
    return pos_.rz();
}

RPoint RArea::centerPos() const
{
    return RPoint(pos_.x() + size_.width()/2, pos_.y() + size_.height()/2, pos_.z());
}

int RArea::centerPosX() const
{
    return pos_.x() + size_.width()/2;
}

int RArea::centerPosY() const
{
    return pos_.y() + size_.height()/2;
}

RRect RArea::outerRect() const
{
    return RRect(outerSize(), outerPos());
}

RSize RArea::outerSize() const
{
    return RSize(outerWidth(), outerHeight());
}

RPoint RArea::outerPos() const
{
    return RPoint(pos_.x()-format_.margin.l, pos_.y()-format_.margin.b, pos_.z());
}

int RArea::outerWidth() const
{
    return size_.width() + format_.margin.l + format_.margin.r;
}

int RArea::outerHeight() const
{
    return size_.height() + format_.margin.t + format_.margin.b;
}

RRect RArea::innerRect() const
{
    return RRect(innerSize(), innerPos());
}

RSize RArea::innerSize() const
{
    return RSize(innerWidth(), innerHeight());
}

RPoint RArea::innerPos() const
{
    return RPoint(pos_.x()+format_.padding.l, pos_.y()+format_.padding.b, pos_.z());
}

int RArea::innerWidth() const
{
    return size_.width() - format_.padding.l - format_.padding.r;
}

int RArea::innerHeight() const
{
    return size_.height() - format_.padding.t - format_.padding.b;
}

RArea::Mode RArea::mode() const
{
    return format_.mode;
}

RArea::Align RArea::vAlign() const
{
    return format_.align.v;
}

RArea::Align RArea::hAlign() const
{
    return format_.align.h;
}

RSize RArea::maxSize() const
{
    return RSize(format_.maxW, format_.maxH);
}

RSize RArea::minSize() const
{
    return RSize(format_.minW, format_.minH);
}

int RArea::dirty() const
{
    return dirty_;
}

bool RArea::isFlipV() const
{
    return format_.flip.v;
}

bool RArea::isFlipH() const
{
    return format_.flip.h;
}

const RArea::Format &RArea::areaFormat() const
{
    return format_;
}
