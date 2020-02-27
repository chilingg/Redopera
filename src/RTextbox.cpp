#include "RTextbox.h"
#include "rsc/RImage.h"

using namespace Redopera;

RTextsbox::Format RTextsbox::fontFmt;
thread_local std::weak_ptr<RTextsbox::RenderTool> RTextsbox::tRenderTool;

void RTextsbox::setDefaultFontFmt(RTextsbox::Format fmt)
{
    fontFmt = fmt;
}

const RTextsbox::Format &RTextsbox::getDefaultFontFmt()
{
    return fontFmt;
}

void deleteRenderTool(RTextsbox::RenderTool *p)
{
    glDeleteVertexArrays(2, &p->vao);
    glDeleteBuffers(2, &p->vbo);
    delete p;
}

std::shared_ptr<const RTextsbox::RenderTool> RTextsbox::renderTool()
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

RTextsbox::RTextsbox():
    RTextsbox(L"Text Label", 82, 18, 0, 0)
{

}

RTextsbox::RTextsbox(const std::wstring &text, int width, int height, int x, int y, int z, const RArea::Format &area):
    RTextsbox(text, RSize(width, height), RPoint(x, y, z), area)
{

}

RTextsbox::RTextsbox(const std::wstring &text, int width, int height, const RPoint &pos, const RArea::Format &area):
    RTextsbox(text, RSize(width, height), pos, area)
{

}

RTextsbox::RTextsbox(const std::wstring &text, const RSize &size, const RPoint &pos, const RArea::Format &area):
    RArea(size, pos, area),
    renderTool_(renderTool()),
    backTex_(renderTool()->defaultBack),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) },
    format_(fontFmt)
{

}

RTextsbox::RTextsbox(const std::wstring &text, const RRect &rect, int z, const RArea::Format &area):
    RTextsbox(text, rect.size(), RPoint(rect.bottomLeft(), z), area)
{

}

RTextsbox::RTextsbox(const RTextsbox &box):
    backTex_(box.backTex_),
    texts_(box.texts_),
    model_(box.model_),
    format_(box.format_),
    font_(box.font_),
    textTex_(box.textTex_),
    resetting_(box.resetting_),
    typesetting(box.typesetting)
{

}

RTextsbox::RTextsbox(RTextsbox &&box):
    backTex_(std::move(box.backTex_)),
    texts_(std::move(box.texts_)),
    model_(std::move(box.model_)),
    format_(box.format_),
    font_(std::move(box.font_)),
    textTex_(std::move(box.textTex_)),
    resetting_(box.resetting_),
    typesetting(box.typesetting)
{

}

RTextsbox &RTextsbox::operator=(const RTextsbox &box)
{
    RArea::operator=(box);
    backTex_ = box.backTex_;
    textTex_ = box.textTex_;
    texts_ = box.texts_;
    format_ = box.format_;
    font_ = box.font_;
    model_ = box.model_;
    resetting_ = box.resetting_;
    typesetting = box.typesetting;
    return *this;
}

RTextsbox &RTextsbox::operator=(const RTextsbox &&box)
{
    RArea::operator=(box);
    backTex_ = std::move(box.backTex_);
    textTex_ = std::move(box.textTex_);
    texts_ = std::move(box.texts_);
    format_ = std::move(box.format_);
    font_ = std::move(box.font_);
    model_ = std::move(box.model_);
    resetting_ = box.resetting_;
    typesetting = box.typesetting;
    return *this;
}

const RShaderProg &RTextsbox::textsShader()
{
    return renderTool_->shaders;
}

RTextsbox::Typeset RTextsbox::typeset() const
{
    return format_.typeset;
}

RColor RTextsbox::fontColor() const
{
    return RColor(format_.fcolor[0]*255, format_.fcolor[0]*255, format_.fcolor[0]*255, format_.fcolor[0]*255);
}

const RFont &RTextsbox::font() const
{
    return font_;
}

unsigned RTextsbox::fontSize() const
{
    return font_.size();
}

float RTextsbox::lineSpacing() const
{
    return format_.lSpacing;
}

float RTextsbox::wordSpacing() const
{
    return format_.wSpacing;
}

float RTextsbox::spacing() const
{
    return format_.spacing;
}

RTextsbox::Format RTextsbox::textFormat() const
{
    return format_;
}

const RTexture &RTextsbox::textTexture() const
{
    return textTex_;
}

bool RTextsbox::isSeting() const
{
    return resetting_;
}

void RTextsbox::setFontColor(R_RGB rgb)
{
    RColor color(rgb << 8);
    format_.fcolor = { color.r()/255.f, color.g()/255.f, color.b()/255.f};
}

void RTextsbox::setFontColor(const RColor &color)
{
    format_.fcolor = { color.r()/255.f, color.g()/255.f, color.b()/255.f };
}

void RTextsbox::setFontColor(unsigned r, unsigned g, unsigned b)
{
    format_.fcolor = { r/255.f, g/255.f, b/255.f };
}

void RTextsbox::setBackColor(R_RGBA rgba)
{
    const RData *colorData = reinterpret_cast<RData*>(&rgba);
    backTex_.load(colorData, 1, 1, 4, RTexture::Nearest4);
}

void RTextsbox::setBackColor(const RColor &color)
{
    setBackColor(color.rgba());
}

void RTextsbox::setBackColor(unsigned r, unsigned g, unsigned b, unsigned a)
{
    setBackColor(RColor(r, g, b, a));
}

void RTextsbox::setTexts(std::wstring texts)
{
    texts_ = texts;
    addDirty(RArea::Typeset);
}

void RTextsbox::setFontSize(unsigned size)
{
    font_.setSize(size);
    addDirty(RArea::Typeset);
}

void RTextsbox::setFont(RFont font)
{
    font_ = font;
    addDirty(RArea::Typeset);
}

void RTextsbox::setlineSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.lSpacing = value;
    addDirty(RArea::Typeset);
}

void RTextsbox::setWordSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.wSpacing = value;
    addDirty(RArea::Typeset);
}

void RTextsbox::setSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.spacing = value;
    addDirty(RArea::Typeset);
}

void RTextsbox::setTextFormat(Format format)
{
    typesetting = format.typeset == Typeset::Vertical ?
                &RTextsbox::verticalTextToTexture : &RTextsbox::horizontalTextToTexture;
    format_ = format;
    addDirty(RArea::Typeset);
}

void RTextsbox::setEllipsis(bool b)
{
    format_.ellipsis = b;
    addDirty(RArea::Typeset);
}

void RTextsbox::setTexture(const RTexture &tex)
{
    textTex_ = tex;
}

void RTextsbox::setPixScale(float x, float y)
{
    format_.pixScale = { x, y };
    addDirty(RArea::Typeset);
}

void RTextsbox::verticalTypeset()
{
    typesetting = &RTextsbox::verticalTextToTexture;
    format_.typeset = Typeset::Vertical;
    addDirty(RArea::Typeset);
}

void RTextsbox::horizontalTypeset()
{
    typesetting = &RTextsbox::horizontalTextToTexture;
    format_.typeset = Typeset::Horizontal;
    addDirty(RArea::Typeset);
}

void RTextsbox::reseting()
{
    resetting_ = true;
}

void RTextsbox::resetingNow()
{
    (this->*typesetting)();
}

void RTextsbox::complete()
{
    resetting_ = false;
}

void RTextsbox::update()
{
    float w = (innerWidth() + 10) / format_.pixScale.x;
    float h = (innerHeight() + 10) / format_.pixScale.y;

    float x, y;
    switch(areaFormat().align.h)
    {
    case RArea::Align::Left:
        x = w / 2.0f + areaFormat().padding.l - (5 / format_.pixScale.x);
        break;
    case RArea::Align::Right:
        x = width() - w/2.0f - areaFormat().padding.r + (5 / format_.pixScale.x);
        break;
    default: // RArea::Align::Mind
        x = width() / 2.0f;
        break;
    }
    switch(areaFormat().align.v)
    {
    case RArea::Align::Bottom:
        y = h / 2.0f + areaFormat().padding.b - (5 / format_.pixScale.y);
        break;
    case RArea::Align::Top:
        y = height() - h/2.0f - areaFormat().padding.t + (5 / format_.pixScale.y);
        break;
    default: // RArea::Align::Mind
        y = height() / 2.0f;
        break;
    }

    model_ = { glm::mat4(1), glm::mat4(1) };
    model_[1] = glm::translate(model_[1], { this->x() + x, this->y() + y, 0 });
    model_[1] = model_[1] * glm::mat4_cast(glm::qua<float>(glm::vec3{ areaFormat().rotate.x, areaFormat().rotate.y, areaFormat().rotate.z }));
    model_[1] = glm::scale(model_[1], { w, h, 0.0f });

    if(areaFormat().flip.h)
    {
        model_[1] [0][0] *= -1;
        model_[1] [0][1] *= -1;
        model_[1] [0][2] *= -1;
        model_[1] [0][3] *= -1;
    }
    if(areaFormat().flip.v)
    {
        model_[1] [1][0] *= -1;
        model_[1] [1][1] *= -1;
        model_[1] [1][2] *= -1;
        model_[1] [1][3] *= -1;
    }

    model_[0] = glm::translate(model_[0], { this->x() + width()/2, this->y() + height()/2, z() });
    model_[0] = model_[0] * glm::mat4_cast(glm::qua<float>(glm::vec3{ areaFormat().rotate.x, areaFormat().rotate.y, areaFormat().rotate.z }));
    model_[0] = glm::scale(model_[0], { width(), height(), 0 });

    if(dirty() & (RArea::Scale | RArea::Typeset) || resetting_)
    {
        (this->*typesetting)();
        complete();
    }
    clearDirty();
}

void RTextsbox::render()
{
    if(dirty()) update();
    else if(isSeting())
        (this->*typesetting)();

    auto rt = renderTool();
    glBindVertexArray(rt->vao);

    RInterface inter = rt->shaders.useInterface();
    inter.setUniform(rt->edgingLoc, 0);
    inter.setUniformMatrix(rt->modelLoc, model_.data(), 2);
    inter.setUniform(rt->colorLoc, format_.fcolor);
    inter.setUniform(rt->textLoc, 1);
    textTex_.bind(1);
    backTex_.bind(0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

    glBindVertexArray(0);
}

void RTextsbox::render(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();
    else if(isSeting())
        (this->*typesetting)();

    auto rt = renderTool();
    glBindVertexArray(rt->vao);

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, model_.data(), 2);
    textTex_.bind(1);
    backTex_.bind(0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

    glBindVertexArray(0);
}

void RTextsbox::edging(const RColor &color)
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
    inter.setUniform(rt->colorLoc, color.r()/255.f, color.g()/255.f, color.b()/255.f);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RTextsbox::edging(const RShaderProg &shaders, GLuint mLoc)
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

void RTextsbox::edgingAll()
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

void RTextsbox::edgingAll(const RShaderProg &shaders, GLuint mLoc)
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

RTextsbox::Vertexs RTextsbox::createVaertexObject()
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

RTextsbox::RenderTool RTextsbox::createRenderTool()
{
    Vertexs v = createVaertexObject();

    static const GLchar *vCode =
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
                "texCoor = aTexCoor;\n"
                "instance = gl_InstanceID;\n"
                "gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos, 1.0);\n"
            "}\n";
    static const GLchar *fCode =
            "#version 330 core\n"
            "in vec2 texCoor;\n"
            "out vec4 outColor;\n"
            "uniform sampler2D backColor;\n"
            "uniform sampler2D text;\n"
            "uniform int edging;\n"
            "uniform vec3 color;\n"
            "flat in int instance;\n"
            "void main(void)\n"
            "{\n"
                "if(edging == 0)\n"
                "{\n"
                    "if(instance == 0)"
                        "outColor = texture(backColor, texCoor.st);\n"
                    "else\n"
                        "outColor = vec4(color, texture(text, texCoor).r);\n"
                "}\n"
                "else if(edging == 1)\n"
                    "outColor = vec4(color, 1.0);\n"
                "else {\n"
                    "if(instance == 0)\n"
                        "outColor = vec4(1.0, 0, 0, 1.0);\n"
                    "else if(instance == 1)\n"
                        "outColor = vec4(0, 1.0, 0, 1.0);\n"
                    "else\n"
                        "outColor = vec4(0, 0, 1.0, 1.0);\n"
                "}\n"
            "}\n";

    RShaderProg shaders({ RShader(vCode, RShader::Type::Vertex), RShader(fCode, RShader::Type::Fragment) });
    GLint mloc = shaders.getUniformLocation("model");
    GLint eloc = shaders.getUniformLocation("edging");
    GLint cloc = shaders.getUniformLocation("color");
    GLint tloc = shaders.getUniformLocation("text");
    RInterface inter = shaders.useInterface();
    inter.setCameraMove(shaders.getUniformLocation("view"), 0, 0, 0);

    return { shaders, v.ao1, v.ao2, mloc, eloc, cloc, tloc, v.bo1, v.bo2 , RTexture::transTex() };
}

void rotate90(RData *dest, const RData *src, int width, int height)
{
    for(int w = 0; w < width; ++w)
    {
        for(int h = height - 1; h >= 0; --h)
        {
            *dest++ = src[h * width + w];
        }
    }
}

void RTextsbox::verticalTextToTexture()
{
    unsigned advanceL = font_.size() * format_.lSpacing; // 行步进
    unsigned advanceW = 0; // 宽步进
    unsigned lenMax = innerHeight();   // 行宽限制
    unsigned lineMax = innerWidth();    // 行高限制
    unsigned linepos = font_.size();  // 行高
    unsigned fsize = linepos;  // 字体大小

    // 计算每行的行宽
    std::vector<unsigned> lines;
    if(innerHeight() > static_cast<int>(fsize) && innerWidth() > static_cast<int>(fsize))
    {
        lines.reserve(8);
        lines.push_back(0);
        unsigned wordsw = 0;
        for(size_t i = 0; i < texts_.size(); ++i)
        {
            if(texts_[i] == L'\n')
            {
                if(linepos + advanceL > lineMax) break;

                lines.back() += 1; // 换行符占位
                lines.push_back(0);
                advanceW = 0;
                linepos += advanceL;

                continue;
            }
            else if(texts_[i] == L' ')
            {
                wordsw = format_.spacing * fsize + advanceW;
                advanceW += format_.spacing * fsize * format_.wSpacing;

                if(wordsw > lenMax)
                {
                    if(linepos + advanceL > lineMax) break;

                    wordsw = format_.spacing * fsize;
                    advanceW = wordsw * format_.wSpacing;
                    linepos += advanceL;
                }

                lines.back() = wordsw;
                continue;
            }

            int len;
            const RFont::Glyph *glyph = font().getFontGlyph(texts_[i]);
            if((texts_[i] >= L'a' && texts_[i] <= L'z') || (texts_[i] >= L'A' && texts_[i] <= L'Z'))
                len = glyph->xoff + glyph->width;
            else
                len = fsize + glyph->yoff + glyph->height;

            wordsw = len + advanceW;
            advanceW += len * format_.wSpacing;

            if(wordsw > lenMax)
            {
                if(linepos + advanceL > lineMax) break;

                lines.push_back(len);
                advanceW = len * format_.wSpacing;
                linepos += advanceL;

                continue;
            }

            lines.back() = wordsw;
        }
    }
    if(!lines.empty() && lines.back() == 0) lines.pop_back();//末尾换行

    // 四方预留5px
    lenMax += 10;
    lineMax += 10;
    RImage loader(nullptr, lineMax, lenMax, 1);
    loader.full(0);

    if(hAlign() == RArea::Align::Left) linepos = lineMax - ((lines.size()-1) * advanceL + fsize) - 5;
    else if(hAlign() == RArea::Align::Mind) linepos = (((lines.size()-1) * advanceL + fsize) + lineMax) / 2;
    else linepos = lineMax - 5;

    int wordOffset;
    const RFont::Glyph *glyph;
    // 为空格预备的Glyph
    RFont::Glyph spacing { 1, static_cast<int>(format_.spacing * fsize), 0, -static_cast<int>(fsize),
                std::unique_ptr<const RData[]>(new RData[static_cast<int>(format_.spacing * fsize)]()) };
    RFont::Glyph letter;
    size_t textNum = 0;
    advanceW = 0;

    for(auto linew : lines)
    {
        if(vAlign() == RArea::Align::Bottom) wordOffset = lenMax - linew - 5;
        else if(vAlign() == RArea::Align::Mind) wordOffset = (lenMax - linew) / 2;
        else wordOffset = 5;

        linew += wordOffset;

        while(textNum < texts_.size())
        {
            if(texts_[textNum] == L'\n')
            {
                linepos -= advanceL;
                ++textNum;
                break;
            }

            if(texts_[textNum] == L' ')
                glyph = &spacing;
            else
                glyph = font().getFontGlyph(texts_[textNum]);

            if((texts_[textNum] >= L'a' && texts_[textNum] <= L'z') || (texts_[textNum] >= L'A' && texts_[textNum] <= L'Z'))
            {
                auto data = std::make_unique<RData[]>(glyph->width * glyph->height);
                rotate90(data.get(), glyph->data.get(), glyph->width, glyph->height);
                letter = { glyph->height, glyph->width, static_cast<int>(fsize) + glyph->yoff + glyph->height,
                           -(static_cast<int>(fsize) - glyph->xoff) };
                letter.data.reset(data.release());
                glyph = &letter;
            }

            int startx = linepos - fsize;
            int starty = wordOffset + fsize + glyph->yoff;

            for(int y = 0; y < glyph->height; ++y)
            {
                for(int x = 0; x < glyph->width; ++x)
                {
                    if(!glyph->data.get()[y * glyph->width + x])
                        continue;
                    loader.data()[(starty + y) * lineMax + startx + x] = glyph->data.get()[y*glyph->width+x];
                }
            }

            ++textNum;
            unsigned len = fsize + glyph->yoff + glyph->height;
            if(wordOffset + len >= linew)
            {
                linepos -= advanceL; break;
            } else
                wordOffset += len * format_.wSpacing;
        }
    }

    if(format_.ellipsis)
    {
        if(textNum < texts_.size())
        {
            for(unsigned y = lenMax - 5; y < lenMax; ++y)
                for(unsigned x = 0; x < 5; ++x)
                    loader.data()[y * lineMax + x] = '\xff';
        }
    }

    loader.flipV();
    if(lineMax != static_cast<unsigned>(textTex_.width()) || lenMax != static_cast<unsigned>(textTex_.height()))
        textTex_.load(loader.data(), lineMax, lenMax, 1, RTexture::SingleL);
    else
        textTex_.reload(loader.data());
    complete();
}

void RTextsbox::horizontalTextToTexture()
{
    unsigned advanceL = font_.size() * format_.lSpacing; // 行步进
    unsigned advanceW = 0; // 宽步进
    unsigned lenMax = innerWidth();   // 行宽限制
    unsigned lineMax = innerHeight();    // 行高限制
    unsigned linepos = font_.size();  // 行高
    unsigned fsize = linepos;  // 字体大小

    // 计算每行的行宽
    std::vector<unsigned> lines;
    if(innerHeight() > static_cast<int>(fsize) && innerWidth() > static_cast<int>(fsize))
    {
        lines.reserve(8);
        lines.push_back(0);
        unsigned wordsw = 0;
        for(size_t i = 0; i < texts_.size(); ++i)
        {
            const RFont::Glyph *glyph = font().getFontGlyph(texts_[i]);
            wordsw = glyph->width + glyph->xoff + advanceW;
            advanceW += (glyph->width + glyph->xoff) * format_.wSpacing;

            if(texts_[i] == L'\n')
            {
                if(linepos + advanceL > lineMax) break;

                lines.back() += 1; // 换行符占位
                lines.push_back(0);
                advanceW = 0;
                linepos += advanceL;

                continue;
            }
            else if(texts_[i] == L' ')
            {
                wordsw = format_.spacing * fsize + advanceW;
                advanceW += format_.spacing * fsize * format_.wSpacing;

                if(wordsw > lenMax)
                {
                    if(linepos + advanceL > lineMax) break;

                    lines.push_back(format_.spacing * fsize);
                    advanceW = lines.back() * format_.wSpacing;
                    linepos += advanceL;

                    continue;
                }
            }

            if(wordsw > lenMax)
            {
                if(linepos + advanceL > lineMax) break;

                lines.push_back(glyph->width + glyph->xoff);
                advanceW = (glyph->width + glyph->xoff) * format_.wSpacing;
                linepos += advanceL;

                continue;
            }

            lines.back() = wordsw;
        }
    }
    if(!lines.empty() && lines.back() == 0) lines.pop_back();//末尾换行

    // 四方预留5px
    lenMax += 10;  // 纹理行宽
    lineMax += 10;    // 纹理行高
    RImage loader(nullptr, lenMax, lineMax, 1);
    loader.full(0);

    if(vAlign() == RArea::Align::Bottom) linepos = lineMax - ((lines.size()-1) * advanceL + fsize) - 5;
    else if(vAlign() == RArea::Align::Mind) linepos = (lineMax - ((lines.size()-1) * advanceL + fsize)) / 2;
    else linepos = 5; // Top 缺省默认

    int wordOffset;
    const RFont::Glyph *glyph;
    // 为空格预备的Glyph
    RFont::Glyph spacing { static_cast<int>(format_.spacing * fsize), 1, 0, 0,
                std::unique_ptr<const RData[]>(new RData[static_cast<int>(format_.spacing * fsize)]()) };
    size_t textNum = 0;
    advanceW = 0;
    for(auto linew : lines)
    {
        if(hAlign() == RArea::Align::Right) wordOffset = lenMax - linew - 5;
        else if(hAlign() == RArea::Align::Mind) wordOffset = (lenMax - linew) / 2;
        else wordOffset = 5; // Align_Left 缺省默认

        linew += wordOffset;

        while(textNum < texts_.size())
        {
            if(texts_[textNum] == L'\n')
            {
                linepos += advanceL;
                ++textNum;
                break;
            }

            if(texts_[textNum] == L' ')
                glyph = &spacing;
            else
                glyph = font().getFontGlyph(texts_[textNum]);

            int startx = wordOffset + glyph->xoff;
            int starty = linepos + fsize + glyph->yoff;

            for(int y = 0; y < glyph->height; ++y)
            {
                for(int x = 0; x < glyph->width; ++x)
                {
                    if(!glyph->data.get()[y * glyph->width + x])
                        continue;
                    loader.data()[(starty + y) * lenMax + startx + x] = glyph->data.get()[y*glyph->width+x];
                }
            }

            ++textNum;
            if(wordOffset + glyph->width + glyph->xoff >= static_cast<int>(linew))
            {
                linepos += advanceL; break;
            } else
                wordOffset += (glyph->width + glyph->xoff) * format_.wSpacing;
        }
    }

    if(format_.ellipsis)
    {
        //若不能显示所有字符，在末尾添加一个5x5方框
        if(textNum < texts_.size())
        {
            for(unsigned y = lineMax - 5; y < lineMax; ++y)
                for(unsigned x = lenMax - 5; x < lenMax; ++x)
                    loader.data()[y * lenMax + x] = '\xff';
        }
    }

    loader.flipV();
    if(lenMax != static_cast<unsigned>(textTex_.width()) || lineMax != static_cast<unsigned>(textTex_.height()))
        textTex_.load(loader.data(), lenMax, lineMax, 1, RTexture::SingleL);
    else
        textTex_.reload(loader.data());
    complete();
}
