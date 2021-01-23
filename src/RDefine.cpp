#include <rsc/RResource.h>
#include <RGame.h>
#include <RDebug.h>
#include <RMath.h>

using namespace Redopera;

// RResource.h
std::string RResource::rscPath_ = "./";

bool RGame::init = false;

const RDebug& Redopera::operator<<(const RDebug &d, const glm::vec3 &vec)
{
    std::string str('(' + std::to_string(vec.x) + ", "
                    + std::to_string(vec.y) + ", "
                    + std::to_string(vec.z) + ") ");

    return d.operator<<(str);
}

const RDebug& Redopera::operator<<(const RDebug &d, const glm::vec4 &vec)
{
    std::string str('(' + std::to_string(vec.x) + ", "
                    + std::to_string(vec.y) + ", "
                    + std::to_string(vec.z) + ", "
                    + std::to_string(vec.w) + ") ");

    return d.operator<<(str);
}

const RDebug& Redopera::operator<<(const RDebug &d, const glm::mat4 &mat)
{
    char buf[256];
    std::snprintf(buf, sizeof(buf),
                  "mat:(%10.3f, %10.3f, %10.3f, %10.3f)\n"
                  "    (%10.3f, %10.3f, %10.3f, %10.3f)\n"
                  "    (%10.3f, %10.3f, %10.3f, %10.3f)\n"
                  "    (%10.3f, %10.3f, %10.3f, %10.3f). ",
                  mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                  mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                  mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                  mat[0][3], mat[1][3], mat[2][3], mat[3][3]);

    return d.operator<<(buf);
}

glm::mat4 glm::perspective(float left, float right, float bottom, float top, float near, float far)
{
    static float accuracy = 0.95; // 投影之后far平面拥有的最大z轴标量

    mat4 mat(1);
    float len = far - near;

    // 缩放
    mat[0][0] = 1 / (right - left) * 2;
    mat[1][1] = 1 / (top - bottom) * 2;
    // z轴翻转与投影
    mat[2][3] = -1 / ((2*accuracy*len - 2*len) / (1+accuracy));
    mat[2][2] = mat[2][3];
    // 左乘以平移矩阵
    mat[3][0] = mat[0][0] * -(left + (right - left) / 2);
    mat[3][1] = mat[1][1] * -(bottom + (top - bottom) / 2);
    mat[3][2] = mat[2][2] * -near;
    mat[3][3] = mat[3][2] + 1;

    return mat;
}
