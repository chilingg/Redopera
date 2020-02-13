#include "rsc/RShader.h"
#include "RDebug.h"

using namespace Redopera;

const std::string &RShader::shaderTypeName(Type type)
{
    static const std::string vetex      = "VertexShader";
    static const std::string fragment   = "FragmentShader";
    static const std::string tessc      = "TessContolShader";
    static const std::string tesse      = "TessEvaluationShader";
    static const std::string geometry   = "GeometryShader";
    static const std::string compute    = "ComputeShader";

    switch(type) {
    case Type::Vertex:
        return vetex;
    case Type::Fragment:
        return fragment;
    case Type::TessContol:
        return tessc;
    case Type::TessEvaluation:
        return tesse;
    case Type::Geometry:
        return geometry;
    case Type::Compute:
        return compute;
    }

    return compute;
}

RShader::RShader():
    RResource("Shader", RResource::Type::Shader)
{

}

RShader::RShader(const std::string &shader, Type type, const std::string &name):
    RResource(name, RResource::Type::Shader)
{
    load(shader, type);
}

RShader::RShader(const RShader &shader):
    RResource(shader),
    shaderID_(shader.shaderID_),
    type_(shader.type_)
{

}

RShader::RShader(const RShader &&shader):
    RResource(std::move(shader)),
    shaderID_(std::move(shader.shaderID_)),
    type_(shader.type_)
{

}

RShader &RShader::operator=(RShader shader)
{
    swap(shader);
    return *this;
}

void RShader::swap(RShader &shader)
{
    RResource::swap(shader);
    shaderID_.swap(shader.shaderID_);
    using std::swap;
    swap(type_, shader.type_);
}

bool RShader::isValid() const
{
    return shaderID_ != nullptr;
}

RShader::Type RShader::type() const
{
    return type_;
}

const std::string &RShader::typeName() const
{
    return shaderTypeName(type_);
}

GLuint RShader::shaderID() const
{
    return *shaderID_;
}

bool RShader::load(const std::string &shader, Type type)
{
    std::string code = getTextFileContent(shader);
    if(code.empty())
        code = shader;

    std::shared_ptr<GLuint> id(new GLuint(glCreateShader(static_cast<GLenum>(type))), deleteShader);
    const GLchar *cp = code.c_str();
    glShaderSource(*id, 1, &cp, nullptr);
    glCompileShader(*id);

    int success;
    glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
    if(check(!success, "Failed to load " + shaderTypeName(type) + '<' + name() + "> in:\n" + shader))
    {
        char infoLog[256];
        glGetShaderInfoLog(*id, sizeof(infoLog), nullptr, infoLog);
        prError(infoLog);
        return false;
    }

    type_ = type;
    if(!shaderID_.unique())
        resetRscID();
    shaderID_.swap(id);
    return true;
}

void RShader::release()
{
    shaderID_.reset();
}

void RShader::deleteShader(GLuint *id)
{
    glDeleteShader(*id);
    delete id;
}

void swap(RShader &shader1, RShader &shader2)
{
    shader1.swap(shader2);
}
