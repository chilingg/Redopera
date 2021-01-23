#include <rsc/RFont.h>
#include <rsc/RFile.h>
#include <RDebug.h>
#include <stb_truetype.h>
#include <SourceCodePro.h>

#include <cstring>
#include <atomic>

using namespace Redopera;

unsigned RFont::cacheMaxSize_ = 1000;
RFont RFont::defaultFont(SOURCE_FONT_DATA, SOURCE_FONT_SIZE);

RFont RFont::sourceCodePro()
{
    return RFont(SOURCE_FONT_DATA, SOURCE_FONT_SIZE);
}

void RFont::setCasheSize(unsigned size)
{
    cacheMaxSize_ = size;
}

void RFont::setDefaultFont(const RFont &font)
{
    defaultFont = font;
}

const RFont &RFont::getDefaulteFont()
{
    return defaultFont;
}

RFont::RFont():
    RFont(getDefaulteFont())
{

}

RFont::RFont(const std::string &path, unsigned fsize):
    cache_{ std::make_shared<std::map<RChar, Glyph>>(), fsize }
{
    load(path);
}

RFont::RFont(const RData *data, const size_t size, unsigned fsize):
    cache_{ std::make_shared<std::map<RChar, Glyph>>(), fsize }
{
    load(data, size);
}

RFont::RFont(const RFont &font):
    data_(font.data_),
    cache_(font.cache_)
{

}

RFont::RFont(const RFont &&font):
    data_{ std::move(font.data_.file), std::move(font.data_.info) },
    cache_{ std::move(font.cache_.caches), font.cache_.fsize }
{

}

RFont &RFont::operator=(RFont font)
{
    swap(font);
    return *this;
}

void RFont::swap(RFont &font)
{
    data_.file.swap(font.data_.file);
    data_.info.swap(font.data_.info);
    cache_.caches.swap(font.cache_.caches);
    using std::swap;
    swap(cache_.fsize, font.cache_.fsize);
}

bool RFont::isValid() const
{
    return data_.file != nullptr;
}

unsigned RFont::size() const
{
    return cache_.fsize;
}

const RFont::Glyph* RFont::getFontGlyph(RFont::RChar c) const
{
    Glyph &glyph = (*cache_.caches)[c];
    if(!glyph.data)
    {
        float scale = stbtt_ScaleForMappingEmToPixels(data_.info.get(), cache_.fsize);
        glyph.data.reset(stbtt_GetCodepointBitmap(
                    data_.info.get(), 0, scale,
                    c, &glyph.width, &glyph.height, &glyph.xoff, &glyph.yoff));
        stbtt_GetCodepointHMetrics(data_.info.get(), c, &glyph.advence, nullptr);
        glyph.advence *= scale;

        if(cache_.caches->size() > cacheMaxSize_)
            clearCache();
    }

    return &glyph;
}

void RFont::setSize(unsigned size)
{
    if (size != cache_.fsize)
    {
        cache_.caches = std::make_shared<std::map<RChar, Glyph>>();
        cache_.fsize = size;
    }
}

bool RFont::load(const std::string &path)
{
    RFile file = RFile::load(path);

    rCheck(!file.size, "Failed to load font in " + path);

    std::shared_ptr<stbtt_fontinfo> info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), file.data.get(), stbtt_GetFontOffsetForIndex(file.data.get(),0));

    if(rCheck(info->numGlyphs == 0, "Unknow font file in " + path))
        return false;

    data_ = { std::move(file.data), std::move(info) };
    return true;
}

bool RFont::load(const RData *data, size_t size)
{
    std::shared_ptr<stbtt_fontinfo> info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data, 0);

    if(rCheck(info->numGlyphs == 0, "Unknow font file"))
        return false;

    std::shared_ptr<RData[]> p(new RData[size]);
    std::memcpy(p.get(), data, size);
    data_ = { std::move(p), std::move(info) };
    return true;
}

void RFont::release()
{
    data_.file.reset();
    data_.info.reset();
    cache_.caches.reset();
}

void RFont::clearCache() const
{
    *cache_.caches = std::map<RChar, Glyph>();
}

void swap(RFont &font1, RFont &font2)
{
    font1.swap(font2);
}
