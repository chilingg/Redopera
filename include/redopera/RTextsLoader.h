#ifndef RTEXTSLOADER_H
#define RTEXTSLOADER_H

#include "RSize.h"
#include "rsc/RImage.h"
#include "rsc/RFont.h"
#include "rsc/RTexture.h"

namespace Redopera {

// 使用utf-8编码保存字符串

class RTextsLoader
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
        struct {
            int t, b, l, r;
        } padding { 5, 5, 5, 5 };
    };

    static void setDefaultFontFmt(const Format &fmt);
    static const Format& defaultFontFmt();

    RTextsLoader();

    RTextsLoader(const std::wstring &text, int width, int height, const Format &fmt = fontFmt);
    RTextsLoader(const std::wstring &text, const RSize &size, const Format &fmt = fontFmt);
    RTextsLoader(const RTextsLoader &timg);
    RTextsLoader(RTextsLoader &&timg);

    RTextsLoader& operator=(const RTextsLoader &timg);
    RTextsLoader& operator=(const RTextsLoader &&timg);

    ~RTextsLoader() = default;

    bool isDirty() const;
    const RSize& size() const;
    const RTexture& texture() const;
    const RImage& image() const;
    const RFont& font() const;
    const Format& textFormat() const;
    const std::wstring& texts() const;

    RSize& rSize();

    void setSize(int width, int height);
    void setSize(const RSize &size);
    void setTexts(std::wstring texts);
    void setFontSize(unsigned size);
    void setFont(const RFont &font);
    void setTextsFormat(const Format &format);
    void setAlign(Align v, Align h);
    void setLineSpacing(float value);
    void setWordSpacing(float value);

    void vertical();
    void horizontal();

private:
    void updataTex();

    static Format fontFmt;

    void verticalTextToTexture();
    void horizontalTextToTexture();

    bool dirty_ = true;
    void (RTextsLoader::*typesetting)();
    Format format_;
    std::wstring texts_;
    RSize size_;

    RTexture tex_;
    RImage img_;
    RFont font_;
};

} // Redopera

#endif // RTEXTSLOADER_H
