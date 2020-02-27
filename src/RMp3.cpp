#include "rsc/RMp3.h"
#include "RDebug.h"

#include "dependents/minimp3.h"
#include "dependents/minimp3_ex.h"

using namespace Redopera;

thread_local RMp3::Decoder RMp3::decoder;

RMp3::RMp3(const std::string &path)
{
    load(path);
}

RMp3::RMp3(const RData *data, size_t size)
{
    load(data, size);
}

RMp3::RMp3(const RMp3 &mp3):
    hz_(mp3.hz_),
    samples_(mp3.samples_),
    channel_(mp3.channel_),
    data_(mp3.data_)
{

}

RMp3::RMp3(const RMp3 &&mp3):
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

bool RMp3::load(std::string path)
{
    RResource::rscpath(path);

    if(!decoder)
    {
        decoder = std::make_unique<mp3dec_t>();
        mp3dec_init(decoder.get());
    }

    mp3dec_file_info_t info;
    if(check(mp3dec_load(decoder.get(), path.c_str(), &info, nullptr, nullptr), "Failed to load mp3 in " + path))
        return false;

    hz_ = static_cast<unsigned>(info.hz);
    samples_ = info.samples;
    channel_ = info.channels;
    data_.reset(info.buffer, std::free);

    return true;
}

bool RMp3::load(const RData *data, size_t size)
{
    if(!data || !size)
        return false;

    if(!decoder)
    {
        decoder = std::make_unique<mp3dec_t>();
        mp3dec_init(decoder.get());
    }

    mp3dec_file_info_t info;
    mp3dec_load_buf(decoder.get(), data, size, &info, nullptr, nullptr);

    hz_ = static_cast<unsigned>(info.hz);
    samples_ = info.samples;
    channel_ = info.channels;
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
