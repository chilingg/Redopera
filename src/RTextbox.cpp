#include "RTextbox.h"
#include "RPlane.h"

using namespace Redopera;

std::wstring_convert<std::codecvt_utf8<wchar_t>> RTextsbxo::strcnv;

thread_local RShaderProg RTextsbxo::tTextShaders;
thread_local GLuint RTextsbxo::MODEL_LOC;
thread_local GLuint RTextsbxo::EDGING_LOC;
thread_local GLuint RTextsbxo::COLOR_LOC;
thread_local GLuint RTextsbxo::TEXT_LOC;

void flipVertical(RData *data, int width, int height)
{
    for(int h = 0, h2 = height - 1; h < height/2; ++h, --h2)
    {
        for(int w = 0, w2 = 0; w < width; ++w, ++w2)
        {
            std::swap(data[h*width+w], data[h2*width+w2]);
        }
    }
}

const RTextsbxo::RenderTool &RTextsbxo::textboxRenderTool()
{
    if(!tTextShaders.isValid())
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
            "flat out int instance;\n"
            "void main(void)\n"
            "{\n"
                "gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos, 1.0);\n"
                "if(edging == vec4(0, 0, 0, 0))\n"
                "{\n"
                    "texCoor = vec4(aTexCoor, 0, 0.0);\n"
                    "instance = gl_InstanceID;\n"
                "}\n"
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
            "uniform sampler2D backColor;\n"
            "uniform sampler2D text;\n"
            "uniform vec3 color;\n"
            "flat in int instance;\n"
            "void main(void)\n"
            "{\n"
                "if(texCoor.a == 0)\n"
                    "if(instance == 1)\n"
                        "outColor = vec4(color, texture(text, texCoor.st).r);\n"
                    "else\n"
                        "outColor = texture(backColor, texCoor.st);\n"
                "else\n"
                    "outColor = texCoor;\n"
            "}\n"
        };

        tTextShaders.releaseShader();
        tTextShaders.rename("TextShaders");
        tTextShaders.attachShader({ RShader(vCode, RShader::Type::Vertex, "Text-VS"),
                                    RShader(fCode, RShader::Type::Fragment, "Text-FS")});
        tTextShaders.linkProgram();
        auto inter = tTextShaders.useInterface();
        MODEL_LOC = tTextShaders.getUniformLocation("model");
        EDGING_LOC = tTextShaders.getUniformLocation("edging");
        COLOR_LOC = tTextShaders.getUniformLocation("color");
        TEXT_LOC = tTextShaders.getUniformLocation("text");
        inter.setCameraMove(tTextShaders.getUniformLocation("view"), 0, 0, 0);
    }

    thread_local static RenderTool tool { tTextShaders, MODEL_LOC, COLOR_LOC, TEXT_LOC, EDGING_LOC };

    return tool;
}

void RTextsbxo::setTextBoxShadersAsThread(const RShaderProg &shaders, GLuint modelLoc, GLuint edgingLoc)
{
    tTextShaders = shaders;
    MODEL_LOC = modelLoc;
    EDGING_LOC = edgingLoc;
}

const RShaderProg &RTextsbxo::textboxShader()
{
    return textboxRenderTool().shaders;
}

RTextsbxo::RTextsbxo():
    RTextsbxo(L"Text Label", 82, 18, 0, 0)
{

}

RTextsbxo::RTextsbxo(const std::wstring &text, int width, int height, int x, int y, int z):
    RArea(width, height, x, y, z),
    backTex_(RTexture::transTex()),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::string &text, int width, int height, int x, int y, int z):
    RArea(width, height, x, y, z),
    backTex_(RTexture::transTex()),
    texts_(strcnv.from_bytes(text)),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::wstring &text, int width, int height, const RPoint &pos):
    RArea(width, height, pos),
    backTex_(RTexture::transTex()),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::string &text, int width, int height, const RPoint &pos):
    RArea(width, height, pos),
    backTex_(RTexture::transTex()),
    texts_(strcnv.from_bytes(text)),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::wstring &text, const RSize &size, const RPoint &pos):
    RArea(size, pos),
    backTex_(RTexture::transTex()),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::string &text, const RSize &size, const RPoint &pos):
    RArea(size, pos),
    backTex_(RTexture::transTex()),
    texts_(strcnv.from_bytes(text)),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::wstring &text, const RRect &rect, int z):
    RArea(rect, z),
    backTex_(RTexture::transTex()),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::string &text, const RRect &rect, int z):
    RArea(rect, z),
    backTex_(RTexture::transTex()),
    texts_(strcnv.from_bytes(text)),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::wstring &text, const RArea::Format &area):
    RArea(area),
    backTex_(RTexture::transTex()),
    texts_(text),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const std::string &text, const RArea::Format &area):
    RArea(area),
    backTex_(RTexture::transTex()),
    texts_(strcnv.from_bytes(text)),
    model_{ glm::mat4(1), glm::mat4(1) }
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(const RTextsbxo &box):
    backTex_(box.backTex_),
    textTex_(box.textTex_),
    texts_(box.texts_),
    format_(box.format_),
    model_(box.model_),
    resetting_(box.resetting_),
    typesetting(box.typesetting)
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo::RTextsbxo(RTextsbxo &&box):
    backTex_(std::move(box.backTex_)),
    textTex_(std::move(box.textTex_)),
    texts_(std::move(box.texts_)),
    format_(std::move(box.format_)),
    model_(std::move(box.model_)),
    resetting_(box.resetting_),
    typesetting(box.typesetting)
{
    backTex_.rename("Textsbox-Back");
    textTex_.rename("Textsbox-Text");
}

RTextsbxo &RTextsbxo::operator=(const RTextsbxo &box)
{
    RArea::operator=(box);
    backTex_ = box.backTex_;
    textTex_ = box.textTex_;
    texts_ = box.texts_;
    format_ = box.format_;
    model_ = box.model_;
    resetting_ = box.resetting_;
    typesetting = box.typesetting;
    return *this;
}

RTextsbxo &RTextsbxo::operator=(const RTextsbxo &&box)
{
    RArea::operator=(box);
    backTex_ = std::move(box.backTex_);
    textTex_ = std::move(box.textTex_);
    texts_ = std::move(box.texts_);
    format_ = std::move(box.format_);
    model_ = std::move(box.model_);
    resetting_ = box.resetting_;
    typesetting = box.typesetting;
    return *this;
}

RTextsbxo::Typeset RTextsbxo::typeset() const
{
    return format_.typeset;
}

RColor RTextsbxo::fontColor() const
{
    return RColor(format_.color[0]*255, format_.color[0]*255, format_.color[0]*255, format_.color[0]*255);
}

const RFont &RTextsbxo::font() const
{
    return format_.font;
}

unsigned RTextsbxo::fontSize() const
{
    return format_.font.size();
}

float RTextsbxo::lineSpacing() const
{
    return format_.lSpacing;
}

float RTextsbxo::wordSpacing() const
{
    return format_.wSpacing;
}

float RTextsbxo::spacing() const
{
    return format_.spacing;
}

RTextsbxo::Format RTextsbxo::textFormat() const
{
    return format_;
}

const RTexture &RTextsbxo::textTexture() const
{
    return textTex_;
}

bool RTextsbxo::isSeting() const
{
    return resetting_;
}

void RTextsbxo::setFontColor(R_RGBA rgba)
{
    RColor color(rgba);
    format_.color = { color.r()/255.f, color.g()/255.f, color.b()/255.f };
}

void RTextsbxo::setFontColor(const RColor &color)
{
    format_.color = { color.r()/255.f, color.g()/255.f, color.b()/255.f };
}

void RTextsbxo::setFontColor(unsigned r, unsigned g, unsigned b)
{
    format_.color = { r/255.f, g/255.f, b/255.f };
}

void RTextsbxo::setBackColor(R_RGBA rgba)
{
    const RData *colorData = reinterpret_cast<RData*>(&rgba);
    backTex_.load(colorData, 1, 1, 4, RTexture::NearestTex);
}

void RTextsbxo::setBackColor(const RColor &color)
{
    setBackColor(color.rgba());
}

void RTextsbxo::setBackColor(unsigned r, unsigned g, unsigned b, unsigned a)
{
    setBackColor(RColor(r, g, b, a));
}

void RTextsbxo::setTexts(std::wstring texts)
{
    texts_ = texts;
    reseting();
}

void RTextsbxo::setTexts(std::string texts)
{
    texts_ = strcnv.from_bytes(texts);
    reseting();
}

void RTextsbxo::setFontSize(unsigned size)
{
    format_.font.setSize(size);
    reseting();
}

void RTextsbxo::setFont(RFont font)
{
    format_.font = font;
    reseting();
}

void RTextsbxo::setlineSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.lSpacing = value;
    reseting();
}

void RTextsbxo::setWordSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.wSpacing = value;
    reseting();
}

void RTextsbxo::setSpacing(float value)
{
    value = std::max(0.0f, value);
    format_.spacing = value;
    reseting();
}

void RTextsbxo::setTextFormat(Format format)
{
    format_ = format;
    reseting();
}

void RTextsbxo::setEllipsis(bool b)
{
    format_.ellipsis = b;
}

void RTextsbxo::setTexture(const RTexture &tex)
{
    textTex_ = tex;
}

void RTextsbxo::setTextureName(const std::string &name)
{
    backTex_.rename(name + "...Back");
    textTex_.rename(name + "...Text");
}

void RTextsbxo::verticalTypeset()
{
    typesetting = &RTextsbxo::verticalTextToTexture;
    format_.typeset = Typeset::Vertical;
    reseting();
}

void RTextsbxo::horizontalTypeset()
{
    typesetting = &RTextsbxo::horizontalTextToTexture;
    format_.typeset = Typeset::Horizontal;
    reseting();
}

void RTextsbxo::reseting()
{
    resetting_ = true;
}

void RTextsbxo::resetingNow()
{
    (this->*typesetting)();
}

void RTextsbxo::complete()
{
    resetting_ = false;
}

void RTextsbxo::update()
{
    float w = innerWidth() + 10;
    float h = innerHeight() + 10;

    float x, y;
    switch(area().align.h)
    {
    case RArea::Align::Left:
        x = w / 2.0f + area().padding.l - 5;
        break;
    case RArea::Align::Right:
        x = width() - w/2.0f - area().padding.r + 5;
        break;
    default: // RArea::Align::Mind
        x = width() / 2.0f;
        break;
    }
    switch(area().align.v)
    {
    case RArea::Align::Bottom:
        y = h / 2.0f + area().padding.b - 5;
        break;
    case RArea::Align::Top:
        y = height() - h/2.0f - area().padding.t + 5;
        break;
    default: // RArea::Align::Mind
        y = height() / 2.0f;
        break;
    }

    model_ = { glm::mat4(1), glm::mat4(1) };
    model_[1] = glm::translate(model_[1], { area().pos.x() + x, area().pos.y() + y, 0 });
    model_[1] = model_[1] * glm::mat4_cast(glm::qua<float>(glm::vec3{ area().rotate.x, area().rotate.y, area().rotate.z }));
    model_[1] = glm::scale(model_[1], { w, h, 0.0f });

    if(area().flipH)
    {
        model_[1] [0][0] *= -1;
        model_[1] [0][1] *= -1;
        model_[1] [0][2] *= -1;
        model_[1] [0][3] *= -1;
    }
    if(area().flipV)
    {
        model_[1] [1][0] *= -1;
        model_[1] [1][1] *= -1;
        model_[1] [1][2] *= -1;
        model_[1] [1][3] *= -1;
    }

    model_[0] = glm::translate(model_[0], { area().pos.x() + width()/2, area().pos.y() + height()/2, area().pos.z() });
    model_[0] = model_[0] * glm::mat4_cast(glm::qua<float>(glm::vec3{ area().rotate.x, area().rotate.y, area().rotate.z }));
    model_[0] = glm::scale(model_[0], { width(), height(), 0 });

    if(dirty() & (RArea::Scale | RArea::Typeset) || resetting_)
    {
        (this->*typesetting)();
        complete();
    }
    clearDirty();
}

void RTextsbxo::render()
{
    if(dirty()) update();
    else if(isSeting())
        (this->*typesetting)();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
    glBindVertexArray(rt.vao);

    const RenderTool &tbrt = textboxRenderTool();
    RInterface inter = tbrt.shaders.useInterface();
    inter.setUniform(tbrt.edgingLoc, .0f, .0f, .0f, .0f);
    inter.setUniformMatrix(tbrt.modelLoc, model_.data(), 2);
    inter.setUniform(tbrt.colorLoc, format_.color);
    inter.setUniform(tbrt.textLoc, 1);
    textTex_.bind(1);
    backTex_.bind(0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

    glBindVertexArray(0);
}

void RTextsbxo::render(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();
    else if(isSeting())
        (this->*typesetting)();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
    glBindVertexArray(rt.vao);

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, model_.data(), 2);
    textTex_.bind(1);
    backTex_.bind(0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

    glBindVertexArray(0);
}

void RTextsbxo::edging(const RColor &color)
{
    if(dirty()) update();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { area().pos.x(), area().pos.y(), area().pos.z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    const RenderTool &tbrt = textboxRenderTool();
    RInterface inter = tbrt.shaders.useInterface();
    inter.setUniformMatrix(tbrt.modelLoc, mat);
    inter.setUniform(tbrt.edgingLoc, color.r()/255.f, color.g()/255.f, color.b()/255.f, 1.0f);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RTextsbxo::edging(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mat(1);
    mat = glm::translate(mat, { area().pos.x(), area().pos.y(), area().pos.z() });
    mat = glm::scale(mat, { width(), height(), 0 });

    RInterface inter = shaders.useInterface();
    inter.setUniformMatrix(mLoc, mat);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RTextsbxo::edgingAll()
{
    if(dirty()) update();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
    glBindVertexArray(rt.edgingVAO);

    glm::mat4 mats[3] { glm::mat4(1), glm::mat4(1), glm::mat4(1)};

    mats[0] = glm::translate(mats[0], { area().pos.x(), area().pos.y(), area().pos.z() });
    mats[0] = glm::scale(mats[0], { width(), height(), 0 });
    mats[1] = glm::translate(mats[1], { innerPos().x(), innerPos().y(), area().pos.z() });
    mats[1] = glm::scale(mats[1], { innerWidth(), innerHeight() , 0 });
    mats[2] = glm::translate(mats[2], { outerPos().x(), outerPos().y(), area().pos.z() });
    mats[2] = glm::scale(mats[2], { outerWidth(), outerHeight() , 0 });

    const RenderTool &tbrt = textboxRenderTool();
    RInterface inter = tbrt.shaders.useInterface();
    inter.setUniformMatrix(tbrt.modelLoc, mats, 3);
    inter.setUniform(tbrt.edgingLoc, .0f, .0f, .0f, 1.0f);

    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, 3);
    glBindVertexArray(0);
}

void RTextsbxo::edgingAll(const RShaderProg &shaders, GLuint mLoc)
{
    if(dirty()) update();

    const RPlane::RenderTool &rt = RPlane::planeRenderTool();
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

void RTextsbxo::verticalTextToTexture()
{
    unsigned advanceL = format_.font.size() * format_.lSpacing; // 行步进
    unsigned advanceW = 0; // 宽步进
    unsigned lenMax = innerHeight();   // 行宽限制
    unsigned lineMax = innerWidth();    // 行高限制
    unsigned linepos = format_.font.size();  // 行高
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

            const RFont::Glyph *glyph = font().getFontGlyph(texts_[i]);
            unsigned len = fsize + glyph->yoff + glyph->height;
            wordsw = len + advanceW;
            advanceW += len * format_.wSpacing;

            if(wordsw > lenMax)
            {
                if(linepos + advanceL > lineMax) break;

                unsigned len = fsize + glyph->yoff + glyph->height;
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
    if(loader_.size() < lenMax*lineMax)
        loader_.resize(lenMax*lineMax);
    std::fill(loader_.begin(), loader_.begin() + lenMax*lineMax, '\0');

    if(hAlign() == RArea::Align::Left) linepos = lineMax - ((lines.size()-1) * advanceL + fsize) - 5;
    else if(hAlign() == RArea::Align::Mind) linepos = (((lines.size()-1) * advanceL + fsize) + lineMax) / 2;
    else linepos = lineMax - 5;

    int wordOffset;
    const RFont::Glyph *glyph;
    // 为空格预备的Glyph
    RFont::Glyph spacing { 1, static_cast<int>(format_.spacing * fsize), 0, -static_cast<int>(fsize),
                std::unique_ptr<const RData[]>(new RData[static_cast<int>(format_.spacing * fsize)]()) };
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

            int startx = linepos - fsize;
            int starty = wordOffset + fsize + glyph->yoff;

            for(int y = 0; y < glyph->height; ++y)
            {
                for(int x = 0; x < glyph->width; ++x)
                {
                    if(!glyph->data.get()[y * glyph->width + x])
                        continue;
                    loader_[(starty + y) * lineMax + startx + x] = glyph->data.get()[y*glyph->width+x];
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
                    loader_[y * lineMax + x] = '\xff';
        }
    }

    flipVertical(loader_.data(), lineMax, lenMax);
    if(lineMax != static_cast<unsigned>(textTex_.width()) || lenMax != static_cast<unsigned>(textTex_.height()))
        textTex_.load(loader_.data(), lineMax, lenMax, 1, RTexture::SingleTex);
    else
        textTex_.reload(loader_.data());
    complete();
}

void RTextsbxo::horizontalTextToTexture()
{
    unsigned advanceL = format_.font.size() * format_.lSpacing; // 行步进
    unsigned advanceW = 0; // 宽步进
    unsigned lenMax = innerWidth();   // 行宽限制
    unsigned lineMax = innerHeight();    // 行高限制
    unsigned linepos = format_.font.size();  // 行高
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
    if(loader_.size() < lenMax*lineMax)
        loader_.resize(lenMax*lineMax);
    std::fill(loader_.begin(), loader_.begin() + lenMax*lineMax, '\0');

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
                    loader_[(starty + y) * lenMax + startx + x] = glyph->data.get()[y*glyph->width+x];
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
                    loader_[y * lenMax + x] = '\xff';
        }
    }

    flipVertical(loader_.data(), lenMax, lineMax);
    if(lenMax != static_cast<unsigned>(textTex_.width()) || lineMax != static_cast<unsigned>(textTex_.height()))
        textTex_.load(loader_.data(), lenMax, lineMax, 1, RTexture::SingleTex);
    else
        textTex_.reload(loader_.data());
    complete();
}
