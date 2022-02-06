#ifndef RTEXTURE_H
#define RTEXTURE_H

#include "../RRect.h"
#include "../RDefine.h"
#include "ROpenGL.h"

#include <memory>

namespace Redopera {

class RImage;
class RColor;

// 修改纹理内容会使所有持相同纹理的RTexture内容变更
class RTexture
{
public:
    enum class InFormat
    {
        R8 = GL_R8,
        RG8 = GL_RG8,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8
    };

    enum class ExtFormat
    {
        RED = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA
    };

    enum class Wrap
    {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
    };

    enum class Filter
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR
    };

    struct Format
    {
        struct {
            Filter max, min;
        } filter { Filter::Linear, Filter::Linear };

        int inChannel = 4;

        struct {
            Wrap s, t;
        } wrap { Wrap::ClampToBorder, Wrap::ClampToBorder };

        std::array<GLfloat, 4> edgeColor { 0, 0, 0, 0 };
        std::array<GLint, 4> swizzle { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};
    };

    static const Format Linear4;
    static const Format Linear3;
    static const Format Nearest4;
    static const Format Nearest3;
    static const Format SingleToLinear4;

    static RTexture createWhiteTex();
    static RTexture createBlackTex();
    static RTexture createRedTex();
    static RTexture createTransTex();

    static Format setTextureFormat(Format format);
    static Format& currentFormat();
    static RTexture colorTexture(const RColor &color);
    static RTexture colorTexture(RGBA rgba);
    static RTexture colorTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xffu);
    static void unbind();

    static InFormat inFormat(int channel);
    static ExtFormat extFormat(int channel);

    RTexture() = default;
    ~RTexture() = default;

    RTexture(const std::string &path);
    RTexture(const RImage &img);
    RTexture(const RData *data, int width, int height, int channel);
    RTexture(const RData *data, const RSize &size, int channel);
    RTexture(const RTexture &tex);
    RTexture(const RTexture &&tex);
    RTexture& operator=(RTexture tex);
    void swap(RTexture &tex);

    bool isValid() const;
    int width() const;
    int height() const;
    const RSize& size() const;
    GLuint id() const;
    void bind(unsigned unit = 0) const; // unit直接指定纹理单元号，无需使用GL_TEXTURE0

    bool load(const RData *data, int width, int height, int echannel);
    bool load(const RData *data, const RSize &size, int echannel);
    bool load(const RImage &img);
    bool load(const std::string &path);
    void reload(const RData* data, int echannel);
    void setSubTexture(const RRect &rect, const RData* data, int echannel);
    void setSubTexture(const RPoint2 &pos, const RSize &size, const RData* data, int echannel);
    void setSubTexture(int x, int y, int width, int height, const RData* data, int echannel);
    void free();
    void copyAs(const RTexture tex, unsigned channal);

private:
    RSize size_;
    std::shared_ptr<GLuint> textureID_;
};

} // Redopera

void swap(Redopera::RTexture &tex1, Redopera::RTexture &tex2);

#endif // RTEXTURE_H
