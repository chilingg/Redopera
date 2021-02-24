#ifndef RSHADERS_H
#define RSHADERS_H

#include "rsc/RShader.h"
#include "RMath.h"
#include "RName.h"

#include <initializer_list>
#include <map>

namespace Redopera {

class RShaders;

class RRPI
{
    friend RShaders;

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

class RShaders
{
public:
    RShaders() = default;
    RShaders(std::initializer_list<RShader> list);
    RShaders(const RShaders &program);
    RShaders(const RShaders &&program);
    RShaders& operator=(RShaders program);
    ~RShaders() = default;
    void swap(RShaders &program);

    bool isValid() const;
    bool isAttachedShader(RShader::Type typr) const;
    GLuint id() const;
    RRPI use() const;

    GLint getUniformLoc(const RName &name) const;
    GLint getSubroutineUniformLoc(RShader::Type type, const RName &name) const;
    GLuint getSubroutineIndex(RShader::Type type, const RName &name) const;

    // attachShader() 与 detachShader() 都是只在重新linkProgram时生效
    void attachShader(const RShader &shader);
    void attachShader(std::initializer_list<RShader> list);
    void detachShader(RShader::Type type);
    bool linkProgram();
    void reLinkProgram();
    void releaseShader();
    void release();

private:
    static void deleteShaderProgram(GLuint *ID);

    std::shared_ptr<GLuint> id_;
    std::map<RShader::Type, RShader> stages_;
};

} // Redopera

void swap(Redopera::RShaders &prog1, Redopera::RShaders&prog2);

#endif // RSHADERS_H
