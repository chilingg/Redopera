#include <RShaders.h>
#include <RDebug.h>

#include <stdexcept>

using namespace Redopera;

thread_local GLuint RRPI::current = 0;
thread_local int RRPI::count = 0;

RRPI::~RRPI()
{
    --count;
    if(count == 0)
    {
        glUseProgram(0);
        current = 0;
    }
}

void RRPI::setUniform(GLint loc, GLfloat v1) const
{
    glUniform1f(loc, v1);
}

void RRPI::setUniform(GLint loc, GLfloat v1, GLfloat v2) const
{
    glUniform2f(loc, v1, v2);
}

void RRPI::setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    glUniform3f(loc, v1, v2, v3);
}

void RRPI::setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const
{
    glUniform4f(loc, v1, v2, v3, v4);
}

void RRPI::setUniform(GLint loc, glm::vec3 vec) const
{
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void RRPI::setUniform(GLint loc, glm::vec4 vec) const
{
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void RRPI::setUniform(GLint loc, GLint v1) const
{
    glUniform1i(loc, v1);
}

void RRPI::setUniform(GLint loc, GLint v1, GLint v2) const
{
    glUniform2i(loc, v1, v2);
}

void RRPI::setUniform(GLint loc, GLint v1, GLint v2, GLint v3) const
{
    glUniform3i(loc, v1, v2, v3);
}

void RRPI::setUniform(GLint loc, GLint v1, GLint v2, GLint v3, GLint v4) const
{
    glUniform4i(loc, v1, v2, v3, v4);
}

void RRPI::setUniform(GLint loc, glm::ivec3 vec) const
{
    glUniform3i(loc, vec.x, vec.y, vec.z);
}

void RRPI::setUniform(GLint loc, glm::ivec4 vec) const
{
    glUniform4i(loc, vec.x, vec.y, vec.z, vec.w);
}

void RRPI::setUniform(GLint loc, GLuint v1) const
{
    glUniform1ui(loc, v1);
}

void RRPI::setUniform(GLint loc, GLuint v1, GLuint v2) const
{
    glUniform2ui(loc, v1, v2);
}

void RRPI::setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3) const
{
    glUniform3ui(loc, v1, v2, v3);
}

void RRPI::setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3, GLuint v4) const
{
    glUniform4ui(loc, v1, v2, v3, v4);
}

void RRPI::setUniform(GLint loc, glm::uvec3 vec) const
{
    glUniform3ui(loc, vec.x, vec.y, vec.z);
}

void RRPI::setUniform(GLint loc, glm::uvec4 vec) const
{
    glUniform4ui(loc, vec.x, vec.y, vec.z, vec.w);
}

void RRPI::setUniform(GLint loc, GLsizei size, GLfloat *vp, GLsizei count) const
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

void RRPI::setUniform(GLint loc, GLsizei size, GLint *vp, GLsizei count) const
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

void RRPI::setUniform(GLint loc, GLsizei size, GLuint *vp, GLsizei count) const
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

void RRPI::setUniform(GLint loc, glm::vec3 *vec, GLsizei count) const
{
    glUniform3fv(loc, count, &vec->x);
}

void RRPI::setUniform(GLint loc, glm::vec4 *vec, GLsizei count) const
{
    glUniform4fv(loc, count, &vec->x);
}

void RRPI::setUniform(GLint loc, glm::ivec3 *vec, GLsizei count) const
{
    glUniform3iv(loc, count, &vec->x);
}

void RRPI::setUniform(GLint loc, glm::ivec4 *vec, GLsizei count) const
{
    glUniform4iv(loc, count, &vec->x);
}

void RRPI::setUniform(GLint loc, glm::uvec3 *vec, GLsizei count) const
{
    glUniform3uiv(loc, count, &vec->x);
}

void RRPI::setUniform(GLint loc, glm::uvec4 *vec, GLsizei count) const
{
    glUniform4uiv(loc, count, &vec->x);
}

void RRPI::setUniformMat(GLint loc, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat2 &mat) const
{
    glUniformMatrix2dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat3 &mat) const
{
    glUniformMatrix3dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat4 &mat) const
{
    glUniformMatrix4dv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void RRPI::setUniformMat(GLint loc, const glm::mat2 *mat, GLsizei count) const
{
    glUniformMatrix2fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, const glm::mat3 *mat, GLsizei count) const
{
    glUniformMatrix3fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, const glm::mat4 *mat, GLsizei count) const
{
    glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat2 *mat, GLsizei count) const
{
    glUniformMatrix2dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat3 *mat, GLsizei count) const
{
    glUniformMatrix3dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, const glm::dmat4 *mat, GLsizei count) const
{
    glUniformMatrix4dv(loc, count, GL_FALSE, glm::value_ptr(*mat));
}

void RRPI::setUniformMat(GLint loc, GLsizei order, GLfloat *vp, GLsizei count, GLboolean transpose) const
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

void RRPI::setUniformMat(GLint loc, GLsizei order, GLdouble *vp, GLsizei count, GLboolean transpose) const
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

void RRPI::setUniformSubroutines(RShader::Type type, GLsizei count, const GLuint *indices)
{
    glUniformSubroutinesuiv(static_cast<GLenum>(type), count, indices);
}

void RRPI::release()
{
    if(count != 1)
         throw std::logic_error("The current thread has other <RRPI> is using!");
    current = 0;
}

RRPI::RRPI(GLuint id)
{
    if(current)
    {
        if(current != id)
            throw std::logic_error("The current thread has other <RRPI> is using!");
        else
            ++count;
    }
    else
    {
        current = id;
        ++count;
        glUseProgram(id);
    }
}

RShaders::RShaders(std::initializer_list<RShader> list)
{
    for(auto &shader : list)
        stages_.emplace(shader.type(), shader);
    linkProgram();
}

RShaders::RShaders(const RShaders &program):
    id_(program.id_),
    stages_(program.stages_)
{

}

RShaders::RShaders(const RShaders &&program):
    id_(std::move(program.id_)),
    stages_(std::move(program.stages_))
{

}

RShaders &RShaders::operator=(RShaders program)
{
    swap(program);
    return *this;
}

void RShaders::swap(RShaders &program)
{
    id_.swap(program.id_);
    stages_.swap(program.stages_);
}

bool RShaders::isValid() const
{
    return id_ != nullptr;
}

bool RShaders::isAttachedShader(RShader::Type type) const
{
    return stages_.count(type);
}

GLuint RShaders::id() const
{
    return *id_;
}

RRPI RShaders::use() const
{
    return RRPI(*id_);
}

GLint RShaders::getUniformLoc(const std::string &name) const
{
    GLint loc = glGetUniformLocation(*id_, name.c_str());
#ifndef NDEBUG
    rCheck(loc < 0, "Invalid locale <" + name + '>');
#endif
    return loc;
}

GLint RShaders::getSubroutineUniformLoc(RShader::Type type, const std::string &name) const
{
    GLint loc = glGetSubroutineUniformLocation(*id_, static_cast<GLenum>(type), name.c_str());
#ifndef NDEBUG
    rCheck(loc < 0, "Invalid subroutine locale <" + name + '>');
#endif
    return loc;
}

GLuint RShaders::getSubroutineIndex(RShader::Type type, const std::string &name) const
{
    GLuint i = glGetSubroutineIndex(*id_, static_cast<GLenum>(type), name.c_str());
#ifndef NDEBUG
    rCheck(i == GL_INVALID_INDEX, "Invalid subroutine index <" + name + '>');
#endif
    return i;
}

void RShaders::attachShader(const RShader &shader)
{
    stages_.emplace(shader.type(), shader);
}

void RShaders::attachShader(std::initializer_list<RShader> list)
{
    for(auto &shader : list)
        stages_.emplace(shader.type(), shader);
}

void RShaders::detachShader(RShader::Type type)
{
    stages_.erase(type);
}

bool RShaders::linkProgram()
{
    std::shared_ptr<GLuint> id(std::shared_ptr<GLuint>(new GLuint(glCreateProgram()), deleteShaderProgram));

    for(auto& shader: stages_)
        glAttachShader(*id, shader.second.id());
    glLinkProgram(*id);

    int success;
    glGetProgramiv(*id, GL_LINK_STATUS, &success);
    if(rCheck(!success, "Failed to link shader program"))
    {
        char infoLog[256];
        glGetProgramInfoLog(*id, sizeof(infoLog), nullptr, infoLog);
        rPrError(infoLog);
        return false;
    }

    id_.swap(id);
    releaseShader();
    return true;
}

void RShaders::reLinkProgram()
{
    glLinkProgram(*id_);
}

void RShaders::releaseShader()
{
    std::map<RShader::Type, RShader> temp;
    stages_.swap(temp);
}

void RShaders::release()
{
    id_.reset();
}

void RShaders::deleteShaderProgram(GLuint *id)
{
    glDeleteProgram(*id);
    delete id;
}

void swap(RShaders &prog1, RShaders &prog2)
{
    prog1.swap(prog2);
}
