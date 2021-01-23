#ifndef RFONT_H
#define RFONT_H

#include <map>
#include <string>
#include <memory>

class stbtt_fontinfo;

namespace Redopera {

using RData = uint8_t;

class RFont
{
    using RChar = int;

public:
    struct Glyph
    {
        int width = 0;
        int height = 0;
        int xoff = 0;
        int yoff = 0;
        int advence = 0;
        std::unique_ptr<const RData[]> data = nullptr;
    };

    // sourceCodePro()定义在字体资源文件中 (SourceCodePro.cpp)
    static RFont sourceCodePro();

    static void setCasheSize(unsigned size);
    static void setDefaultFont(const RFont &font);
    static const RFont& getDefaulteFont();

    RFont();
    explicit RFont(const std::string &path, unsigned fsize = 14);
    RFont(const RData* data, const size_t size, unsigned fsize = 14);
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
    bool load(const RData *data, size_t size);
    void release();
    void clearCache() const;

private:
    static RFont defaultFont;
    static unsigned cacheMaxSize_;

    struct {
        std::shared_ptr<RData[]> file;
        std::shared_ptr<stbtt_fontinfo> info;
    } data_;

    struct {
        std::shared_ptr<std::map<RChar, Glyph>> caches;
        unsigned fsize = 14;
    } cache_;
};

} // Redopera

void swap(Redopera::RFont &font1, Redopera::RFont &font2);

#endif // RFONT_H
