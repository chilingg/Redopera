#ifndef RFONT_H
#define RFONT_H

#include "RResource.h"
#include "../RDefine.h"

class stbtt_fontinfo;

namespace Redopera {

class RFont
{
public:
    struct Glyph
    {
        int width = 0;
        int height = 0;
        int xoff = 0;
        int yoff = 0;
        int advence = 0;
        std::unique_ptr<const RData[]> data;
    };

    // sourceCodePro()定义在字体资源文件中 (SourceCodePro.cpp)
    static RFont sourceCodePro();
    static RFont& defaultFont();

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
    Glyph getGlyph(RChar c) const;

    void setSize(unsigned size);
    bool load(const std::string &path);
    bool load(const RData *data, size_t size);
    void free();

private:
    unsigned fsize_ = 14;
    std::shared_ptr<stbtt_fontinfo> info_;
    std::shared_ptr<RData[]> file_;
};

} // Redopera

void swap(Redopera::RFont &font1, Redopera::RFont &font2);

#endif // RFONT_H
