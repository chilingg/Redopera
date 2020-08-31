#include <rsc/RCursor.h>
#include <rsc/RImage.h>

using namespace Redopera;

RCursor::RCursor(const RImage &image, int xhot, int yhot)
{
    load(image, xhot, yhot);
}

RCursor::RCursor(RCursor::Shape shape)
{
    load(shape);
}

RCursor::RCursor(const RCursor &cursor):
    data_(cursor.data_),
    shape_(cursor.shape_)
{

}

RCursor::RCursor(const RCursor &&cursor):
    data_(std::move(cursor.data_)),
    shape_(cursor.shape_)
{

}

RCursor &RCursor::operator=(RCursor cursor)
{
    swap(cursor);
    return *this;
}

void RCursor::swap(RCursor &cursor)
{
    data_.swap(cursor.data_);
    using std::swap;
    swap(shape_, cursor.shape_);
}

bool RCursor::isValid() const
{
    return data_ != nullptr;
}

RCursor::Shape RCursor::shape() const
{
    return shape_;
}

GLFWcursor *RCursor::data() const
{
    return data_.get();
}

bool RCursor::load(const RImage &image, int xhot, int yhot)
{
    GLFWimage img { image.width(), image.height(), image.data() };
    GLFWcursor *cursor = glfwCreateCursor(&img, xhot, yhot);
    if(cursor)
    {
        data_.reset(cursor, glfwDestroyCursor);
        shape_ = Shape::Custom;
        return true;
    }

    return false;
}

bool RCursor::load(RCursor::Shape shape)
{
    GLFWcursor *cursor = glfwCreateStandardCursor(static_cast<int>(shape));
    if(cursor)
    {
        data_.reset(cursor, glfwDestroyCursor);
        shape_ = shape;
        return true;
    }

    return false;
}

void RCursor::release()
{
    data_.reset();
}

void swap(RCursor &cursor1, RCursor &cursor2)
{
    cursor1.swap(cursor2);
}
