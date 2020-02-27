#include "RPlane.h"

using namespace Redopera;

thread_local std::weak_ptr<RPlane::RenderTool> RPlane::tRenderTool;

RPlane::Vertexs RPlane::createVaertexObject()
{
    Vertexs v;

    glGenVertexArrays(2, &v.ao1);
    glGenBuffers(2, &v.bo1);

    float plane[24]{
            0.5f,-0.5f, 0.0f, 1.0f, 0.0f,//右下
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,//右上
           -0.5f,-0.5f, 0.0f, 0.0f, 0.0f,//左下
           -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,//左上
    };

    glBindVertexArray(v.ao1);
    glBindBuffer(GL_ARRAY_BUFFER, v.bo1);
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

    glBindVertexArray(v.ao2);
    glBindBuffer(GL_ARRAY_BUFFER, v.bo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edge), edge, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glBindVertexArray(0);

    return v;
}

RPlane::RenderTool RPlane::createRenderTool()
{
    Vertexs v = createVaertexObject();

    const GLchar *vCode =
            "#version 330 core\n"
            "layout(location = 0) in vec3 aPos;\n"
            "layout(location = 1) in vec2 aTexCoor;\n"
            "uniform mat4 model[3];\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "out vec2 texCoor;\n"
            "flat out int instance;\n"
            "void main(void)\n"
            "{\n"
                "instance = gl_InstanceID;\n"
                "texCoor = aTexCoor;\n"
                "gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos, 1.0);\n"
            "}\n";
    const GLchar *fCode =
            "#version 330 core\n"
            "uniform int edging;\n"
            "uniform float luminance;\n"
            "uniform vec4 hue;\n"
            "uniform vec4 eColor;\n"
            "uniform sampler2D tex;\n"
            "in vec2 texCoor;\n"
            "flat in int instance;\n"
            "out vec4 outColor;\n"
            "void main(void)\n"
            "{\n"
                "if(edging == 0)\n"
                "{\n"
                    "outColor = texture(tex, texCoor) * hue;\n"
                    "outColor.rgb = outColor.rgb * luminance;\n"
                "}\n"
                "else if(edging == 1)\n"
                    "outColor = eColor;\n"
                "else {\n"
                    "if(instance == 0) outColor = vec4(1.0, 0, 0, 1.0);\n"
                    "else if(instance == 1) outColor = vec4(0, 1.0, 0, 1.0);\n"
                    "else outColor = vec4(0, 0, 1.0, 1.0);\n"
                "}\n"
            "}\n";

    RShaderProg shaders({ RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment) });
    GLint mloc = shaders.getUniformLocation("model");
    GLint eloc = shaders.getUniformLocation("edging");
    RInterface inter = shaders.useInterface();
    inter.setCameraMove(shaders.getUniformLocation("view"), 0, 0, 0);
    inter.setUniform(shaders.getUniformLocation("luminance"), 1.f);
    inter.setUniform(shaders.getUniformLocation("hue"), 1.f, 1.f, 1.f, 1.f);

    return { shaders, v.ao1, v.ao2, mloc, eloc, v.bo1, v.bo2 };
}

void deleteRenderTool(RPlane::RenderTool *p)
{
    glDeleteVertexArrays(2, &p->vao);
    glDeleteBuffers(2, &p->vbo);
    delete p;
}

const std::shared_ptr<RPlane::RenderTool> RPlane::setShadersAsThread(const RShaderProg &shaders, GLint mloc, GLint eloc)
{
    if(tRenderTool.expired())
    {
        Vertexs v = createVaertexObject();
        auto rt = std::make_unique<RenderTool>(RenderTool{ shaders, v.ao1, v.ao2, mloc, eloc, v.bo1, v.bo2 });

        return std::shared_ptr<RPlane::RenderTool>(rt.release(), deleteRenderTool);
    }
    else {
        auto rt = tRenderTool.lock();
        rt->shaders = shaders;
        rt->modelLoc = mloc;
        rt->edgingLoc = eloc;

        return rt;
    }
}

RPlane::RPlane():
    RPlane(0, 0, 0, 0, 0)
{

}

RPlane::RPlane(int width, int height, int x, int y, int z, const RArea::Format &area):
    RPlane(RSize(width, height), RPoint(x, y, z), area)
{

}

RPlane::RPlane(int width, int height, const RPoint &pos, const RArea::Format &area):
    RPlane(RSize(width, height), pos, area)
{

}

RPlane::RPlane(const RSize &size, const RPoint &pos, const RArea::Format &area):
    RArea(size, pos, area),
    renderControl(std::bind(&RPlane::defaultRenderControl, this, std::placeholders::_1, std::placeholders::_2)),
    renderTool_(renderTool()),
    mats_{ glm::mat4(1), glm::mat4(1), glm::mat4(1) },
    model_(1)
{

}

RPlane::RPlane(const RRect &rect, int z, const RArea::Format &area):
    RPlane(rect.size(), RPoint(rect.bottomLeft(), z), area)
{

}

RPlane::RPlane(const RPlane &plane):
    RArea(plane),
    renderControl(plane.renderControl),
    renderTool_(plane.renderTool_),
    mats_(plane.mats_),
    model_(plane.model_),
    texture_(plane.texture_)
{

}

RPlane::RPlane(const RPlane &&plane):
    RArea(plane),
    renderControl(std::move(plane.renderControl)),
    renderTool_(plane.renderTool_),
    mats_(std::move(plane.mats_)),
    model_(std::move(plane.model_)),
    texture_(std::move(plane.texture_))
{

}

RPlane &RPlane::operator=(const RPlane &plane)
{
    RArea::operator=(plane);
    renderControl = plane.renderControl;
    renderTool_ = plane.renderTool_;
    mats_ = plane.mats_;
    model_ = plane.model_;
    texture_ = plane.texture_;

    return *this;
}

RPlane &RPlane::operator=(const RPlane &&plane)
{
    RArea::operator=(plane);
    renderControl = std::move(plane.renderControl);
    renderTool_ = plane.renderTool_;
    mats_ = std::move(plane.mats_);
    model_ = std::move(plane.model_);
    texture_ = std::move(plane.texture_);
    return *this;
}

const RShaderProg &RPlane::planeShader()
{
    return renderTool()->shaders;
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

void RPlane::setColorTexture(const RColor &color)
{
    R_RGBA rgba = color.rgba();
    setColorTexture(rgba);
}

void RPlane::setColorTexture(R_RGBA rgba)
{
    const RData *colorData = reinterpret_cast<RData*>(&rgba);
    texture_.load(colorData, 1, 1, 4, RTexture::Nearest4);
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

void RPlane::update()
{
    if(dirty() & (RArea::Scale | RArea::Typeset))
    {
        float w, h, tw = texture_.width(), th = texture_.height();
        float min = innerWidth() / tw;
        float max = innerHeight() / th;
        if(min > max) std::swap(min, max);

        switch(areaFormat().mode)
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
        switch(areaFormat().align.h)
        {
        case RArea::Align::Left:
            x = w / 2.0f + areaFormat().padding.l;
            break;
        case RArea::Align::Right:
            x = width() - w/2.0f - areaFormat().padding.r;
            break;
        default: // RArea::Align::Mind
            x = width() / 2.0f;
            break;
        }
        switch(areaFormat().align.v)
        {
        case RArea::Align::Bottom:
            y = h / 2.0f + areaFormat().padding.b;
            break;
        case RArea::Align::Top:
            y = height() - h/2.0f - areaFormat().padding.t;
            break;
        default: // RArea::Align::Mind
            y = height() / 2.0f;
            break;
        }

        mats_.tran = glm::translate(glm::mat4(1), { x, y, 0 });
        mats_.scale = glm::scale(glm::mat4(1), { w, h, 0.0f });

        if(areaFormat().flip.h)
        {
            mats_.scale[0][0] *= -1;
            mats_.scale[0][1] *= -1;
            mats_.scale[0][2] *= -1;
            mats_.scale[0][3] *= -1;
        }
        if(areaFormat().flip.v)
        {
            mats_.scale[1][0] *= -1;
            mats_.scale[1][1] *= -1;
            mats_.scale[1][2] *= -1;
            mats_.scale[1][3] *= -1;
        }
    }

    if(dirty() & RArea::Rotate)
        mats_.rotate = glm::mat4_cast(glm::qua<float>(glm::vec3{ areaFormat().rotate.x, areaFormat().rotate.y, areaFormat().rotate.z }));

    glm::mat4 move(1);
    move = glm::translate(move, { x(), y(), z() });

    model_ = move * mats_.tran * mats_.rotate * mats_.scale;
    clearDirty();
}

void RPlane::render()
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->vao);

    RInterface inter = rt->shaders.useInterface();
    inter.setUniform(rt->edgingLoc, 0);
    renderControl(rt->shaders, rt->modelLoc);

    glBindVertexArray(0);
}

void RPlane::render(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->vao);

    RInterface inter = shaders.useInterface();
    renderControl(shaders, mLoc);

    glBindVertexArray(0);
}

void RPlane::edging(const RColor &color)
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { x(), y(), z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    RInterface inter = rt->shaders.useInterface();
    inter.setUniformMatrix(rt->modelLoc, mat);
    inter.setUniform(rt->edgingLoc, 1);
    inter.setUniform(rt->shaders.getUniformLocation("eColor"), color.r()/255.f, color.g()/255.f, color.b()/255.f, 1.0f);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RPlane::edging(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { x(), y(), z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, mat);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RPlane::edgingAll()
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->edgingVAO);

    glm::mat4 mats[3] { glm::mat4(1), glm::mat4(1), glm::mat4(1)};

    mats[0] = glm::translate(mats[0], { x(), y(), z() });
    mats[0] = glm::scale(mats[0], { width(), height(), 0 });
    mats[1] = glm::translate(mats[1], { innerPos().x(), innerPos().y(), z() });
    mats[1] = glm::scale(mats[1], { innerWidth(), innerHeight() , 0 });
    mats[2] = glm::translate(mats[2], { outerPos().x(), outerPos().y(), z() });
    mats[2] = glm::scale(mats[2], { outerWidth(), outerHeight() , 0 });

    RInterface inter = rt->shaders.useInterface();
    inter.setUniformMatrix(rt->modelLoc, mats, 3);
    inter.setUniform(rt->edgingLoc, 3);

    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, 3);
    glBindVertexArray(0);
}

std::shared_ptr<const RPlane::RenderTool> RPlane::renderTool()
{
    if(tRenderTool.expired())
    {
        auto rt = std::make_unique<RenderTool>(createRenderTool());
        std::shared_ptr<RenderTool> srt(rt.release(), deleteRenderTool);
        tRenderTool = srt;
        return srt;
    }
    else
        return tRenderTool.lock();
}

void RPlane::edgingAll(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    auto rt = renderTool();
    glBindVertexArray(rt->edgingVAO);

    glm::mat4 mats[3] { glm::mat4(1), glm::mat4(1), glm::mat4(1)};

    mats[0] = glm::translate(mats[0], { x(), y(), z() });
    mats[0] = glm::scale(mats[0], { width(), height(), 0 });
    mats[1] = glm::translate(mats[1], { innerPos().x(), innerPos().y(), z() });
    mats[1] = glm::scale(mats[1], { innerWidth(), innerHeight() , 0 });
    mats[2] = glm::translate(mats[2], { outerPos().x(), outerPos().y(), z() });
    mats[2] = glm::scale(mats[2], { outerWidth(), outerHeight() , 0 });

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, mats, 3);

    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, 3);
    glBindVertexArray(0);
}

void RPlane::defaultRenderControl(const RShaderProg &shaders, GLuint mLoc)
{
    auto inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, model_);
    texture_.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
