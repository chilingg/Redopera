#ifndef RTEXTBOX_H
#define RTEXTBOX_H

#include "RPoint.h"
#include "RSize.h"
#include "RMath.h"
#include "RColor.h"
#include "rsc/RImage.h"
#include "rsc/RFont.h"
#include "rsc/RTexture.h"

namespace Redopera {

// 使用utf-8编码保存字符串

class RTextbox
{
public:
    enum class Typeset
    {
        Horizontal,
        Vertical
    };

    enum class Align
    {
        Left,
        Right,
        Mind,
        Top,
        Bottom
    };

    struct Format
    {
        bool ellipsis = true;
        float lSpacing = 1.2f;
        float wSpacing = 1.0f;
        float spacing = 0.6f;   // 空格
        Typeset typeset = Typeset::Horizontal;
        struct {
            Align v, h;
        } align { Align::Top, Align::Left};
        RColor fcolor;
        struct {
            int t, b, l, r;
        } padding { 5, 5, 5, 5 };
    };

    static void setDefaultFontFmt(Format fmt);
    static const Format& getDefaultFontFmt();

    RTextbox();

    RTextbox(const std::wstring &text, int width, int height, const Format &fmt = getDefaultFontFmt());
    RTextbox(const std::wstring &text, const RSize &size, const Format &fmt = getDefaultFontFmt());
    RTextbox(const RTextbox &box);
    RTextbox(RTextbox &&box);

    RTextbox& operator=(const RTextbox &box);
    RTextbox& operator=(const RTextbox &&box);

    ~RTextbox() = default;

    bool isDirty() const;
    const RSize& size() const;
    RRect rect() const;
    const RTexture& texture();
    const RImage& image();
    const RFont& font() const;
    const Format& textFormat() const;
    const std::wstring& texts() const;

    RPoint& rPos();
    RSize& rSize();

    void setFontColor(RGBA rgba);
    void setFontColor(const RColor &color);
    void setFontColor(unsigned r, unsigned g, unsigned b);

    void setTexts(std::wstring texts);
    void setFontSize(unsigned size);
    void setFont(const RFont &font);
    void setTextFormat(const Format &format);
    void setAlign(Align v, Align h);
    void setLineSpacing(float value);
    void setWordSpacing(float value);

    void vertical();
    void horizontal();

    void updataTex();

private:
    static Format fontFmt;

    void verticalTextToTexture();
    void horizontalTextToTexture();

    bool typeset_ = true;
    void (RTextbox::*typesetting)();
    Format format_;
    std::wstring texts_;
    RFont font_;
    RSize size_;
    RTexture textTex_;

    RImage loader_;
};

} // Redopera

#endif // RTEXTBOX_H
