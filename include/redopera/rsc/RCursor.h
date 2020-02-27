#ifndef RCORSOR_H
#define RCORSOR_H

#include "RImage.h"
#include "ROpenGL.h"

#include <memory>

class GLFWcursor;

namespace Redopera {

class RCursor
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

    RCursor() = default;
    explicit RCursor(const RImage &image, int xhot = 0, int yhot = 0);
    explicit RCursor(Shape shape);
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
    Shape shape_;
};

} // Redopera

void swap(Redopera::RCursor &cursor1, Redopera::RCursor &cursor2);

#endif // RCORSOR_H
