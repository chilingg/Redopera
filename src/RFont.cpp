#include <rsc/RFont.h>
#include <rsc/RFile.h>

#include <stb/stb_truetype.h>

using namespace Redopera;

RFont &RFont::defaultFont()
{
    static RFont font(sourceCodePro());
    return font;
}

RFont::RFont():
    RFont(defaultFont())
{

}

RFont::RFont(const std::string &path, unsigned fsize):
    fsize_(fsize)
{
    load(path);
}

RFont::RFont(const RData *data, const size_t size, unsigned fsize):
    fsize_(fsize)
{
    load(data, size);
}

RFont::RFont(const RFont &font):
    fsize_(font.fsize_),
    info_(font.info_),
    file_(font.file_)
{

}

RFont::RFont(const RFont &&font):
    fsize_(font.fsize_),
    info_(std::move(font.info_)),
    file_(std::move(font.file_))
{

}

RFont &RFont::operator=(RFont font)
{
    swap(font);
    return *this;
}

void RFont::swap(RFont &font)
{
    std::swap(fsize_, font.fsize_);
    std::swap(info_, font.info_);
    std::swap(file_, font.file_);
}

bool RFont::isValid() const
{
    return info_.operator bool();
}

unsigned RFont::size() const
{
    return fsize_;
}

RFont::Glyph RFont::getGlyph(RChar c) const
{
    Glyph glyph;

    float scale = stbtt_ScaleForMappingEmToPixels(info_.get(), fsize_);
    glyph.data.reset(stbtt_GetCodepointBitmap(info_.get(), 0, scale, c, &glyph.width, &glyph.height, &glyph.xoff, &glyph.yoff));
    stbtt_GetCodepointHMetrics(info_.get(), c, &glyph.advence, nullptr);
    glyph.advence *= scale;

    return glyph;
}

void RFont::setSize(unsigned size)
{
    fsize_ = size;
}

bool RFont::load(const std::string &path)
{
    std::unique_ptr<RData[]> data = RFile::load(path);
    if(!data)
        return false;

    auto info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data.get(), stbtt_GetFontOffsetForIndex(data.get(), 0));

    if(rCheck(info->numGlyphs == 0, "Unknow font file in {}!\n", path))
        return false;

    info_ = std::move(info);
    file_.reset(data.release());
    return true;
}

bool RFont::load(const RData *data, size_t size)
{
    auto info = std::make_shared<stbtt_fontinfo>();
    stbtt_InitFont(info.get(), data, 0);

    if(rCheck(info->numGlyphs == 0, "Unknow font file!\n"))
        return false;


    //file_ = std::make_shared<RData[]>(size);
    file_ = std::shared_ptr<RData[]> (new RData[size]);
    std::memcpy(file_.get(), data, size);
    info_ = std::move(info);
    return true;
}

void RFont::free()
{
    info_.reset();
    file_.reset();
}

void swap(RFont &font1, RFont &font2)
{
    font1.swap(font2);
}
