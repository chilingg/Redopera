#include "RPlane.h"
#include "RKeeper.h"

using namespace Redopera;

thread_local RShaderProg RPlane::tPlaneShaders;
thread_local GLuint RPlane::MODEL_LOC;
thread_local GLuint RPlane::EDGING_LOC;

const RPlane::RenderTool& RPlane::planeRenderTool()
{
    thread_local static std::unique_ptr<GLuint[], std::function<void(GLuint *p)>> vao(new GLuint[2]{0}, [](GLuint *p){
    glDeleteVertexArrays(2, p); delete [] p; });
    thread_local static std::unique_ptr<GLuint[], std::function<void(GLuint *p)>> vbo(new GLuint[2]{0}, [](GLuint *p){
    glDeleteBuffers(2, p); delete [] p; });

    if(!vao[0])
    {
        glGenVertexArrays(2, vao.get());
        glGenBuffers(2, vbo.get());

        float plane[24]{
                0.5f,-0.5f, 0.0f, 1.0f, 0.0f,//右下
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f,//右上
               -0.5f,-0.5f, 0.0f, 0.0f, 0.0f,//左下
               -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,//左上
        };
        glBindVertexArray(vao[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), bufOff(3*sizeof(float)));
        glBindVertexArray(0);

        float edge[12]{
                1.0f, 0.0f, 0.0f,//右下
                1.0f, 1.0f, 0.0f,//右上
                0.0f, 1.0f, 0.0f,//左上
                0.0f, 0.0f, 0.0f,//左下
        };
        glBindVertexArray(vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(edge), edge, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
        glBindVertexArray(0);
    }

    if(!tPlaneShaders.isValid())
    {
        static const GLchar *vCode = {
            "#version 330 core\n"
            "layout(location = 0) in vec3 aPos;\n"
            "layout(location = 1) in vec2 aTexCoor;\n"
            "uniform vec4 edging;\n"
            "uniform mat4 model[3];\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "out vec4 texCoor; // 末位检测是否渲染边框\n"
            "void main(void)\n"
            "{\n"
                "gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos, 1.0);\n"
                "if(edging == vec4(0, 0, 0, 0))\n"
                    "texCoor = vec4(aTexCoor, 0, 0.0);\n"
                "else if(edging == vec4(0, 0, 0, 1))"
                "{\n"
                    "if(gl_InstanceID == 0)\n"
                        "texCoor = vec4(1.0, 0, 0, 1.0);\n"
                    "else if(gl_InstanceID == 1)\n"
                        "texCoor = vec4(0, 1.0, 0, 1.0);\n"
                    "else\n"
                        "texCoor = vec4(0, 0, 1.0, 1.0);\n"
                "}\n"
                "else\n"
                    "texCoor = edging;\n"
            "}\n"
        };
        static const GLchar *fCode = {
            "#version 330 core\n"
            "in vec4 texCoor; // 边框渲染时用作颜色值\n"
            "out vec4 outColor;\n"
            "uniform sampler2D tex;\n"
            "void main(void)\n"
            "{\n"
                "if(texCoor.a == 0)\n"
                    "outColor = texture(tex, texCoor.st);\n"
                "else\n"
                    "outColor = texCoor;\n"
            "}\n"
        };

        tPlaneShaders.releaseShader();
        tPlaneShaders.rename("PlaneShaders");
        tPlaneShaders.attachShader({ RShader(vCode, RShader::Type::Vertex, "Plane-VS"),
                                     RShader(fCode, RShader::Type::Fragment, "Plane-FS") });
        tPlaneShaders.linkProgram();
        auto inter = tPlaneShaders.useInterface();
        MODEL_LOC = tPlaneShaders.getUniformLocation("model");
        EDGING_LOC = tPlaneShaders.getUniformLocation("edging");
        inter.setCameraMove(tPlaneShaders.getUniformLocation("view"), 0, 0, 0);
    }

    thread_local static RenderTool tool { tPlaneShaders, vao[0], MODEL_LOC, vao[1], EDGING_LOC };

    return tool;
}

void RPlane::setPlaneShadersAsThread(const RShaderProg &shaders, GLuint mLoc, GLuint eLoc)
{
    tPlaneShaders = shaders;
    MODEL_LOC = mLoc;
    EDGING_LOC = eLoc;
}

const RShaderProg &RPlane::planeShader()
{
    return planeRenderTool().shaders;
}

RPlane::RPlane():
    name_("Plane"),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(RTexture::whiteTex())
{
    texture_.rename(name_ + "-Tex");
}

RPlane::RPlane(int width, int height, int x, int y, int z, const RTexture &tex, const std::string &name):
    RArea(width, height, x, y, z),
    name_(name),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(tex)
{
    texture_.rename(name + "-Tex");
}

RPlane::RPlane(int width, int height, const RPoint &pos, const RTexture &tex, const std::string &name):
    RArea(width, height, pos),
    name_(name),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(tex)
{
    texture_.rename(name + "-Tex");
}

RPlane::RPlane(const RSize &size, const RPoint &pos, const RTexture &tex, const std::string &name):
    RArea(size, pos),
    name_(name),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(tex)
{
    texture_.rename(name + "-Tex");
}

RPlane::RPlane(const RRect &rect, int z, const RTexture &tex, const std::string &name):
    RArea(rect, z),
    name_(name),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(tex)
{
    texture_.rename(name + "-Tex");
}

RPlane::RPlane(const RArea::Format &format, const RTexture &tex, const std::string &name):
    RArea(format),
    name_(name),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1),
    texture_(tex)
{
    texture_.rename(name + "-Tex");
}

RPlane::RPlane(const RPlane &plane):
    RArea(plane),
    name_(plane.name_),
    mats_(plane.mats_),
    model_(plane.model_),
    texture_(plane.texture_)
{
    texture_.rename(name_ + "-Tex");
}

RPlane::RPlane(const RPlane &&plane):
    RArea(plane),
    name_(plane.name_),
    mats_(std::move(plane.mats_)),
    model_(std::move(plane.model_)),
    texture_(std::move(plane.texture_))
{
    texture_.rename(name_ + "-Tex");
}

RPlane &RPlane::operator=(const RPlane &plane)
{
    RArea::operator=(plane);
    name_ = plane.name_;
    mats_ = plane.mats_;
    model_ = plane.model_;
    texture_ = plane.texture_;
    texture_.rename(name_ + "-Tex");
    return *this;
}

RPlane &RPlane::operator=(const RPlane &&plane)
{
    RArea::operator=(plane);
    name_ = std::move(plane.name_);
    mats_ = std::move(plane.mats_);
    model_ = std::move(plane.model_);
    texture_ = std::move(plane.texture_);
    texture_.rename(name_ + "-Tex");
    return *this;
}

const glm::mat4 &RPlane::modelMat() const
{
    return model_;
}

const RPlane::ModelMat &RPlane::modelMats() const
{
    return mats_;
}

const RTexture &RPlane::texture() const
{
    return texture_;
}

const std::string &RPlane::name() const
{
    return name_;
}

void RPlane::setColorTexture(const RColor &color)
{
    R_RGBA rgba = color.rgba();
    setColorTexture(rgba);
}

void RPlane::setColorTexture(R_RGBA rgba)
{
    const RData *colorData = reinterpret_cast<RData*>(&rgba);
    texture_.load(colorData, 1, 1, 4, RTexture::NearestTex);
}

void RPlane::setColorTexture(unsigned r, unsigned g, unsigned b, unsigned a)
{
    RColor color(r, g, b, a);
    setColorTexture(color.rgba());
}

void RPlane::setTexture(const RImage &img)
{
    texture_.load(img);
}

void RPlane::setTexture(const RTexture &tex)
{
    texture_ = tex;
}

void RPlane::rename(std::string name)
{
    name_.swap(name);
    texture_.rename(name_ + "-Tex");
}

void RPlane::update()
{
    if(dirty() & (RArea::Scale | RArea::Typeset))
    {
        float w, h, tw = texture_.width(), th = texture_.height();
        float min = innerWidth() / tw;
        float max = innerHeight() / th;
        if(min > max) std::swap(min, max);

        switch(area().mode)
        {
        case RArea::Mode::Fix:
            w = tw;
            h = th;
            break;
        case RArea::Mode::Auto:
            w = innerWidth();
            h = innerHeight();
            break;
        case RArea::Mode::Cover:
            w = tw * max;
            h = th * max;
            break;
        case RArea::Mode::Contain:
            w = tw * min;
            h = th * min;
            break;
        }

        float x, y;
        switch(area().align.h)
        {
        case RArea::Align::Left:
            x = w / 2.0f + area().padding.l;
            break;
        case RArea::Align::Right:
            x = area().size.width() - w/2.0f - area().padding.r;
            break;
        default: // RArea::Align::Mind
            x = area().size.width() / 2.0f;
            break;
        }
        switch(area().align.v)
        {
        case RArea::Align::Bottom:
            y = h / 2.0f + area().padding.b;
            break;
        case RArea::Align::Top:
            y = area().size.height() - h/2.0f - area().padding.t;
            break;
        default: // RArea::Align::Mind
            y = area().size.height() / 2.0f;
            break;
        }

        mats_.tran = glm::translate(glm::mat4(1), { x, y, 0 });
        mats_.scale = glm::scale(glm::mat4(1), { w, h, 0.0f });

        if(area().flipH)
        {
            mats_.scale[0][0] *= -1;
            mats_.scale[0][1] *= -1;
            mats_.scale[0][2] *= -1;
            mats_.scale[0][3] *= -1;
        }
        if(area().flipV)
        {
            mats_.scale[1][0] *= -1;
            mats_.scale[1][1] *= -1;
            mats_.scale[1][2] *= -1;
            mats_.scale[1][3] *= -1;
        }
    }

    if(dirty() & RArea::Rotate)
        mats_.rotate = glm::mat4_cast(glm::qua<float>(glm::vec3{ area().rotate.x, area().rotate.y, area().rotate.z }));

    glm::mat4 move(1);
    move = glm::translate(move, { pos().x(), pos().y(), pos().z()});

    model_ = move * mats_.tran * mats_.rotate * mats_.scale;
    clearDirty();
}

void RPlane::render()
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.vao);

    RInterface inter = rt.shaders.useInterface();
    inter.setUniform(rt.edgingLoc, .0f, .0f, .0f, .0f);
    renderControl(rt.shaders, rt.modelLoc);

    glBindVertexArray(0);
}

void RPlane::render(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.vao);

    RInterface inter = shaders.useInterface();
    renderControl(shaders, mLoc);

    glBindVertexArray(0);
}

void RPlane::edging(const RColor &color)
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { area().pos.x(), area().pos.y(), area().pos.z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    RInterface inter = rt.shaders.useInterface();
    inter.setUniformMatrix(rt.modelLoc, mat);
    inter.setUniform(rt.edgingLoc, color.r()/255.f, color.g()/255.f, color.b()/255.f, 1.0f);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RPlane::edging(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { area().pos.x(), area().pos.y(), area().pos.z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, mat);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RPlane::edgingAll()
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mats[3] { glm::mat4(1), glm::mat4(1), glm::mat4(1)};

    mats[0] = glm::translate(mats[0], { area().pos.x(), area().pos.y(), area().pos.z() });
    mats[0] = glm::scale(mats[0], { width(), height(), 0 });
    mats[1] = glm::translate(mats[1], { innerPos().x(), innerPos().y(), area().pos.z() });
    mats[1] = glm::scale(mats[1], { innerWidth(), innerHeight() , 0 });
    mats[2] = glm::translate(mats[2], { outerPos().x(), outerPos().y(), area().pos.z() });
    mats[2] = glm::scale(mats[2], { outerWidth(), outerHeight() , 0 });

    RInterface inter = rt.shaders.useInterface();
    inter.setUniformMatrix(rt.modelLoc, mats, 3);
    inter.setUniform(rt.edgingLoc, .0f, .0f, .0f, 1.0f);

    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, 3);
    glBindVertexArray(0);
}

void RPlane::edgingAll(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    const RenderTool& rt = planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mats[3] { glm::mat4(1), glm::mat4(1), glm::mat4(1)};

    mats[0] = glm::translate(mats[0], { area().pos.x(), area().pos.y(), area().pos.z() });
    mats[0] = glm::scale(mats[0], { width(), height(), 0 });
    mats[1] = glm::translate(mats[1], { innerPos().x(), innerPos().y(), area().pos.z() });
    mats[1] = glm::scale(mats[1], { innerWidth(), innerHeight() , 0 });
    mats[2] = glm::translate(mats[2], { outerPos().x(), outerPos().y(), area().pos.z() });
    mats[2] = glm::scale(mats[2], { outerWidth(), outerHeight() , 0 });

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, mats, 3);

    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, 3);
    glBindVertexArray(0);
}

void RPlane::renderControl(const RShaderProg &shaders, GLuint mLoc)
{
    auto inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, model_);
    texture_.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
