#ifndef RMATHS_H
#define RMATHS_H

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/* GLM算法和功能
 * min(genType x, genType y)   //返回最小值
 * max(genType x, genType y)   //返回最大值
 * abs(int x)                  //返回绝对值
 * round(genType x)            //四舍六入五取偶
 * trunc(genType x)            //返回截断整数部分
 */

#include <RRect.h>

namespace Redopera {

namespace math
{

glm::mat4 perspective(const RRectF &rect, float near, float far, float accuracy = 0.95); // accuracy 投影之后far平面拥有的最大z轴标量
void setPerspectiveAs(glm::mat4 &mat, const RRectF &rect, float near, float far, float accuracy);

glm::mat4 viewport(const RRectF &rect);
void setViewportAs(glm::mat4 &mat, const RRectF &rect);

glm::mat4 &unitize(glm::mat4 &mat);

glm::mat4 scale(const RSizeF &size);
void setScale(glm::mat4 &mat, const RSizeF &size);

glm::mat4 move(const RPointF &pos);
void setMove(glm::mat4 &mat, const RPointF &pos);

glm::mat4 rect(const RRectF &rect, float depth = 0);
void setRect(glm::mat4 &mat, const RRectF &rect, float depth = 0);
void setRectAs(glm::mat4 &mat, const RRectF &rect, float depth = 0);

} // ns math

} // ns glm

#endif // RMATHS_H
