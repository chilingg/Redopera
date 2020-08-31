#include <rsc/RFont.h>
#include <RDebug.h>
#include <dependents/stb_truetype.h>
#include <dependents/SourceCodePro.h>

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
    cache_ = { std::make_shared<std::map<RChar, Glyph>>(), size };
}

bool RFont::load(std::string path)
{
    RResource::rscPath(path);

    FILE *fp = fopen(path.c_str(), "rb");
    check(!fp, "Failed to load font in " + path);

    fseek(fp, 0L, SEEK_END);
    auto size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    std::unique_ptr<RData[]> data(std::make_unique<RData[]>(size));
    fread(data.get(), 1, size, fp);
    fclose(fp);

    std::shared_ptr<stbtt_fontinfo> info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data.get(), stbtt_GetFontOffsetForIndex(data.get(),0));

    if(check(info->numGlyphs == 0, "Unknow font file in " + path))
        return false;

    data_ = { std::move(data), std::move(info) };
    return true;
}

bool RFont::load(const RData *data, size_t size)
{
    std::shared_ptr<stbtt_fontinfo> info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data, 0);

    if(check(info->numGlyphs == 0, "Unknow font file"))
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
