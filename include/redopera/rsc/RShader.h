#ifndef RSHADER_H
#define RSHADER_H

#include "RResource.h"
#include "ROpenGL.h"

#include <memory>

namespace Redopera {

class RShader
{
public:
    enum class Type
    {
        Vertex    = GL_VERTEX_SHADER,
        Fragment  = GL_FRAGMENT_SHADER,
        TessContol        = GL_TESS_CONTROL_SHADER,
        TessEvaluation    = GL_TESS_EVALUATION_SHADER,
        Geometry  = GL_GEOMETRY_SHADER,
        Compute   = GL_COMPUTE_SHADER
    };

    static const std::string& shaderTypeName(Type type);

    RShader() = default;
    RShader(const std::string &shader, Type type);
    RShader(const RShader &shader);
    RShader(const RShader &&shader);
    RShader& operator=(RShader shader);
    void swap(RShader &shader);
    ~RShader() = default;

    bool isValid() const;
    Type type() const;
    const std::string& typeName() const;
    GLuint shaderID() const;

    bool load(const std::string &shader, Type type);
    void release();

private:
    static void deleteShader(GLuint *id);

    std::shared_ptr<GLuint> shaderID_;
    Type type_;
};

} // Redopera

void swap(Redopera::RShader &shader1, Redopera::RShader &shader2);

#endif // RSHADER_H
