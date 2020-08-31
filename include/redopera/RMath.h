#ifndef RMATHS_H
#define RMATHS_H

#include "dependents/glm/mat4x4.hpp"
#include "dependents/glm/gtc/matrix_transform.hpp"
#include "dependents/glm/gtc/type_ptr.hpp"

/* GLM算法和功能
 * min(genType x, genType y)   //返回最小值
 * max(genType x, genType y)   //返回最大值
 * abs(int x)                  //返回绝对值
 * round(genType x)            //四舍六入五取偶
 * trunc(genType x)            //返回截断整数部分
 */

namespace glm {

mat4 perspective(float left, float right, float bottom, float top, float near, float far);

}

#include <string>

std::string toStdString(glm::vec3 vec);
std::string toStdString(glm::vec4 vec);
std::string toStdString(glm::mat4 vec);

#endif // RMATHS_H
