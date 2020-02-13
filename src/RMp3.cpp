#include "rsc/RMp3.h"
#include "RDebug.h"

using namespace Redopera;

thread_local RMp3::Decoder RMp3::decoder;

RMp3::RMp3():
    RResource("Mp3", Type::Mp3)
{

}

RMp3::RMp3(const std::string &path, const std::string &name):
    RResource(name, Type::Mp3)
{
    load(path);
}

RMp3::RMp3(const RData *data, size_t size, const std::string &name):
    RResource(name, Type::Mp3)
{
    load(data, size);
}

RMp3::RMp3(const RMp3 &mp3):
    RResource(mp3),
    hz_(mp3.hz_),
    samples_(mp3.samples_),
    channel_(mp3.channel_),
    data_(mp3.data_)
{

}

RMp3::RMp3(const RMp3 &&mp3):
    RResource(std::move(mp3)),
    hz_(mp3.hz_),
    samples_(mp3.samples_),
    channel_(mp3.channel_),
    data_(std::move(mp3.data_))
{

}

RMp3 &RMp3::operator=(RMp3 mp3)
{
    swap(mp3);
    return *this;
}

void RMp3::swap(RMp3 &mp3)
{
    RResource::swap(mp3);
    using std::swap;
    swap(hz_, mp3.hz_);
    swap(samples_, mp3.samples_);
    swap(channel_, mp3.channel_);
    swap(data_, mp3.data_);
}

bool RMp3::isValid() const
{
    return data_ != nullptr;
}

unsigned RMp3::hz() const
{
    return hz_;
}

size_t RMp3::samples() const
{
    return samples_;
}

int RMp3::channel() const
{
    return channel_;
}

RMp3::Sample *RMp3::data() const
{
    return data_.get();
}

bool RMp3::load(const std::string &path)
{
    std::string newpath = rscpath(path);

    if(!decoder)
    {
        decoder.reset(new minimp3::mp3dec_t);
        minimp3::mp3dec_init(decoder.get());
    }

    minimp3::mp3dec_file_info_t info;
    if(check(minimp3::mp3dec_load
             (decoder.get(), newpath.c_str(), &info, nullptr, nullptr), "Failed to load mp3 <" + name() + "> in " + newpath))
        return false;

    hz_ = static_cast<unsigned>(info.hz);
    samples_ = info.samples;
    channel_ = info.channels;
    if(!data_.unique())
        resetRscID();
    data_.reset(info.buffer, std::free);

    return true;
}

bool RMp3::load(const RData *data, size_t size)
{
    if(!data || !size)
        return false;

    if(!decoder)
    {
        decoder.reset(new minimp3::mp3dec_t);
        minimp3::mp3dec_init(decoder.get());
    }

    minimp3::mp3dec_file_info_t info;
    minimp3::mp3dec_load_buf(decoder.get(), data, size, &info, nullptr, nullptr);

    hz_ = static_cast<unsigned>(info.hz);
    samples_ = info.samples;
    channel_ = info.channels;
    if(!data_.unique())
        resetRscID();
    data_.reset(info.buffer, std::free);

    return true;
}

void RMp3::release()
{
    data_.reset();
}

void swap(RMp3 &img1, RMp3 &img2)
{
    img1.swap(img2);
}
