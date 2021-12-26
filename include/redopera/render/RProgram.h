#ifndef RSHADERS_H
#define RSHADERS_H

#include "RShader.h"
#include "../RMath.h"

#include <initializer_list>

namespace Redopera {

class RProgram;

class RRPI
{
    friend RProgram;

public:
    ~RRPI();

    void setUniform(GLint loc, GLfloat v1) const;
    void setUniform(GLint loc, GLfloat v1, GLfloat v2) const;
    void setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3) const;
    void setUniform(GLint loc, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const;

    void setUniform(GLint loc, glm::vec3 vec) const;
    void setUniform(GLint loc, glm::vec4 vec) const;

    void setUniform(GLint loc, GLint v1) const;
    void setUniform(GLint loc, GLint v1, GLint v2) const;
    void setUniform(GLint loc, GLint v1, GLint v2, GLint v3) const;
    void setUniform(GLint loc, GLint v1, GLint v2, GLint v3, GLint v4) const;

    void setUniform(GLint loc, glm::ivec3 vec) const;
    void setUniform(GLint loc, glm::ivec4 vec) const;

    void setUniform(GLint loc, GLuint v1) const;
    void setUniform(GLint loc, GLuint v1, GLuint v2) const;
    void setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3) const;
    void setUniform(GLint loc, GLuint v1, GLuint v2, GLuint v3, GLuint v4) const;

    void setUniform(GLint loc, glm::uvec3 vec) const;
    void setUniform(GLint loc, glm::uvec4 vec) const;

    void setUniform(GLint loc, GLsizei size, GLfloat *vp, GLsizei count) const;
    void setUniform(GLint loc, GLsizei size, GLint *vp, GLsizei count) const;
    void setUniform(GLint loc, GLsizei size, GLuint *vp, GLsizei count) const;

    void setUniform(GLint loc, glm::vec3 *vec, GLsizei count) const;
    void setUniform(GLint loc, glm::vec4 *vec, GLsizei count) const;
    void setUniform(GLint loc, glm::ivec3 *vec, GLsizei count) const;
    void setUniform(GLint loc, glm::ivec4 *vec, GLsizei count) const;
    void setUniform(GLint loc, glm::uvec3 *vec, GLsizei count) const;
    void setUniform(GLint loc, glm::uvec4 *vec, GLsizei count) const;

    void setUniformMat(GLint loc, const glm::mat2 &mat) const;
    void setUniformMat(GLint loc, const glm::mat3 &mat) const;
    void setUniformMat(GLint loc, const glm::mat4 &mat) const;
    void setUniformMat(GLint loc, const glm::dmat2 &mat) const;
    void setUniformMat(GLint loc, const glm::dmat3 &mat) const;
    void setUniformMat(GLint loc, const glm::dmat4 &mat) const;

    void setUniformMat(GLint loc, const glm::mat2 *mat, GLsizei count) const;
    void setUniformMat(GLint loc, const glm::mat3 *mat, GLsizei count) const;
    void setUniformMat(GLint loc, const glm::mat4 *mat, GLsizei count) const;
    void setUniformMat(GLint loc, const glm::dmat2 *mat, GLsizei count) const;
    void setUniformMat(GLint loc, const glm::dmat3 *mat, GLsizei count) const;
    void setUniformMat(GLint loc, const glm::dmat4 *mat, GLsizei count) const;

    void setUniformMat(GLint loc, GLsizei order, GLfloat *vp, GLsizei count = 1, GLboolean transpose = false) const;
    void setUniformMat(GLint loc, GLsizei order, GLdouble *vp, GLsizei count = 1, GLboolean transpose = false) const;

    void setUniformSubroutines(RShader::Type type, GLsizei count, const GLuint *indices);

    void release();

private:
    RRPI(GLuint id);

    thread_local static GLuint current;
    thread_local static int count;
};

class RProgram
{
public:
    RProgram() = default;
    RProgram(std::initializer_list<RShader> list);
    RProgram(const RProgram &program);
    RProgram(const RProgram &&program);
    RProgram& operator=(RProgram program);
    ~RProgram() = default;
    void swap(RProgram &program);

    bool isValid() const;
    GLuint id() const;
    RRPI use() const;

    GLint getUniformLoc(const char *name) const;
    GLint getSubroutineUniformLoc(RShader::Type type, const std::string &name) const;
    GLuint getSubroutineIndex(RShader::Type type, const std::string &name) const;

    bool linkProgram(std::initializer_list<RShader> list);
    void reLinkProgram();
    void free();

private:
    std::shared_ptr<GLuint> id_;
};

} // Redopera

void swap(Redopera::RProgram &prog1, Redopera::RProgram&prog2);

#endif // RSHADERS_H
