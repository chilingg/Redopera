#ifndef RTEXTBOX_H
#define RTEXTBOX_H

#include "RArea.h"
#include "RColor.h"
#include "RMath.h"
#include "rsc/RFont.h"
#include "rsc/RTexture.h"
#include "rsc/RShaderProg.h"

#include <codecvt>
#include <locale>

namespace Redopera {

// 使用utf-8编码保存字符串

class RTextsbxo : public RArea
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
        glm::vec3 color = { 1.f, 1.f, 1.f };
        RFont font;
        bool ellipsis = true;   //若不能显示所有字符，在末尾添加一个5x5方框
    };

    struct RenderTool
    {
        RShaderProg &shaders;
        GLuint &modelLoc;
        GLuint &colorLoc;
        GLuint &textLoc;
        GLuint &edgingLoc;
    };

    static const RenderTool& textboxRenderTool();
    static void setTextBoxShadersAsThread(const RShaderProg &shaders, GLuint modelLoc, GLuint edgingLoc = -1);
    static const RShaderProg& textboxShader();

    static std::wstring_convert<std::codecvt_utf8<wchar_t>> strcnv;

    RTextsbxo();

    RTextsbxo(const std::wstring &text, int width, int height, int x, int y, int z = 0);
    RTextsbxo(const std::string &text, int width, int height, int x, int y, int z = 0);

    RTextsbxo(const std::wstring &text, int width, int height, const RPoint &pos);
    RTextsbxo(const std::string &text, int width, int height, const RPoint &pos);

    RTextsbxo(const std::wstring &text, const RSize &size, const RPoint &pos);
    RTextsbxo(const std::string &text, const RSize &size, const RPoint &pos);

    RTextsbxo(const std::wstring &text, const RRect &rect, int z = 0);
    RTextsbxo(const std::string &text, const RRect &rect, int z = 0);

    RTextsbxo(const std::wstring &text, const RArea::Format &area);
    RTextsbxo(const std::string &text, const RArea::Format &area);

    RTextsbxo(const RTextsbxo &box);
    RTextsbxo(RTextsbxo &&box);

    RTextsbxo& operator=(const RTextsbxo &box);

    RTextsbxo& operator=(const RTextsbxo &&box);

    ~RTextsbxo() override = default;

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

    void setFontColor(R_RGBA rgba);
    void setFontColor(const RColor &color);
    void setFontColor(unsigned r, unsigned g, unsigned b);

    void setBackColor(R_RGBA rgba);
    void setBackColor(const RColor &color);
    void setBackColor(unsigned r, unsigned g, unsigned b, unsigned a = 255u);

    void setTexts(std::wstring texts);
    void setTexts(std::string texts);
    void setFontSize(unsigned size);
    void setFont(RFont font);
    void setlineSpacing(float value);
    void setWordSpacing(float value);
    void setSpacing(float value);
    void setTextFormat(Format format);
    void setEllipsis(bool b);
    void setTexture(const RTexture &tex);
    void setTextureName(const std::string &name);

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
    thread_local static RShaderProg tTextShaders;
    thread_local static GLuint MODEL_LOC;
    thread_local static GLuint COLOR_LOC;
    thread_local static GLuint TEXT_LOC;
    thread_local static GLuint EDGING_LOC;

    void verticalTextToTexture();
    void horizontalTextToTexture();

    std::vector<RData> loader_;
    RTexture backTex_;
    RTexture textTex_;
    std::wstring texts_;
    Format format_;

    std::array<glm::mat4, 2> model_;
    bool resetting_ = true;
    void (RTextsbxo::*typesetting)() = &RTextsbxo::horizontalTextToTexture;
};

} // Redopera

#endif // RTEXTBOX_H
