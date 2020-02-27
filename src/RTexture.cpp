#include "rsc/RTexture.h"
#include "rsc/RImage.h"
#include "RSize.h"

using namespace Redopera;

const RTexture::Format RTexture::Linear4 { { Filter::Linear, Filter::Nearest }, 4 };
const RTexture::Format RTexture::Linear3 { { Filter::Linear, Filter::Nearest }, 3 };
const RTexture::Format RTexture::Nearest4 { { Filter::Nearest, Filter::Nearest }, 4 };
const RTexture::Format RTexture::Nearest3 { { Filter::Nearest, Filter::Nearest }, 3 };
const RTexture::Format RTexture::SingleL { { Filter::Linear, Filter::Nearest }, 1 };
const RTexture::Format RTexture::SingleN { { Filter::Nearest, Filter::Nearest }, 1 };

RTexture::Format RTexture::textureFormat;

RTexture RTexture::whiteTex()
{
    return { reinterpret_cast<const RData*>("\xff\xff\xff"), 1, 1, 3, RTexture::Nearest3 };
}

RTexture RTexture::blackTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0"), 1, 1, 3, RTexture::Nearest3 };
}

RTexture RTexture::transTex()
{
    return { reinterpret_cast<const RData*>("\x0\x0\x0\x0"), 1, 1, 4, RTexture::Nearest4 };
}

void RTexture::setDefaultTextureFomat(const RTexture::Format &format)
{
    textureFormat = format;
}

void RTexture::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RTexture::RTexture(const std::string &path, const RTexture::Format &format)
{
    load(path, format);
}

RTexture::RTexture(const RImage &img, const RTexture::Format &format)
{
    load(img, format);
}

RTexture::RTexture(const RData *data, int width, int height, int channel, const RTexture::Format &format)
{
    load(data, width, height, channel, format);
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

GLuint RTexture::textureID() const
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

    ExtFormat eformat;
    switch(echannel)
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
        throw std::invalid_argument("Invalid set texture echannel to " + std::to_string(echannel));
    }

    inFormat iformat;
    switch(format.inChannel)
    {
    case 4:
        iformat = inFormat::RGBA8; break;
    case 3:
        iformat = inFormat::RGB8; break;
    case 2:
        iformat = inFormat::RG8; break;
    case 1:
        iformat = inFormat::R8; break;
    default:
        throw std::invalid_argument("Invalid set texture ichannel to " + std::to_string(format.inChannel));
    }

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, format.edgeColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(format.wrap.s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(format.wrap.t));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(format.filter.min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(format.filter.max));

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(iformat), width, height, 0, static_cast<GLenum>(eformat), GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    format_ = format;
    width_ = width;
    height_ = height;
    return true;
}

bool RTexture::load(const RImage &img, const RTexture::Format &format)
{
    return load(img.data(), img.width(), img.height(), img.channel(), format);
}

bool RTexture::load(const std::string &path, const RTexture::Format &format)
{
    return load(RImage(path), format);
}

void RTexture::reload(const RData *data)
{
    if(!textureID_.unique())
        load(data, width_, height_, format_.inChannel, format_);

    ExtFormat eformat;
    inFormat iformat;
    switch(format_.inChannel)
    {
    case 4:
        eformat = ExtFormat::RGBA;
        iformat = inFormat::RGBA8; break;
    case 3:
        eformat = ExtFormat::RGB;
        iformat = inFormat::RGB8; break;
    case 2:
        eformat = ExtFormat::RG;
        iformat = inFormat::RG8; break;
    case 1:
        eformat = ExtFormat::RED;
        iformat = inFormat::R8; break;
    default:
        throw std::invalid_argument("Invalid set texture ichannel to " + std::to_string(format_.inChannel));
    }

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(iformat), width_, height_, 0,
                 static_cast<GLenum>(eformat), GL_UNSIGNED_BYTE, data);
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

void swap(RTexture &tex, RTexture &tex2)
{
    tex.swap(tex2);
}
