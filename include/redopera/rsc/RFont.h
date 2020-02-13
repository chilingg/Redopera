#ifndef RFONT_H
#define RFONT_H

#include "RResource.h"
#include "../dependents/stb_truetype.h"

#include <vector>
#include <map>

namespace Redopera {

// RFont在不同线程拥有各自的缺省设置

class RFont : public RResource
{
    using RChar = int;

    friend void swap(RFont &font1, RFont &font2);

public:
    struct Glyph
    {
        int width = 0;
        int height = 0;
        int xoff = 0;
        int yoff = 0;
        std::unique_ptr<const RData[]> data = nullptr;
    };

    struct FontResource
    {
        std::shared_ptr<RData[]> font;
        stbtt_fontinfo info;
    };

    // sourceCodePro()定义在字体资源文件中 (SourceCodePro.cpp)
    static RFont sourceCodePro();

    static void setCasheSize(unsigned size);
    static void setDefaultFontSize(unsigned size);
    static void setDefaultFont(const RFont &font);
    static const RFont& getDefaulteFont();

    RFont();
    explicit RFont(const std::string &path, const std::string &name = "Font", unsigned fsize = 14);
    RFont(const std::shared_ptr<RData[]> &data, const std::string &name = "Font", unsigned fsize = 14);
    RFont(const RFont &font);
    RFont(const RFont &&font);
    RFont& operator=(RFont font);
    void swap(RFont &font);
    ~RFont() = default;

    bool isValid() const;
    unsigned size() const;
    const Glyph *getFontGlyph(RChar c) const;

    void setSize(unsigned size);
    bool load(const std::string &path);
    bool load(const std::shared_ptr<RData[]> &data);
    void release();
    void clearFontDataCache() const;

private:
    // defaultFont定义在字体资源文件中 (SourceCodePro.cpp)
    thread_local static RFont defaultFont;

    static unsigned cacheMaxSize_;

    std::shared_ptr<FontResource> resource_;
    std::shared_ptr<std::map<RChar, Glyph>> caches_;
    unsigned size_ = 14;
};

} // Redopera

void swap(Redopera::RFont &font1, Redopera::RFont &font2);

#endif // RFONT_H
