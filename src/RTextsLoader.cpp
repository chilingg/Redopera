#include <RTextsLoader.h>
#include <RRect.h>
#include <vector>

using namespace Redopera;

RTextsLoader::Format RTextsLoader::fontFmt;

void RTextsLoader::setDefaultFontFmt(const Format &fmt)
{
    fontFmt = fmt;
}

const RTextsLoader::Format &RTextsLoader::defaultFontFmt()
{
    return fontFmt;
}

RTextsLoader::RTextsLoader():
    RTextsLoader(L"Input Texts", 90, 25)
{

}

RTextsLoader::RTextsLoader(const std::wstring &text, int width, int height, const Format &fmt):
    RTextsLoader(text, RSize(width, height), fmt)
{

}

RTextsLoader::RTextsLoader(const std::wstring &text, const RSize &size, const Format &fmt):
    typesetting(fmt.typeset == Typeset::Vertical ?
                    &RTextsLoader::verticalTextToTexture : &RTextsLoader::horizontalTextToTexture),
    format_(fmt),
    texts_(text),
    size_(size)
{

}


RTextsLoader::RTextsLoader(const RTextsLoader &box):
    dirty_(box.dirty_),
    typesetting(box.typesetting),
    format_(box.format_),
    texts_(box.texts_),
    size_(box.size_)
{

}

RTextsLoader::RTextsLoader(RTextsLoader &&box):
    dirty_(box.dirty_),
    typesetting(box.typesetting),
    format_(std::move(box.format_)),
    texts_(std::move(box.texts_)),
    size_(box.size_)
{

}

RTextsLoader &RTextsLoader::operator=(const RTextsLoader &box)
{
    dirty_ = box.dirty_;
    typesetting = box.typesetting;
    format_ = box.format_;
    texts_ = box.texts_;
    size_ = box.size_;
    return *this;
}

RTextsLoader &RTextsLoader::operator=(const RTextsLoader &&box)
{
    dirty_ = box.dirty_;
    typesetting = box.typesetting;
    format_ = std::move(box.format_);
    texts_ = std::move(box.texts_);
    size_ = box.size_;
    return *this;
}

bool RTextsLoader::isDirty() const
{
    return dirty_;
}

const RSize &RTextsLoader::size() const
{
    return size_;
}

const RTexture &RTextsLoader::texture() const
{
    if(dirty_)
        const_cast<RTextsLoader*>(this)->updataTex();

    return tex_;
}

const RImage &RTextsLoader::image() const
{
    if(dirty_)
        const_cast<RTextsLoader*>(this)->updataTex();

    return img_;
}

const RFont &RTextsLoader::font() const
{
    return font_;
}

const RTextsLoader::Format &RTextsLoader::textFormat() const
{
    return format_;
}

const std::wstring &RTextsLoader::texts() const
{
    return texts_;
}

RSize &RTextsLoader::rSize()
{
    dirty_ = true;
    return size_;
}

void RTextsLoader::setTexts(std::wstring texts)
{
    texts_ = texts;
    dirty_ = true;
}

void RTextsLoader::setFontSize(unsigned size)
{
    font_.setSize(size);
    dirty_ = true;
}

void RTextsLoader::setFont(const RFont &font)
{
    font_ = font;
    dirty_ = true;
}

void RTextsLoader::setTextsFormat(const Format &format)
{
    typesetting = format.typeset == Typeset::Vertical ?
                &RTextsLoader::verticalTextToTexture : &RTextsLoader::horizontalTextToTexture;
    format_ = format;
    dirty_ = true;
}

void RTextsLoader::setAlign(RTextsLoader::Align v, RTextsLoader::Align h)
{
    format_.align = { v, h };
    dirty_ = true;
}

void RTextsLoader::setLineSpacing(float value)
{
    format_.lSpacing = value;
    dirty_ = true;
}

void RTextsLoader::setWordSpacing(float value)
{
    format_.wSpacing = value;
    dirty_ = true;
}

void RTextsLoader::vertical()
{
    typesetting = &RTextsLoader::verticalTextToTexture;
    format_.typeset = Typeset::Vertical;
    dirty_ = true;
}

void RTextsLoader::horizontal()
{
    typesetting = &RTextsLoader::horizontalTextToTexture;
    format_.typeset = Typeset::Horizontal;
    dirty_ = true;
}

void RTextsLoader::updataTex()
{
    if (img_.size() != size_)
        img_.load(nullptr, size_.width(), size_.height(), 1);

    std::fill(img_.data(), img_.data() + (size_.width() * size().height()), '\0');

    (this->*typesetting)();
    dirty_ = false;
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

void RTextsLoader::verticalTextToTexture()
{
    RRect area(format_.padding.l, format_.padding.t,
               size_.width() - format_.padding.l - format_.padding.r,
               size_.height() - format_.padding.t - format_.padding.b);
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
                            img_.data()[(starty + y) * size_.width() + (startx + x)] = glyph->data.get()[y * glyph->width + x];
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
                img_.data()[y * size().width() + x] = '\xff';
            }
            ++incre;
        }
    }

    img_.flipV();
    if (size_ == tex_.size())
        tex_.reload(img_.data());
    else
        tex_.load(img_.data(), size_.width(), size_.height(), 1, RTexture::SingleL);
}

void RTextsLoader::horizontalTextToTexture()
{
    RRect area(format_.padding.l, format_.padding.t,
               size_.width() - format_.padding.l - format_.padding.r,
               size_.height() - format_.padding.t - format_.padding.b);
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
                            img_.data()[(starty + y) * size_.width() + (startx + x)] = glyph->data.get()[y * glyph->width + x];
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
                img_.data()[y * size().width() + x] = '\xff';
            }
            ++incre;
        }
    }

    img_.flipV();
    if (size_ == tex_.size())
        tex_.reload(img_.data());
    else
        tex_.load(img_.data(), size_.width(), size_.height(), 1, RTexture::SingleL);
}
