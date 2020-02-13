#ifndef RCORSOR_H
#define RCORSOR_H

#include "RResource.h"
#include "../ROpenGL.h"
#include "RImage.h"

namespace Redopera {

class RCursor : public RResource
{
    friend void swap(RCursor &cs1, RCursor &cs2);

public:
    enum class Shape
    {
        Arrow        = GLFW_ARROW_CURSOR,
        Ibeam        = GLFW_IBEAM_CURSOR,
        Crosshair    = GLFW_CROSSHAIR_CURSOR,
        Hand         = GLFW_HAND_CURSOR,
        Hresize      = GLFW_HRESIZE_CURSOR,
        Vresize      = GLFW_VRESIZE_CURSOR,
        Custom
    };

    RCursor();
    explicit RCursor(const RImage &image, const std::string &name = "Cursor", int xhot = 0, int yhot = 0);
    explicit RCursor(Shape shape, const std::string &name = "Cursor");
    RCursor(const RCursor &cursor);
    RCursor(const RCursor &&cursor);
    RCursor& operator=(RCursor cursor);
    ~RCursor() = default;
    void swap(RCursor &cursor);

    bool isValid() const;
    Shape shape() const;
    GLFWcursor* data() const;

    bool load(const RImage &image, int xhot = 0, int yhot = 0);
    bool load(Shape shape);
    void release();

private:
    std::shared_ptr<GLFWcursor> data_;
    Shape shape_ = Shape::Custom;
};

} // Redopera

void swap(Redopera::RCursor &cursor1, Redopera::RCursor &cursor2);

#endif // RCORSOR_H
