#ifndef RTEXTLOADER_H
#define RTEXTLOADER_H

#include "rsc/RFont.h"
#include "rsc/RImage.h"
#include "RSize.h"
#include <unordered_map>

namespace Redopera {

class RTextLoader
{
public:
    enum class Align
    {
        Left,
        Top = Left,
        Mind,
        Right,
        Bottom = Right
    };

    struct Format
    {
        float lSpacing = 1.2f;
        float wSpacing = 1.0f;
        float spacing = 0.6f;   // 空格
        struct {
            int t, r, b, l;
        } padding { 0, 0, 0, 0 };
    };

    static void setDefaultFontFormat(const Format &fmt);
    static const Format& defaultFontFormat();

    RTextLoader() = default;
    RTextLoader(const Format &fmt, const RFont &font = RFont());

    RTextLoader(RTextLoader&) = delete;
    RTextLoader& operator=(const RTextLoader&) = delete;
    ~RTextLoader() = default;

    const Format& format() const { return fmt_; }
    const RFont& font() { return font_; }
    const RFont::Glyph& getGlyph(RChar c);
    RSize getTextBoxSize(std::wstring_view texts);

    void setFontSize(unsigned size);
    void setFont(RFont font);
    void setFormat(const Format &fmt);
    void setLineSpacing(float value);
    void setWordSpacing(float value);
    void setPadding(int t, int r, int b, int l);

    void makeSimpleTextBoxBitmapAs(RImage &textbox, std::wstring_view texts);
    RImage makeTextBoxBitmap(std::wstring_view texts);
    void makeTextBoxBitmapAs(RImage &textbox, std::wstring_view texts, Align v, Align h, bool ellipsisTips = true);

private:
    Format fmt_;
    RFont font_;
    std::unordered_map<wchar_t, RFont::Glyph> cache_;
};

} // ns Redopera

#endif // RTEXTLOADER_H
