#include "rsc/RFont.h"
#include "RDebug.h"
#include "dependents/stb_truetype.h"
#include "cstring"

#include <fstream>
#include <atomic>

using namespace Redopera;

unsigned RFont::cacheMaxSize_ = 1000;
std::unique_ptr<RFont> RFont::defaultFont(nullptr);

void RFont::setCasheSize(unsigned size)
{
    cacheMaxSize_ = size;
}

void RFont::setDefaultFontSize(unsigned size)
{
    if(!defaultFont)
        getDefaulteFont();
    defaultFont->setSize(size);
}

const RFont &RFont::getDefaulteFont()
{
    if(!defaultFont)
        defaultFont = std::make_unique<RFont>(sourceCodePro());

    return *defaultFont;
}

void RFont::setDefaultFont(const RFont &font)
{
    defaultFont = std::make_unique<RFont>(font);
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
        glyph.data.reset(stbtt_GetCodepointBitmap(
                    data_.info.get(), 0, stbtt_ScaleForMappingEmToPixels(data_.info.get(), cache_.fsize),
                    c, &glyph.width, &glyph.height, &glyph.xoff, &glyph.yoff));

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
    RResource::rscpath(path);

    std::shared_ptr<RData[]> data;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path, std::ios::binary | std::ios::ate);
        size_t size = file.tellg();
        data.reset(new RData[size]);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(data.get()), size);
        file.close();
    }
    catch(...)
    {
        prError("Failed to load font in " + path);
        return false;
    }

    std::shared_ptr<stbtt_fontinfo> info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data.get(), 0);

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
