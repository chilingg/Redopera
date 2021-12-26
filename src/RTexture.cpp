#include <render/RTexture.h>
#include <rsc/RImage.h>
#include <RColor.h>
#include <RFormat.h>

using namespace Redopera;

namespace  {

RTexture::Format textureFormat;

void setTextureFomat()
{
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, textureFormat.edgeColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(textureFormat.wrap.s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(textureFormat.wrap.t));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(textureFormat.filter.min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureFormat.filter.max));
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, textureFormat.swizzle.data());
}

void deleteTexture(GLuint *id)
{
    glDeleteTextures(1, id);
    delete id;
}

} // ns

const RTexture::Format RTexture::Linear4 { { Filter::Linear, Filter::Linear }, 4 };
const RTexture::Format RTexture::Linear3 { { Filter::Linear, Filter::Linear }, 3 };
const RTexture::Format RTexture::Nearest4 { { Filter::Nearest, Filter::Nearest }, 4 };
const RTexture::Format RTexture::Nearest3 { { Filter::Nearest, Filter::Nearest }, 3 };
const RTexture::Format RTexture::SingleToLinear4 { { Filter::Linear, Filter::Linear },
                                               4,
                                               { Wrap::ClampToBorder, Wrap::ClampToBorder },
                                               { 0, 0, 0, 0 },
                                               { GL_ONE, GL_ONE, GL_ONE, GL_RED }};

RTexture RTexture::createWhiteTex()
{
    return { reinterpret_cast<const RData*>("\xff\xff\xff"), 1, 1, 3 };
}

RTexture RTexture::createBlackTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0"), 1, 1, 3 };
}

RTexture RTexture::createRedTex()
{
    return { reinterpret_cast<const RData*>("\xff\x0\x0"), 1, 1, 3 };
}

RTexture RTexture::createTransTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0\x0"), 1, 1, 4 };
}

void RTexture::setTextureFomat(const RTexture::Format &format)
{
    textureFormat = format;

}

RTexture::Format &RTexture::currentFormat()
{
    return textureFormat;
}

RTexture RTexture::colorTexture(const RColor &color)
{
    RGBA abgr = color.abgr();
    RData *colorData = reinterpret_cast<RData*>(&abgr);
    return RTexture(colorData, 1, 1, 4);
}

RTexture RTexture::colorTexture(RGBA rgba)
{
    return colorTexture(RColor(rgba));
}

RTexture RTexture::colorTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    RColor color(r, g, b, a);
    return colorTexture(color);
}

void RTexture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RTexture::InFormat RTexture::inFormat(int channel)
{
    InFormat iformat = InFormat::RGBA8;
    switch(channel)
    {
    case 4:
        break;
    case 3:
        iformat = InFormat::RGB8; break;
    case 2:
        iformat = InFormat::RG8; break;
    case 1:
        iformat = InFormat::R8; break;
    default:
        rError("Invalid set texture ichannel as {}!\n", std::to_string(channel));
    }
    return iformat;
}

RTexture::ExtFormat RTexture::extFormat(int channel)
{
    ExtFormat eformat = ExtFormat::RGBA;
    switch(channel)
    {
    case 4:
        break;
    case 3:
        eformat = ExtFormat::RGB; break;
    case 2:
        eformat = ExtFormat::RG; break;
    case 1:
        eformat = ExtFormat::RED; break;
    default:
        rError("Invalid set texture echannel as {}!\n", std::to_string(channel));
    }
    return eformat;
}

RTexture::RTexture(const std::string &path):
    RTexture(RImage(path))
{

}

RTexture::RTexture(const RImage &img):
    RTexture(img.data(), img.width(), img.height(), img.channel())
{

}

RTexture::RTexture(const RData *data, int width, int height, int channel)
{
    load(data, width, height, channel);
}

RTexture::RTexture(const RData *data, const RSize &size, int channel):
    RTexture(data, size.width(), size.height(), channel)
{

}

RTexture::RTexture(const RTexture &tex):
    size_(tex.size_),
    textureID_(tex.textureID_)
{

}

RTexture::RTexture(const RTexture &&tex):
    size_(tex.size_),
    textureID_(std::move(tex.textureID_))
{

}

RTexture &RTexture::operator=(RTexture tex)
{
    swap(tex);
    return *this;
}

void RTexture::swap(RTexture &tex)
{
    std::swap(size_, tex.size_);
    textureID_.swap(tex.textureID_);
}

bool RTexture::isValid() const
{
    return textureID_ != nullptr;
}

int RTexture::width() const
{
    return size_.width();
}

int RTexture::height() const
{
    return size_.height();
}

const RSize& RTexture::size() const
{
    return size_;
}

GLuint RTexture::id() const
{
    return *textureID_;
}

void RTexture::bind(unsigned unit) const
{
    glActiveTexture(unit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *textureID_);
}

bool RTexture::load(const RData *data, int width, int height, int echannel)
{
    GLuint id;
    glGenTextures(1, &id);
    textureID_.reset(new GLuint(id), deleteTexture);

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    ::setTextureFomat();

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(inFormat(textureFormat.inChannel)),
                 width, height, 0, static_cast<GLenum>(extFormat(echannel)), GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    size_.setSize(width, height);
    return true;
}

bool RTexture::load(const RData *data, const RSize &size, int echannel)
{
    if(data)
        return load(data, size.width(), size.height(), echannel);
    else
        return false;
}

bool RTexture::load(const RImage &img)
{
    return load(img.data(), img.width(), img.height(), img.channel());
}

bool RTexture::load(const std::string &path)
{
    RImage img(path, true);

    return load(img);
}

void RTexture::reload(const RData *data, int echannel)
{
    if(textureID_.unique())
    {
        glBindTexture(GL_TEXTURE_2D, *textureID_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_.width(), size_.height(), static_cast<GLint>(extFormat(echannel)), GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
        load(data, size_.width(), size_.height(), echannel);
}

void RTexture::setSubTexture(const RRect &rect, const RData *data, int echannel)
{
    setSubTexture(rect.width(), rect.height(), rect.left(), rect.bottom(), data, echannel);
}

void RTexture::setSubTexture(const RPoint2 &pos, const RSize &size, const RData *data, int echannel)
{
    setSubTexture(size.width(), size.height(), pos.x(), pos.y(), data, echannel);
}

void RTexture::setSubTexture(int x, int y, int width, int height, const RData *data, int echannel)
{
    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, static_cast<GLint>(extFormat(echannel)), GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RTexture::free()
{
    textureID_.reset();
}

void RTexture::copyAs(const RTexture tex, unsigned channal)
{
    if(channal < 1 || channal > 4)
        return;

    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, tex.width() * tex.height() * channal, nullptr, GL_DYNAMIC_COPY);
    glBindTexture(GL_TEXTURE_2D, *tex.textureID_);
    glGetTexImage(GL_TEXTURE_2D, 0, static_cast<GLint>(extFormat(channal)), GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    load(nullptr, tex.width(), tex.height(), channal);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glDeleteBuffers(1, &pbo);
}

void swap(RTexture &tex, RTexture &tex2)
{
    tex.swap(tex2);
}
