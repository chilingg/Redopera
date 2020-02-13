#include "rsc/RCursor.h"

Redopera::RCursor::RCursor():
    RResource("Cursor", RResource::Type::Cursor)
{

}

Redopera::RCursor::RCursor(const Redopera::RImage &image, const std::string &name, int xhot, int yhot):
    RResource(name, RResource::Type::Cursor)
{
    load(image, xhot, yhot);
}

Redopera::RCursor::RCursor(Redopera::RCursor::Shape shape, const std::string &name):
    RResource(name, RResource::Type::Cursor)
{
    load(shape);
}

Redopera::RCursor::RCursor(const Redopera::RCursor &cursor):
    RResource(cursor),
    data_(cursor.data_),
    shape_(cursor.shape_)
{

}

Redopera::RCursor::RCursor(const Redopera::RCursor &&cursor):
    RResource(std::move(cursor)),
    data_(std::move(cursor.data_)),
    shape_(cursor.shape_)
{

}

Redopera::RCursor &Redopera::RCursor::operator=(Redopera::RCursor cursor)
{
    swap(cursor);
    return *this;
}

void Redopera::RCursor::swap(Redopera::RCursor &cursor)
{
    RResource::swap(cursor);
    data_.swap(cursor.data_);
    using std::swap;
    swap(shape_, cursor.shape_);
}

bool Redopera::RCursor::isValid() const
{
    return data_ != nullptr;
}

Redopera::RCursor::Shape Redopera::RCursor::shape() const
{
    return shape_;
}

GLFWcursor *Redopera::RCursor::data() const
{
    return data_.get();
}

bool Redopera::RCursor::load(const Redopera::RImage &image, int xhot, int yhot)
{
    GLFWimage img { image.width(), image.height(), image.data() };
    GLFWcursor *cursor = glfwCreateCursor(&img, xhot, yhot);
    if(cursor)
    {
        if(!data_.unique())
            resetRscID();
        data_.reset(cursor, glfwDestroyCursor);
        shape_ = Shape::Custom;
        return true;
    }

    return false;
}

bool Redopera::RCursor::load(Redopera::RCursor::Shape shape)
{
    GLFWcursor *cursor = glfwCreateStandardCursor(static_cast<int>(shape));
    if(cursor)
    {
        if(!data_.unique())
            resetRscID();
        data_.reset(cursor, glfwDestroyCursor);
        shape_ = shape;
        return true;
    }

    return false;
}

void Redopera::RCursor::release()
{
    data_.reset();
}

void swap(Redopera::RCursor &cursor1, Redopera::RCursor &cursor2)
{
    cursor1.swap(cursor2);
}
