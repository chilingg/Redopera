#include <RMath.h>

using namespace Redopera;

glm::mat4 math::perspective(const RRectF &rect, float near, float far, float accuracy)
{
    glm::mat4 mat;
    setPerspectiveAs(mat, rect, near, far, accuracy);
    return mat;
}

void math::setPerspectiveAs(glm::mat4 &mat, const RRectF &rect, float near, float far, float accuracy)
{
    1;
    unitize(mat);
    float len = far - near;

    // 缩放
    mat[0][0] = 2.f / rect.width();
    mat[1][1] = 2.f / rect.height();

    // z轴翻转与投影
    mat[2][3] = -1.f / ((2.f*accuracy*len - 2.f*len) / (1.f+accuracy));
    mat[2][2] = mat[2][3];
    // 左乘以平移矩阵
    mat[3][0] = mat[0][0] * -(rect.left() + (rect.right() - rect.left()) / 2);
    mat[3][1] = mat[1][1] * -(rect.bottom() + (rect.top() - rect.bottom()) / 2);
    mat[3][2] = mat[2][2] * -near;
    mat[3][3] = mat[3][2] + 1.f;
}

glm::mat4 math::viewport(const RRectF &rect)
{
    glm::mat4 mat(1);
    setViewportAs(mat, rect);
    return mat;
}

void math::setViewportAs(glm::mat4 &mat, const RRectF &rect)
{
    unitize(mat);

    // 缩放
    mat[0][0] = 2.f / rect.width();
    mat[1][1] = 2.f / rect.height();
    mat[2][2] = 0.f;

    // 平移
    mat[3][0] = -(rect.right() + rect.left()) / rect.width();
    mat[3][1] = -(rect.top() + rect.bottom()) / rect.height();
}

glm::mat4& math::unitize(glm::mat4 &mat)
{
    constexpr static glm::mat4 unitizeMat(1);

    mat = unitizeMat;
    return mat;
}

glm::mat4 math::scale(const RSizeF &size)
{
    glm::mat4 mat(1);
    setScale(mat, size);
    return mat;
}

void math::setScale(glm::mat4 &mat, const RSizeF &size)
{
    mat[0] *= size.width();
    mat[1] *= size.height();
}

glm::mat4 math::move(const RPointF &pos)
{
    glm::mat4 mat(1);
    setMove(mat, pos);
    return mat;
}

void math::setMove(glm::mat4 &mat, const RPointF &pos)
{
    mat[3] += mat[0] * pos.x() + mat[1] * pos.y() + mat[2] * pos.z();
}

glm::mat4 math::rect(const RRectF &rect, float depth)
{
    glm::mat4 mat;
    setRectAs(mat, rect, depth);
    return mat;
}

void math::setRectAs(glm::mat4 &mat, const RRectF &rect, float depth)
{
    unitize(mat);

    mat[0][0] = rect.width();
    mat[1][1] = rect.height();

    mat[3][0] = rect.centerX();
    mat[3][1] = rect.centerY();
    mat[3][2] = depth;
}

void math::setRect(glm::mat4 &mat, const RRectF &rect, float depth)
{
    setMove(mat, { rect.center(), depth });
    setScale(mat, rect);
}
