#ifndef RAREA_H
#define RAREA_H

#include <RPoint.h>
#include <RSize.h>
#include <RRect.h>

namespace Redopera {

class RArea
{
public:
    enum class Mode
    {
        Fix,
        Auto,
        Cover,
        Contain
    };

    enum class Align
    {
        Left,
        Right,
        Mind,
        Top,
        Bottom
    };

    enum Dirty
    {
        Clear   = 0,
        Move    = 1,
        Typeset = 2,
        Rotate  = 4,
        Scale   = 8   // 包括镜像
    };

    struct Format
    {
        struct {
            bool h, v;
        } flip { false, false };

        Mode mode = Mode::Auto;

        struct {
            Align v, h;
        } align { Align::Top, Align::Left};

        struct {
            int t, b, l, r;
        } margin { 0, 0, 0, 0 }, padding { 0, 0, 0, 0 };

        struct {
            float x, y, z;
        } rotate { 0.0f, 0.0f, 0.0f };

        int minW = 0;
        int minH = 0;
        int maxW = ~0u >> 1;
        int maxH = ~0u >> 1;
    };

    static void setDefaultArea(Format fmt);
    static const Format& getDefaultArea();

    RArea();
    RArea(int width, int height, int x, int y, int z = 0, const Format &area = areaFmt);
    RArea(int width, int height, const RPoint &pos, const Format &area = areaFmt);
    RArea(const RSize &size, const RPoint &pos, const Format &area = areaFmt);
    explicit RArea(const RRect &rect, int z = 0, const Format &area = areaFmt);
    RArea(const RArea &area);
    RArea& operator=(const RArea &area);
    ~RArea() = default;

    void setFormat(Format fmt);

    void setMinSize(int minw, int minh);
    void setMinSize(const RSize &size);
    void setMaxSize(int maxw, int maxh);
    void setMaxSize(const RSize &size);

    void setSize(int width, int height);
    void setSize(const RSize &size);
    void setWidth(int width);
    void setHeight(int height);

    void setPos(int x, int y, int z = 0);
    void setPos(const RPoint &pos);
    void setX(int x);
    void setY(int y);
    void setZ(int z);

    void setOuterPos(const RPoint2 &pos);
    void setOuterPos(int x, int y);
    void setInnerPos(const RPoint2 &pos);
    void setInnerPos(int x, int y);

    void setCenterPos(const RPoint2 &pos);
    void setCenterPos(int x, int y);
    void setCenterPosX(int x);
    void setCenterPosY(int y);

    void move(int x, int y, int z = 0);
    void move(const RPoint &pos);

    void setMargin(int top, int bottom, int left, int right);
    void setMargin(int value);
    void setPadding(int top, int bottom, int left, int right);
    void setPadding(int value);

    void setMode(Mode mode);
    void setAlign(Align v, Align h);

    void addDirty(Dirty dirty);
    void setDirty(Dirty dirty);
    void clearDirty();

    void rotateX(float x);
    void rotateY(float y);
    void rotateZ(float z);

    void flipH();
    void flipV();

    RRect rect() const;
    RSize size() const;
    RPoint pos() const;
    int width() const;
    int height() const;
    int x() const;
    int y() const;
    int z() const;
    int& rx();
    int& ry();
    int& rz();

    RPoint centerPos() const;
    int centerPosX() const;
    int centerPosY() const;

    RRect outerRect() const;
    RSize outerSize() const;
    RPoint outerPos() const;
    int outerWidth() const;
    int outerHeight() const;

    RRect innerRect() const;
    RSize innerSize() const;
    RPoint innerPos() const;
    int innerWidth() const;
    int innerHeight() const;

    Mode mode() const;
    Align vAlign() const;
    Align hAlign() const;

    RSize maxSize() const;
    RSize minSize() const;

    int dirty() const;
    bool isFlipV() const;
    bool isFlipH() const;

    const Format &areaFormat() const;

private:
    static Format areaFmt;

    RSize size_;
    RPoint pos_;
    Format format_;
    int dirty_ = Move | Typeset | Rotate | Scale;
};

} //ns Redopera

#endif // RAREA_H
