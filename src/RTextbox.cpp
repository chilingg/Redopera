#include <RTextbox.h>
#include <RRect.h>
#include <rsc/RImage.h>

#include <vector>

using namespace Redopera;

RTextbox::Format RTextbox::fontFmt;

void RTextbox::setDefaultFontFmt(RTextbox::Format fmt)
{
    fontFmt = fmt;
}

const RTextbox::Format &RTextbox::getDefaultFontFmt()
{
    return fontFmt;
}

RTextbox::RTextbox():
    RTextbox(L"Text Label", 90, 25)
{

}

RTextbox::RTextbox(const std::wstring &text, int width, int height, const Format &fmt):
    RTextbox(text, RSize(width, height), fmt)
{

}

RTextbox::RTextbox(const std::wstring &text, const RSize &size, const Format &fmt):
    typesetting(fmt.typeset == Typeset::Vertical ?
                    &RTextbox::verticalTextToTexture : &RTextbox::horizontalTextToTexture),
    format_(fmt),
    texts_(text),
    size_(size)
{

}


RTextbox::RTextbox(const RTextbox &box):
    typeset_(box.typeset_),
    typesetting(box.typesetting),
    format_(box.format_),
    texts_(box.texts_),
    font_(box.font_),
    size_(box.size_),
    textTex_(box.textTex_)
{

}

RTextbox::RTextbox(RTextbox &&box):
    typeset_(box.typeset_),
    typesetting(box.typesetting),
    format_(std::move(box.format_)),
    texts_(std::move(box.texts_)),
    font_(std::move(box.font_)),
    size_(box.size_),
    textTex_(std::move(box.textTex_))
{

}

RTextbox &RTextbox::operator=(const RTextbox &box)
{
    typeset_ = box.typeset_;
    typesetting = box.typesetting;
    format_ = box.format_;
    texts_ = box.texts_;
    font_ = box.font_;
    size_ = box.size_;
    textTex_ = box.textTex_;
    return *this;
}

RTextbox &RTextbox::operator=(const RTextbox &&box)
{
    typeset_ = box.typeset_;
    typesetting = box.typesetting;
    format_ = std::move(box.format_);
    texts_ = std::move(box.texts_);
    font_ = std::move(box.font_);
    size_ = box.size_;
    textTex_ = std::move(box.textTex_);
    return *this;
}

bool RTextbox::isDirty() const
{
    return typeset_;
}

const RSize &RTextbox::size() const
{
    return size_;
}

const RTexture &RTextbox::texture()
{
    if(typeset_)
        updataTex();

    return textTex_;
}

const RImage &RTextbox::image()
{
    if(typeset_)
        updataTex();

    return loader_;
}

const RFont &RTextbox::font() const
{
    return font_;
}

const RTextbox::Format &RTextbox::textFormat() const
{
    return format_;
}

const std::wstring &RTextbox::texts() const
{
    return texts_;
}

RSize &RTextbox::rSize()
{
    typeset_ = true;
    return size_;
}

void RTextbox::setFontColor(RGBA rgba)
{
    setFontColor(RColor(rgba));
}

void RTextbox::setFontColor(const RColor &color)
{
    format_.fcolor = color;
    typeset_ = true;
}

void RTextbox::setFontColor(unsigned r, unsigned g, unsigned b)
{
    setFontColor(RColor(r, g, b));
}

void RTextbox::setTexts(std::wstring texts)
{
    texts_ = texts;
    typeset_ = true;
}

void RTextbox::setFontSize(unsigned size)
{
    font_.setSize(size);
    typeset_ = true;
}

void RTextbox::setFont(const RFont &font)
{
    font_ = font;
    typeset_ = true;
}

void RTextbox::setTextFormat(const Format &format)
{
    typesetting = format.typeset == Typeset::Vertical ?
                &RTextbox::verticalTextToTexture : &RTextbox::horizontalTextToTexture;
    format_ = format;
    typeset_ = true;
}

void RTextbox::setAlign(RTextbox::Align v, RTextbox::Align h)
{
    format_.align = { v, h };
    typeset_ = true;
}

void RTextbox::setLineSpacing(float value)
{
    format_.lSpacing = value;
    typeset_ = true;
}

void RTextbox::setWordSpacing(float value)
{
    format_.wSpacing = value;
    typeset_ = true;
}

void RTextbox::vertical()
{
    typesetting = &RTextbox::verticalTextToTexture;
    format_.typeset = Typeset::Vertical;
    typeset_ = true;
}

void RTextbox::horizontal()
{
    typesetting = &RTextbox::horizontalTextToTexture;
    format_.typeset = Typeset::Horizontal;
    typeset_ = true;
}

void RTextbox::updataTex()
{
    if (loader_.size() != size_)
        loader_.load(nullptr, size_.width(), size_.height(), 4);

    uint32_t *p = reinterpret_cast<uint32_t*>(loader_.data());
    std::fill(p, p + (size_.width() * size().height()), (format_.fcolor.bgr()));

    (this->*typesetting)();
    typeset_ = false;
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

void RTextbox::verticalTextToTexture()
{
    RRect area(size_.width() - format_.padding.l - format_.padding.r,
               size_.height() - format_.padding.t - format_.padding.b,
               format_.padding.l,
               format_.padding.t);
    const int advanceL = font_.size() * format_.lSpacing; // 行步进
    int linepos = format_.padding.r;
    int fsize = static_cast<int>(font_.size());
    bool ellipsis = true;

    if (area.width() > fsize && area.height() > fsize)
    {
        // 计算每行的行宽
        std::vector<int> lines;
        lines.reserve(8);
        lines.push_back(0);
        for (size_t i = 0; i < texts_.size();)
        {
            if (texts_[i] == L'\n')
            {
                ++i;
                newline:
                linepos += advanceL;
                if (linepos + fsize >= area.width())
                    break;

                lines.push_back(0);
                continue;
            }
            else if (texts_[i] == L' ')
            {
                lines.back() += format_.spacing * fsize;
            }
            else
            {
                const RFont::Glyph *glyph = font().getFontGlyph(texts_[i]);
                if (lines.back() + fsize > area.height())
                    goto newline;
                lines.back() += glyph->advence * format_.wSpacing;
            }
            ++i;
        }

        if(format_.align.h == Align::Left) linepos = area.left() + ((lines.size()-1) * advanceL + fsize);
        else if(format_.align.h == Align::Mind) linepos = (((lines.size()-1) * advanceL + fsize) + size_.width()) / 2;
        else linepos = area.right();

        int wordpos;
        size_t textNum = 0;

        for (auto linew : lines)
        {
            if(format_.align.v == Align::Bottom) wordpos = area.top() - linew;
            else if(format_.align.v == Align::Mind) wordpos = (size_.height() - linew) / 2;
            else wordpos = area.bottom();

            linew += wordpos;

            while(textNum < texts_.size())
            {
                if (texts_[textNum] == L'\n')
                {
                    linepos -= advanceL;
                    ++textNum;
                    break;
                }
                else if (wordpos == static_cast<int>(linew))
                {
                    linepos -= advanceL;
                    break;
                }
                else if (texts_[textNum] == L' ')
                {
                    wordpos += format_.spacing * fsize;
                }
                else
                {
                    const RFont::Glyph* glyph = font_.getFontGlyph(texts_[textNum]);
                    RFont::Glyph letter;

                    if((texts_[textNum] > L'\x20' && texts_[textNum] < L'\x7f'))
                    {
                        auto data = std::make_unique<RData[]>(glyph->width * glyph->height);
                        rotate90(data.get(), glyph->data.get(), glyph->width, glyph->height);
                        letter = { glyph->height, glyph->width,
                                   -(glyph->height + glyph->yoff),
                                   -(fsize - glyph->xoff),
                                   glyph->advence };
                        letter.data.reset(data.release());
                        glyph = &letter;
                    }

                    int startx = linepos - (fsize + glyph->width - glyph->xoff) / 2;
                    int starty = wordpos + fsize + glyph->yoff;

                    for(int y = 0; y < glyph->height; ++y)
                    {
                        for(int x = 0; x < glyph->width; ++x)
                        {
                            loader_.data()[(starty + y) * size_.width() * 4 + (startx + x) * 4 + 3]
                                    = glyph->data.get()[y * glyph->width + x];
                        }
                    }
                    wordpos += glyph->advence * format_.wSpacing;
                }
                ++textNum;
            }
        }
        if (textNum == texts_.size())
            ellipsis = false;
    }

    if(format_.ellipsis && ellipsis)
    {
        int incre = 1;
        for(int y = size_.height() - 5; y < size_.height(); ++y)
        {
            for(int x = 0; x < incre; ++x)
            {
                loader_.data()[y * size().width() * 4 + x * 4] = '\xff';
                loader_.data()[y * size().width() * 4 + x * 4 + 1] = '\x00';
                loader_.data()[y * size().width() * 4 + x * 4 + 2] = '\x00';
                loader_.data()[y * size().width() * 4 + x * 4 + 3] = '\xff';
            }
            ++incre;
        }
    }

    loader_.flipV();
    if (size_ == textTex_.size())
        textTex_.reload(loader_.data());
    else
        textTex_.load(loader_.data(), size_.width(), size_.height(), 4, RTexture::Linear4);
}

void RTextbox::horizontalTextToTexture()
{
    RRect area(size_.width() - format_.padding.l - format_.padding.r,
               size_.height() - format_.padding.t - format_.padding.b,
               format_.padding.l,
               format_.padding.t);
    const int advanceL = font_.size() * format_.lSpacing; // 行步进
    int linepos = area.bottom();
    int fsize = static_cast<int>(font_.size());
    bool ellipsis = true;

    if (area.width() > fsize && area.height() > fsize)
    {
        // 计算每行的行宽
        std::vector<int> lines;
        lines.reserve(8);
        lines.push_back(0);
        for (size_t i = 0; i < texts_.size();)
        {
            if (texts_[i] == L'\n')
            {
                ++i;
                newline:
                linepos += advanceL;
                if (linepos + fsize >= area.height())
                    break;

                lines.push_back(0);
                continue;
            }
            else if (texts_[i] == L' ')
            {
                lines.back() += format_.spacing * fsize;
            }
            else
            {
                const RFont::Glyph* glyph = font_.getFontGlyph(texts_[i]);
                if (lines.back() + glyph->advence * format_.wSpacing > area.width())
                    goto newline;
                lines.back() += glyph->advence * format_.wSpacing;
            }
            ++i;
        }

        if (format_.align.v == Align::Bottom) linepos = area.top() - ((lines.size()-1) * advanceL + fsize);
        else if (format_.align.v == Align::Mind) linepos = (size_.height() - ((lines.size()-1) * advanceL + fsize)) / 2;
        else linepos = area.bottom(); // Top 缺省默认

        int wordpos;
        size_t textNum = 0;

        for (auto linew : lines)
        {
            if(format_.align.h == Align::Right) wordpos = area.right() - linew;
            else if(format_.align.h == Align::Mind) wordpos = (size_.width() - linew) / 2;
            else wordpos = area.left(); // Align_Left 缺省默认

            linew += wordpos;

            while(textNum < texts_.size())
            {
                if (texts_[textNum] == L'\n')
                {
                    linepos += advanceL;
                    ++textNum;
                    break;
                }
                else if (wordpos == static_cast<int>(linew))
                {
                    linepos += advanceL;
                    break;
                }
                else if (texts_[textNum] == L' ')
                {
                    wordpos += format_.spacing * fsize;
                }
                else
                {
                    const RFont::Glyph* glyph = font_.getFontGlyph(texts_[textNum]);

                    int startx = wordpos + glyph->xoff;
                    int starty = linepos + fsize + glyph->yoff;

                    for(int y = 0; y < glyph->height; ++y)
                    {
                        for(int x = 0; x < glyph->width; ++x)
                        {
                            loader_.data()[(starty + y) * size_.width() * 4 + (startx + x) * 4 + 3]
                                    = glyph->data.get()[y * glyph->width + x];
                        }
                    }
                    wordpos += glyph->advence * format_.wSpacing;
                }
                ++textNum;
            }
        }
        if (textNum == texts_.size())
            ellipsis = false;
    }

    if(format_.ellipsis && ellipsis)
    {
        int incre = 1;
        for(int y = size_.height() - 5; y < size_.height(); ++y)
        {
            for(int x = size_.width() - incre; x < size_.width(); ++x)
            {
                loader_.data()[y * size().width() * 4 + x * 4] = '\xff';
                loader_.data()[y * size().width() * 4 + x * 4 + 1] = '\x00';
                loader_.data()[y * size().width() * 4 + x * 4 + 2] = '\x00';
                loader_.data()[y * size().width() * 4 + x * 4 + 3] = '\xff';
            }
            ++incre;
        }
    }

    loader_.flipV();
    if (size_ == textTex_.size())
        textTex_.reload(loader_.data());
    else
        textTex_.load(loader_.data(), size_.width(), size_.height(), 4, RTexture::Linear4);
}
