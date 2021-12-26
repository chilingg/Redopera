#include <render/RShader.h>
#include <rsc/RFile.h>

using namespace Redopera;

namespace  {

void deleteShader(GLuint *id)
{
    glDeleteShader(*id);
    delete id;
}

} // ns

std::string_view RShader::shaderTypeName(Type type)
{
    static const std::string vetex      = "VertexShader";
    static const std::string fragment   = "FragmentShader";
    static const std::string tessc      = "TessContolShader";
    static const std::string tesse      = "TessEvaluationShader";
    static const std::string geometry   = "GeometryShader";
    static const std::string compute    = "ComputeShader";

    switch(type)
    {
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

RShader::RShader(const std::string &shader, Type type)
{
    load(shader, type);
}

RShader::RShader(const RShader &shader):
    shaderID_(shader.shaderID_),
    type_(shader.type_)
{

}

RShader::RShader(const RShader &&shader):
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

std::string_view RShader::typeName() const
{
    return shaderTypeName(type_);
}

GLuint RShader::id() const
{
    return *shaderID_;
}

bool RShader::load(const std::string &shader, Type type)
{
    const char *code;
    std::unique_ptr<RData[]> file;

    if(shader.find('\n') == std::string::npos)
    {
        file = RFile::load(shader);
        if (!file)
        {
            rError("Failed to load shader file {}!\n", shader);
            return false;
        }
        else
            code = reinterpret_cast<char*>(file.get());
    }
    else
        code = shader.c_str();

    std::shared_ptr<GLuint> id(new GLuint(glCreateShader(static_cast<GLenum>(type))), deleteShader);
    glShaderSource(*id, 1, &code, nullptr);
    glCompileShader(*id);

    int success;
    glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
    if(rCheck(!success, "Failed to compile shader {}: {}\n", shaderTypeName(type), shader))
    {
        char infoLog[256];
        glGetShaderInfoLog(*id, sizeof(infoLog), nullptr, infoLog);
        rError(infoLog);
        return false;
    }

    type_ = type;
    shaderID_.swap(id);
    return true;
}

void RShader::free()
{
    shaderID_.reset();
}

void swap(RShader &shader1, RShader &shader2)
{
    shader1.swap(shader2);
}
