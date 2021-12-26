#include <RTextLoader.h>
#include <RRect.h>
#include <RFormat.h>

#include <vector>

using namespace Redopera;

namespace  {

RTextLoader::Format defaultFormat;

} // ns

void RTextLoader::setDefaultFontFormat(const Format &fmt)
{
    defaultFormat = fmt;
}

const RTextLoader::Format &RTextLoader::defaultFontFormat()
{
    return defaultFormat;
}

RTextLoader::RTextLoader(const Format &fmt, const RFont &font):
    fmt_(fmt),
    font_(font)
{

}

const RFont::Glyph &RTextLoader::getGlyph(RChar c)
{
    auto &glyph = cache_[c];

    if(!glyph.data)
        glyph = font_.getGlyph(c);

    return glyph;
}

RSize RTextLoader::getTextBoxSize(std::wstring_view texts)
{
    const int advenceLine = font_.size() * fmt_.lSpacing;
    int line = 0;
    int lineLength = advenceLine;
    int lineWidth = 0;
    int maxLineWidth = 0;

    for(auto c : texts)
    {
        switch(c)
        {
        case L'\n':
            ++line;
            maxLineWidth = std::max(lineWidth, maxLineWidth);
            lineWidth = 0;
            lineLength += advenceLine;
            break;
        case L' ':
            lineWidth += font_.size() * format().spacing;
            break;
        default:
            auto &glyph = getGlyph(c);
            lineWidth += glyph.advence * fmt_.wSpacing;
            break;
        }
    }
    if(texts.back() == L'\n')
        lineLength -= advenceLine;
    else
        maxLineWidth = std::max(lineWidth, maxLineWidth);

    return RSize(maxLineWidth + fmt_.padding.l + fmt_.padding.r,
                 lineLength + fmt_.padding.t + fmt_.padding.b);
}

void RTextLoader::setFontSize(unsigned size)
{
    cache_.clear();
    font_.setSize(size);
}

void RTextLoader::setFont(RFont font)
{
    cache_.clear();
    font_.swap(font);
}

void RTextLoader::setFormat(const Format &fmt)
{
    fmt_ = fmt;
}

void RTextLoader::setLineSpacing(float value)
{
    fmt_.lSpacing = value;
}

void RTextLoader::setWordSpacing(float value)
{
    fmt_.wSpacing = value;
}

void RTextLoader::setPadding(int t, int r, int b, int l)
{
    fmt_.padding = { t, r, b, l };
}

RImage RTextLoader::makeTextBoxBitmap(std::wstring_view texts)
{
    RImage textbox(nullptr, getTextBoxSize(texts), 1);
    textbox.fill(0);
    makeSimpleTextBoxBitmapAs(textbox, texts);

    return textbox;
}

void RTextLoader::makeSimpleTextBoxBitmapAs(RImage &textbox, std::wstring_view texts)
{
    const int advenceLine = font_.size() * fmt_.lSpacing;

    int point = fmt_.padding.l;
    int linePos = fmt_.padding.t + font_.size();

    for(auto c : texts)
    {
        switch(c)
        {
        case L'\n':
            point = fmt_.padding.l;
            linePos += advenceLine;
            break;
        case L' ':
            point += font_.size() * format().spacing;
            break;
        default:
            auto &glyph = getGlyph(c);
            int boundaryW = std::min(glyph.width + point + glyph.xoff, textbox.width() - fmt_.padding.l);
            int boundaryH = std::min(glyph.height + linePos + glyph.yoff, textbox.height() - fmt_.padding.b);

            for(int lp = linePos + glyph.yoff, i = 0; lp < boundaryH; ++lp, ++i)
            {
                for(int wp = point + glyph.xoff, j = 0; wp < boundaryW; ++wp, ++j)
                {
                    textbox.data()[lp * textbox.width() + wp] = glyph.data[i * glyph.width + j];
                }
            }
            point += glyph.advence * fmt_.wSpacing;

            break;
        }
    }
}

void RTextLoader::makeTextBoxBitmapAs(RImage &textbox, std::wstring_view texts, Align v, Align h, bool ellipsisTips)
{
    RRect area(fmt_.padding.l, fmt_.padding.t,
               textbox.width() - fmt_.padding.l - fmt_.padding.r,
               textbox.height() - fmt_.padding.t - fmt_.padding.b);

    if(rCheck(area.width() < font_.size() && area.height() < font_.size(), "Invalid use padding({}, {}, {}, {}) to make text box {}\n",
              fmt_.padding.t, fmt_.padding.r, fmt_.padding.b, fmt_.padding.l, area.size()))
        return;

    const int advenceLine = font_.size() * fmt_.lSpacing;
    const int spaceWidth = font_.size() * format().spacing;
    int lineLength = font_.size();

    std::vector<int> lineInfo(1, 0);

    for(size_t i = 0; i < texts.size();)
    {
        if(texts[i] == L'\n')
        {
            if (lineLength + advenceLine >= area.height())
                break;
            lineLength += advenceLine;
            lineInfo.push_back(0);
        }
        else if(texts[i] == L' ')
            lineInfo.back() += spaceWidth;
        else
        {
            auto &glyph = getGlyph(texts[i]);
            int advence = glyph.advence * fmt_.wSpacing;

            if(lineInfo.back() + advence > area.width())
            {
                if (lineLength + advenceLine > area.height())
                    break;
                lineLength += advenceLine;
                lineInfo.push_back(0);
                continue;
            }
            lineInfo.back() += advence;
        }
        ++i;
    }
    if(texts.back() == L'\n')
        lineInfo.pop_back();

    int linePos = fmt_.padding.t;
    if(v == Align::Bottom)
        linePos = area.top() - lineLength;
    else if(v == Align::Mind)
        linePos = (area.height() - lineLength) / 2 + fmt_.padding.t;
    linePos += font_.size();

    size_t tNum = 0;

    for(auto line : lineInfo)
    {
        int point = fmt_.padding.l;
        if(h == Align::Right)
            point = area.right() - line;
        else if(h == Align::Mind)
            point = (area.width() - line) / 2 + fmt_.padding.l;

        line += point;

        while(tNum < texts.size())
        {
            if(texts[tNum] == L'\n')
            {
                ++tNum;
                break;
            }
            else if(point == line)
                break;
            else if(texts[tNum] == L' ')
                point += spaceWidth;
            else
            {
                auto &glyph = getGlyph(texts[tNum]);
                int boundaryW = std::min(glyph.width + point + glyph.xoff, area.right());
                int boundaryH = std::min(glyph.height + linePos + glyph.yoff, area.top());

                for(int lp = linePos + glyph.yoff, i = 0; lp < boundaryH; ++lp, ++i)
                {
                    for(int wp = point + glyph.xoff, j = 0; wp < boundaryW; ++wp, ++j)
                    {
                        textbox.data()[lp * textbox.width() + wp] = glyph.data[i * glyph.width + j];
                    }
                }
                point += glyph.advence * fmt_.wSpacing;
            }
            ++tNum;
        }
        linePos += advenceLine;
    }

    if(tNum != texts.size() && ellipsisTips)
    {
        int incre = 0;
        int startX = std::max(textbox.width() - 8, 0);
        int endY = std::max(textbox.height() - 8, 0);

        for(int y = textbox.height() - 1; y >= endY; --y)
        {
            for(int x = startX + incre ; x < textbox.width(); ++x)
            {
                textbox.data()[y * textbox.width() + x] = '\xff';
            }
            ++incre;
        }
    }
}
