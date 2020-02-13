#include "rsc/RTexture.h"

using namespace Redopera;

const RTexture::Format RTexture::LinearTex = std::make_shared<RTexture::TexFormat>(TexFormat{ Filter::Linear, Filter::Nearest });
const RTexture::Format RTexture::NearestTex = std::make_shared<RTexture::TexFormat>(TexFormat{ Filter::Nearest, Filter::Nearest });
const RTexture::Format RTexture::SingleTex = std::make_shared<RTexture::TexFormat>(TexFormat{ Filter::Linear, Filter::Nearest, 1 });;

RTexture::Format RTexture::textureFormat = makeTexFormat();

const RTexture &RTexture::whiteTex()
{
    thread_local static const RTexture WHITE_TEX(reinterpret_cast<const RData*>("\xff\xff\xff\xff"), 1, 1, 4, "WhiteTex", RTexture::NearestTex);
    return WHITE_TEX;
}

const RTexture &RTexture::blackTex()
{
    thread_local static const RTexture BLACK_TEX(reinterpret_cast<const RData*>("\x0\x0\x0\xff"), 1, 1, 4, "BlackTex", RTexture::NearestTex);
    return BLACK_TEX;
}

const RTexture &RTexture::transTex()
{
    thread_local static const RTexture TRANS_TEX(reinterpret_cast<const RData*>("\x0\x0\x0\x0"), 1, 1, 4, "TransTex", RTexture::NearestTex);
    return TRANS_TEX;
}

void RTexture::setDefaultTextureFomat(const RTexture::Format &format)
{
    textureFormat = format;
}

RTexture::Format RTexture::makeTexFormat()
{
    return std::make_shared<RTexture::TexFormat>();
}

void RTexture::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

RTexture::RTexture():
    RResource("Texture", Type::Texture)
{

}

RTexture::RTexture(const std::string &path, const std::string &name, const RTexture::Format &format):
    RResource(name, Type::Texture)
{
    load(path, format);
}

RTexture::RTexture(const RImage &img, const std::string &name, const RTexture::Format &format):
    RResource(name, Type::Texture)
{
    load(img, format);
}

RTexture::RTexture(const RData *data, int width, int height, int channel, const std::string &name, const RTexture::Format &format):
    RResource(name, Type::Texture)
{
    load(data, width, height, channel, format);
}

RTexture::RTexture(const RTexture &tex):
    RResource(tex),
    textureID_(tex.textureID_),
    format_(tex.format_),
    width_(tex.width_),
    height_(tex.height_)
{

}

RTexture::RTexture(const RTexture &&tex):
    RResource(std::move(tex)),
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
    RResource::swap(tex);
    textureID_.swap(tex.textureID_);
    format_.swap(tex.format_);
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
    if(!textureID_.unique() && textureID_ != nullptr)
        resetRscID();
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
    switch(format->inChannel)
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
        throw std::invalid_argument("Invalid set texture ichannel to " + std::to_string(format->inChannel));
    }

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, format->edgeColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(format->wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(format->wrapT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(format->filterMin));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(format->filterMax));

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
        load(data, width_, height_, format_->inChannel, format_);

    ExtFormat eformat;
    inFormat iformat;
    switch(format_->inChannel)
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
        throw std::invalid_argument("Invalid set texture ichannel to " + std::to_string(format_->inChannel));
    }

    glBindTexture(GL_TEXTURE_2D, *textureID_);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(iformat), width_, height_, 0, static_cast<GLenum>(eformat), GL_UNSIGNED_BYTE, data);
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
