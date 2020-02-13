#include "rsc/RShaderProg.h"
#include "RDebug.h"

using namespace Redopera;

thread_local GLuint RInterface::current = 0;
thread_local int RInterface::count = 0;

RInterface::~RInterface()
{
    --count;
    if(!count)
    {
        glUseProgram(0);
        current = 0;
    }
}

void RInterface::setViewprot(GLint loc, float left, float right, float bottom, float top, float near, float far) const
{
    glm::mat4 projection = glm::ortho(left, right, bottom, top, near, far);
    setUniformMatrix(loc, 4, glm::value_ptr(projection));
}

void RInterface::setPerspective(GLint loc, float left, float right, float bottom, float top, float near, float far) const
{
    glm::mat4 projection = perspective(left, right, bottom, top, near, far);
    setUniformMatrix(loc, 4, glm::value_ptr(projection));
}

void RInterface::setCameraMove(GLint loc, float x, float y, float z) const
{
    glm::mat4 view = glm::translate(glm::mat4(1), {-x, -y, -z});
    setUniformMatrix(loc, 4, glm::value_ptr(view));
}

void RInterface::setUniform(GLint loc, GLfloat v1) const
{
    glUniform1f(loc, v1);
}

void RInterface::setUniform(GLint loc, GLfloat v1, GLfloat v2) const
{
    glUniform2f(loc, v1, v2);
}

void RInterface::setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    glUniform3f(loc, v1, v2, v3);
}

void RInterface::setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const
{
    glUniform4f(loc, v1, v2, v3, v4);
}

void RInterface::setUniform(GLint loc, glm::vec3 vec) const
{
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void RInterface::setUniform(GLint loc, glm::vec4 vec) const
{
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void RInterface::setUniform(GLint loc, GLint v1) const
{
    glUniform1i(loc, v1);
}

void RInterface::setUniform(GLint loc, GLint v1, GLint v2) const
{
    glUniform2i(loc, v1, v2);
}

void RInterface::setUniform(GLint loc, GLint v1, GLint v2, GLint v3) const
{
    glUniform3i(loc, v1, v2, v3);
}

void RInterface::setUniform(GLint loc, GLint v1, GLint v2, GLint v3, GLint v4) const
{
    glUniform4i(loc, v1, v2, v3, v4);
}

void RInterface::setUniform(GLint loc, glm::ivec3 vec) const
{
    glUniform3i(loc, vec.x, vec.y, vec.z);
}

void RInterface::setUniform(GLint loc, glm::ivec4 vec) const
{
    glUniform4i(loc, vec.x, vec.y, vec.z, vec.w);
}

void RInterface::setUniform(GLint loc, GLuint v1) const
{
    glUniform1ui(loc, v1);
}

void RInterface::setUniform(GLint loc, GLuint v1, GLuint v2) const
{
    glUniform2ui(loc, v1, v2);
}

void RInterface::setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3) const
{
    glUniform3ui(loc, v1, v2, v3);
}

void RInterface::setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3, GLuint v4) const
{
    glUniform4ui(loc, v1, v2, v3, v4);
}

void RInterface::setUniform(GLint loc, glm::uvec3 vec) const
{
    glUniform3ui(loc, vec.x, vec.y, vec.z);
}

void RInterface::setUniform(GLint loc, glm::uvec4 vec) const
{
    glUniform4ui(loc, vec.x, vec.y, vec.z, vec.w);
}

void RInterface::setUniform(GLint loc, GLsizei size, GLfloat *vp, GLsizei count) const
{
    switch(size) {
    case 1:
        glUniform1fv(loc, count, vp);
        break;
    case 2:
        glUniform2fv(loc, count, vp);
        break;
    case 3:
        glUniform3fv(loc, count, vp);
        break;
    case 4:
        glUniform4fv(loc, count, vp);
        break;
    default:
        throw std::invalid_argument("Set Invalid size argument for Uniform" + std::to_string(size) + "fv");
    }
}

void RInterface::setUniform(GLint loc, GLsizei size, GLint *vp, GLsizei count) const
{
    switch(size) {
    case 1:
        glUniform1iv(loc, count, vp);
        break;
    case 2:
        glUniform2iv(loc, count, vp);
        break;
    case 3:
        glUniform3iv(loc, count, vp);
        break;
    case 4:
        glUniform4iv(loc, count, vp);
        break;
    default:
        throw std::invalid_argument("Invalid size argument for Uniform" + std::to_string(size) + "iv");
        break;
    }
}

void RInterface::setUniform(GLint loc, GLsizei size, GLuint *vp, GLsizei count) const
{
    switch(size) {
    case 1:
        glUniform1uiv(loc, count, vp);
        break;
    case 2:
        glUniform2uiv(loc, count, vp);
        break;
    case 3:
        glUniform3uiv(loc, count, vp);
        break;
    case 4:
        glUniform4uiv(loc, count, vp);
        break;
    default:
        throw std::invalid_argument("Invalid size argument for Uniform" + std::to_string(size) + "uiv");
        break;
    }
}

void RInterface::setUniform(GLint loc, glm::vec3 *vec, GLsizei count) const
{
    glUniform3fv(loc, count, &vec->x);
}

void RInterface::setUniform(GLint loc, glm::vec4 *vec, GLsizei count) const
{
    glUniform4fv(loc, count, &vec->x);
}

void RInterface::setUniform(GLint loc, glm::ivec3 *vec, GLsizei count) const
{
    glUniform3iv(loc, count, &vec->x);
}

void RInterface::setUniform(GLint loc, glm::ivec4 *vec, GLsizei count) const
{
    glUniform4iv(loc, count, &vec->x);
}

void RInterface::setUniform(GLint loc, glm::uvec3 *vec, GLsizei count) const
{
    glUniform3uiv(loc, count, &vec->x);
}

void RInterface::setUniform(GLint loc, glm::uvec4 *vec, GLsizei count) const
{
    glUniform4uiv(loc, count, &vec->x);
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat2 &mat) const
{
    glUniformMatrix2dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat3 &mat) const
{
    glUniformMatrix3dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat4 &mat) const
{
    glUniformMatrix4dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat2 *mat, GLsizei count) const
{
    glUniformMatrix2fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat3 *mat, GLsizei count) const
{
    glUniformMatrix3fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::mat4 *mat, GLsizei count) const
{
    glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat2 *mat, GLsizei count) const
{
    glUniformMatrix2dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat3 *mat, GLsizei count) const
{
    glUniformMatrix3dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, const glm::dmat4 *mat, GLsizei count) const
{
    glUniformMatrix4dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RInterface::setUniformMatrix(GLint loc, GLsizei order, GLfloat *vp, GLsizei count, GLboolean transpose) const
{
    switch(order) {
    case 2:
        glUniformMatrix2fv(loc, count, transpose, vp);
        break;
    case 3:
        glUniformMatrix3fv(loc, count, transpose, vp);
        break;
    case 4:
        glUniformMatrix4fv(loc, count, transpose, vp);
        break;
    default:
        throw std::invalid_argument("Invalid size argument for UniformMatrix" + std::to_string(order) + "fv");
        break;
    }
}

void RInterface::setUniformMatrix(GLint loc, GLsizei order, GLdouble *vp, GLsizei count, GLboolean transpose) const
{
    switch(order) {
    case 2:
        glUniformMatrix2dv(loc, count, transpose, vp);
        break;
    case 3:
        glUniformMatrix3dv(loc, count, transpose, vp);
        break;
    case 4:
        glUniformMatrix4dv(loc, count, transpose, vp);
        break;
    default:
        throw std::invalid_argument("Invalid size argument for UniformMatrix" + std::to_string(order) + "dv");
        break;
    }
}

void RInterface::reset(GLuint id)
{
    if(count != 1 && id != current)
         throw std::logic_error("The current thread has other <Interfaces> is using!");
    current = id;
}

RInterface::RInterface(GLuint id)
{
    if(current && current != id) throw std::logic_error("The current thread has other <Interfaces> is using!");
    current = id;
    ++count;
    glUseProgram(id);
}

RShaderProg::RShaderProg():
    RResource("ShaderProgram", Type::ShaderProg)
{

}

RShaderProg::RShaderProg(std::initializer_list<RShader> list, const std::string &name):
    RResource(name, Type::ShaderProg)
{
    for(auto &shader : list)
        shaders_.emplace(shader.type(), shader);
    linkProgram();
}

RShaderProg::RShaderProg(const RShaderProg &program):
    RResource(program),
    progID_(program.progID_),
    shaders_(program.shaders_)
{

}

RShaderProg::RShaderProg(const RShaderProg &&program):
    RResource(std::move(program)),
    progID_(std::move(program.progID_)),
    shaders_(std::move(program.shaders_))
{

}

RShaderProg &RShaderProg::operator=(RShaderProg program)
{
    swap(program);
    return *this;
}

void RShaderProg::swap(RShaderProg &program)
{
    RResource::swap(program);
    progID_.swap(program.progID_);
    shaders_.swap(program.shaders_);
}

bool RShaderProg::isValid() const
{
    return progID_ != nullptr;
}

bool RShaderProg::isAttachShader(RShader::Type type) const
{
    return shaders_.count(type);
}

GLuint RShaderProg::shaderProgramID() const
{
    return *progID_;
}

RInterface RShaderProg::useInterface() const
{
    return RInterface(*progID_);
}

GLint RShaderProg::getUniformLocation(const std::string &name) const
{
    GLint loc = glGetUniformLocation(*progID_, name.c_str());
    check(loc < 0, "Invalid locale <" + name + '>');
    return loc;
}

void RShaderProg::attachShader(std::initializer_list<RShader> list)
{
    for(auto &shader : list)
        shaders_.emplace(shader.type(), shader);
}

void RShaderProg::detachShader(RShader::Type type)
{
    shaders_.erase(type);
}

bool RShaderProg::linkProgram()
{
    std::shared_ptr<GLuint> id(std::shared_ptr<GLuint>(new GLuint(glCreateProgram()), deleteShaderProgram));

    for(auto shader: shaders_)
        glAttachShader(*id, shader.second.shaderID());
    glLinkProgram(*id);

    int success;
    glGetProgramiv(*id, GL_LINK_STATUS, &success);
    if(check(!success, "Failed to link shader program <" + name() + '>'))
    {
        char infoLog[256];
        glGetProgramInfoLog(*id, sizeof(infoLog), nullptr, infoLog);
        prError(infoLog);
        return false;
    }

    if(!progID_.unique())
        resetRscID();
    progID_.swap(id);
    return true;
}

void RShaderProg::reLinkProgram()
{
    glLinkProgram(*progID_);
}

void RShaderProg::releaseShader()
{
    std::map<RShader::Type, RShader> temp;
    shaders_.swap(temp);
}

void RShaderProg::release()
{
    releaseShader();
    progID_.reset();
}

void RShaderProg::deleteShaderProgram(GLuint *id)
{
    glDeleteProgram(*id);
    delete id;
}

void swap(RShaderProg &prog1, RShaderProg &prog2)
{
    prog1.swap(prog2);
}

void* Redopera::bufOff(size_t off)
{
    return reinterpret_cast<void*>(off);
}

glm::mat4 Redopera::perspective(float left, float right, float bottom, float top, float near, float far)
{
    static float accuracy = 0.95; // 投影之后far平面拥有的最大z轴标量

    glm::mat4 mat(1);
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
