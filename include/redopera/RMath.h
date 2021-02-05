#ifndef RMATHS_H
#define RMATHS_H

#include "dependent/glm/mat4x4.hpp"
#include "dependent/glm/gtc/matrix_transform.hpp"
#include "dependent/glm/gtc/type_ptr.hpp"

/* GLM算法和功能
 * min(genType x, genType y)   //返回最小值
 * max(genType x, genType y)   //返回最大值
 * abs(int x)                  //返回绝对值
 * round(genType x)            //四舍六入五取偶
 * trunc(genType x)            //返回截断整数部分
 */

namespace glm {

mat4 perspective(float left, float right, float bottom, float top, float near, float far);

} // ns glm

namespace Redopera {

class RDebug;

} // ns Redopera

const Redopera::RDebug& operator<<(const Redopera::RDebug &d, const glm::vec3 &vec);
const Redopera::RDebug& operator<<(const Redopera::RDebug &d, const glm::vec4 &vec);
const Redopera::RDebug& operator<<(const Redopera::RDebug &d, const glm::mat4 &mat);

#endif // RMATHS_H
