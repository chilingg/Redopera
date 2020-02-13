#ifndef RSHADERPROG_H
#define RSHADERPROG_H

#include "RShader.h"
#include "../RMath.h"

#include <initializer_list>

namespace Redopera {

class RShaderProg;

class RInterface
{
    friend RShaderProg;

public:
    ~RInterface();

    void setViewprot(GLint loc, float left, float right, float bottom, float top, float near = -127.0f, float far = 128.0f) const;
    void setPerspective(GLint loc, float left, float right, float bottom, float top, float near, float far) const;
    void setCameraMove(GLint loc, float x, float y, float z = 0) const;

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

    void setUniformMatrix(GLint loc, const glm::mat2 &mat) const;
    void setUniformMatrix(GLint loc, const glm::mat3 &mat) const;
    void setUniformMatrix(GLint loc, const glm::mat4 &mat) const;
    void setUniformMatrix(GLint loc, const glm::dmat2 &mat) const;
    void setUniformMatrix(GLint loc, const glm::dmat3 &mat) const;
    void setUniformMatrix(GLint loc, const glm::dmat4 &mat) const;

    void setUniformMatrix(GLint loc, const glm::mat2 *mat, GLsizei count) const;
    void setUniformMatrix(GLint loc, const glm::mat3 *mat, GLsizei count) const;
    void setUniformMatrix(GLint loc, const glm::mat4 *mat, GLsizei count) const;
    void setUniformMatrix(GLint loc, const glm::dmat2 *mat, GLsizei count) const;
    void setUniformMatrix(GLint loc, const glm::dmat3 *mat, GLsizei count) const;
    void setUniformMatrix(GLint loc, const glm::dmat4 *mat, GLsizei count) const;

    void setUniformMatrix(GLint loc, GLsizei order, GLfloat *vp, GLsizei count = 1, GLboolean transpose = false) const;
    void setUniformMatrix(GLint loc, GLsizei order, GLdouble *vp, GLsizei count = 1, GLboolean transpose = false) const;

    void reset(GLuint id);

private:
    RInterface(GLuint id);

    thread_local static GLuint current;
    thread_local static int count;
};

class RShaderProg : public RResource
{
    friend void swap(RShaderProg &prog1, RShaderProg&prog2);

public:
    RShaderProg();
    RShaderProg(std::initializer_list<RShader> list, const std::string &name = "ShaderProgram");
    RShaderProg(const RShaderProg &program);
    RShaderProg(const RShaderProg &&program);
    RShaderProg& operator=(RShaderProg program);
    ~RShaderProg() = default;
    void swap(RShaderProg &program);

    bool isValid() const;
    bool isAttachShader(RShader::Type typr) const;
    GLuint shaderProgramID() const;
    RInterface useInterface() const;

    GLint getUniformLocation(const std::string &name) const;

    // attachShader() 与 detachShader() 都是只在重新linkProgram时生效
    void attachShader(std::initializer_list<RShader> list);
    void detachShader(RShader::Type type);
    bool linkProgram();
    void reLinkProgram();
    void releaseShader();
    void release();

private:
    static void deleteShaderProgram(GLuint *ID);

    std::shared_ptr<GLuint> progID_;
    std::map<RShader::Type, RShader> shaders_;
};

} // Redopera

void swap(Redopera::RShaderProg &prog1, Redopera::RShaderProg&prog2);

#endif // RSHADERPROG_H
