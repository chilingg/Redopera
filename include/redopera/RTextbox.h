#ifndef RTEXTBOX_H
#define RTEXTBOX_H

#include "RArea.h"
#include "RColor.h"
#include "rsc/RFont.h"
#include "rsc/RTexture.h"
#include "rsc/RShaderProg.h"

namespace Redopera {

// 使用utf-8编码保存字符串

class RTextsbox : public RArea
{
public:
    enum class Typeset
    {
        Horizontal,
        Vertical
    };

    struct Format
    {
        float lSpacing = 1.2f;
        float wSpacing = 1.0f;
        float spacing = 0.6f;   // 空格
        Typeset typeset = Typeset::Horizontal;
        glm::vec3 fcolor = { 1.f, 1.f, 1.f };
        struct {
            float x, y;
        } pixScale { 1.0f, 1.0f };
#ifdef R_DEBUG
        bool ellipsis = true;   // 若不能显示所有字符，在末尾添加一个5x5方框
#else
        bool ellipsis = false;
#endif
    };

    struct RenderTool
    {
        RShaderProg shaders;
        GLuint vao;
        GLuint edgingVAO;
        GLint modelLoc;
        GLint edgingLoc;
        GLint colorLoc;
        GLint textLoc;
        GLuint vbo;
        GLuint edgingVBO;
        RTexture defaultBack;
    };

    static void setDefaultFontFmt(Format fmt);
    static const Format& getDefaultFontFmt();

    static std::shared_ptr<const RenderTool> renderTool();

    RTextsbox();

    RTextsbox(const std::wstring &text, int width, int height, int x, int y, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RTextsbox(const std::wstring &text, int width, int height, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    RTextsbox(const std::wstring &text, const RSize &size, const RPoint &pos, const RArea::Format &area = RArea::getDefaultArea());
    RTextsbox(const std::wstring &text, const RRect &rect, int z = 0, const RArea::Format &area = RArea::getDefaultArea());
    RTextsbox(const RTextsbox &box);
    RTextsbox(RTextsbox &&box);

    RTextsbox& operator=(const RTextsbox &box);
    RTextsbox& operator=(const RTextsbox &&box);

    ~RTextsbox() = default;

    const RShaderProg& textsShader();
    Typeset typeset() const;
    RColor fontColor() const;
    const RFont& font() const;
    unsigned fontSize() const;
    float lineSpacing() const;
    float wordSpacing() const;
    float spacing() const;
    Format textFormat() const;
    const RTexture& textTexture() const;
    bool isSeting() const;
    const std::wstring& getTexts() const;

    void setFontColor(R_RGB rgb);
    void setFontColor(const RColor &color);
    void setFontColor(unsigned r, unsigned g, unsigned b);

    void setBackColor(R_RGBA rgba);
    void setBackColor(const RColor &color);
    void setBackColor(unsigned r, unsigned g, unsigned b, unsigned a = 255u);

    void setTexts(std::wstring texts);
    void setFontSize(unsigned size);
    void setFont(RFont font);
    void setlineSpacing(float value);
    void setWordSpacing(float value);
    void setSpacing(float value);
    void setTextFormat(Format format);
    void setEllipsis(bool b);
    void setTexture(const RTexture &tex);
    void setPixScale(float x, float y);

    void append(int code);
    void append(wchar_t c);
    void append(std::wstring str);

    void verticalTypeset();
    void horizontalTypeset();

    void reseting();
    void resetingNow();
    void complete();

    void update();
    void render();
    void render(const RShaderProg &shaders, GLuint mLoc);
    void edging(const RColor &color = RColor(0xff0000ff));
    void edging(const RShaderProg &shaders, GLuint mLoc);
    void edgingAll();
    void edgingAll(const RShaderProg &shaders, GLuint mLoc);

private:
    static Format fontFmt;
    thread_local static std::weak_ptr<RenderTool> tRenderTool;

    struct Vertexs { GLuint ao1, ao2, bo1, bo2; };

    void verticalTextToTexture();
    void horizontalTextToTexture();

    static Vertexs createVaertexObject();
    static RenderTool createRenderTool();

    std::shared_ptr<const RenderTool> renderTool_;
    RTexture backTex_;
    std::wstring texts_;
    std::array<glm::mat4, 2> model_;
    Format format_;
    RFont font_;

    RTexture textTex_;
    bool resetting_ = true;
    void (RTextsbox::*typesetting)() = &RTextsbox::horizontalTextToTexture;
};

} // Redopera

#endif // RTEXTBOX_H
