#include <rsc/RTexture.h>
#include <rsc/RImage.h>
#include <RColor.h>

#include <stdexcept>
#include <algorithm>

using namespace Redopera;

const RTexture::Format RTexture::Linear4 { { Filter::Linear, Filter::Linear }, 4 };
const RTexture::Format RTexture::Linear3 { { Filter::Linear, Filter::Linear }, 3 };
const RTexture::Format RTexture::Nearest4 { { Filter::Nearest, Filter::Nearest }, 4 };
const RTexture::Format RTexture::Nearest3 { { Filter::Nearest, Filter::Nearest }, 3 };
const RTexture::Format RTexture::SingleL { { Filter::Linear, Filter::Linear }, 1 };
const RTexture::Format RTexture::SingleN { { Filter::Nearest, Filter::Nearest }, 1 };

RTexture::Format RTexture::textureFormat;

RTexture RTexture::createWhiteTex()
{
    return { reinterpret_cast<const RData*>("\xff\xff\xff"), 1, 1, 3, RTexture::Nearest3 };
}

RTexture RTexture::createBlackTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0"), 1, 1, 3, RTexture::Nearest3 };
}

RTexture RTexture::createRedTex()
{
    return { reinterpret_cast<const RData*>("\xff\x0\x0"), 1, 1, 3, RTexture::Nearest3 };
}

RTexture RTexture::createTransTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0\x0"), 1, 1, 4, RTexture::Nearest4 };
}

void RTexture::setDefaultTextureFomat(const RTexture::Format &format)
{
    textureFormat = format;
}

const RTexture::Format &RTexture::defaultFormat()
{
    return textureFormat;
}

RTexture RTexture::colorTexture(const RColor &color)
{
    RGBA rgba = color.rgba();
    return colorTexture(rgba);
}

RTexture RTexture::colorTexture(RGBA rgba)
{
    RData *colorData = reinterpret_cast<RData*>(&rgba);
    std::reverse(colorData, colorData + 4);
    return RTexture(colorData, 1, 1, 4, RTexture::Nearest4);
}

RTexture RTexture::colorTexture(unsigned r, unsigned g, unsigned b, unsigned a)
{
    RColor color(r, g, b, a);
    return colorTexture(color.rgba());
}

void RTexture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RTexture::InFormat RTexture::inFormat(int channel)
{
    InFormat iformat;
    switch(channel)
    {
    case 4:
        iformat = InFormat::RGBA8; break;
    case 3:
        iformat = InFormat::RGB8; break;
    case 2:
        iformat = InFormat::RG8; break;
    case 1:
        iformat = InFormat::R8; break;
    default:
        throw std::invalid_argument("Invalid set texture ichannel to " + std::to_string(channel));
    }
    return iformat;
}

RTexture::ExtFormat RTexture::extFormat(int channel)
{
    ExtFormat eformat;
    switch(channel)
    {
    case 4:
        eformat = ExtFormat::RGBA; break;
    case 3:
        eformat = ExtFormat::RGB; break;
    case 2:
        eformat = ExtFormat::RG; break;
    case 1:
        eformat = ExtFormat::RED; break;
    default:
        throw std::invalid_argument("Invalid set texture echannel to " + std::to_string(channel));
    }
    return eformat;
}

RTexture::RTexture(const std::string &path, const RTexture::Format &format):
    RTexture(RImage(path), format)
{

}

RTexture::RTexture(const RImage &img, const RTexture::Format &format):
    RTexture(img.data(), img.width(), img.height(), img.channel(), format)
{

}

RTexture::RTexture(const RData *data, int width, int height, int channel, const RTexture::Format &format)
{
    load(data, width, height, channel, format);
}

RTexture::RTexture(const RData *data, const RSize &size, int channel, const RTexture::Format &format):
    RTexture(data, size.width(), size.height(), channel, format)
{

}

RTexture::RTexture(const RTexture &tex):
    textureID_(tex.textureID_),
    format_(tex.format_),
    width_(tex.width_),
    height_(tex.height_)
{

}

RTexture::RTexture(const RTexture &&tex):
    textureID_(std::move(tex.textureID_)),
    format_(std::move(tex.format_)),
    width_(tex.width_),
    height_(tex.height_)
{

}

RTexture &RTexture::operator=(RTexture tex)
{
    swap(tex);
    return *this;
}

void RTexture::swap(RTexture &tex)
{
    textureID_.swap(tex.textureID_);
    format_ = tex.format_;
    width_ = tex.width_;
    height_ = tex.height_;
}

bool RTexture::isValid() const
{
    return textureID_ != nullptr;
}

int RTexture::width() const
{
    return width_;
}

int RTexture::height() const
{
    return height_;
}

RSize RTexture::size() const
{
    return RSize(width_, height_);
}

const RTexture::Format &RTexture::format() const
{
    return format_;
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

bool RTexture::load(const RData *data, int width, int height, int echannel, const RTexture::Format &format)
{
    GLuint id;
    glGenTextures(1, &id);
    textureID_.reset(new GLuint(id), deleteTexture);

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, format.edgeColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(format.wrap.s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(format.wrap.t));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(format.filter.min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(format.filter.max));

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(inFormat(format.inChannel)),
                 width, height, 0, static_cast<GLenum>(extFormat(echannel)), GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    format_ = format;
    width_ = width;
    height_ = height;
    return true;
}

bool RTexture::load(const RData *data, const RSize &size, int echannel, const RTexture::Format &format)
{
    return load(data, size.width(), size.height(), echannel, format);
}

bool RTexture::load(const RImage &img, const RTexture::Format &format)
{
    return load(img.data(), img.width(), img.height(), img.channel(), format);
}

bool RTexture::load(const std::string &path, const RTexture::Format &format)
{
    return load(RImage(path, true), format);
}

void RTexture::reload(const RData *data)
{
    if(textureID_.unique())
    {
        glBindTexture(GL_TEXTURE_2D, *textureID_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, static_cast<GLint>(extFormat(format_.inChannel)), GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
        load(data, width_, height_, format_.inChannel, format_);
}

void RTexture::setSubTexture(const RRect &rect, const RData *data)
{
    setSubTexture(rect.width(), rect.height(), rect.left(), rect.bottom(), data);
}

void RTexture::setSubTexture(const RPoint2 &pos, const RSize &size, const RData *data)
{
    setSubTexture(size.width(), size.height(), pos.x(), pos.y(), data);
}

void RTexture::setSubTexture(int x, int y, int width, int height, const RData *data)
{
    if(!textureID_.unique())
        copyOnWrite();

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, static_cast<GLint>(extFormat(format_.inChannel)), GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RTexture::release()
{
    textureID_.reset();
}

void RTexture::deleteTexture(GLuint *id)
{
    glDeleteTextures(1, id);
    delete id;
}

void RTexture::copyOnWrite()
{
    std::unique_ptr<RData[]> data = std::make_unique<RData[]>(width_ * height_ * format_.inChannel);
    bind();
    glGetTexImage(GL_TEXTURE_2D, 0, static_cast<GLint>(extFormat(format_.inChannel)), GL_UNSIGNED_BYTE, data.get());
    load(data.get(), width_, height_, format_.inChannel, format_);
}

void swap(RTexture &tex, RTexture &tex2)
{
    tex.swap(tex2);
}
