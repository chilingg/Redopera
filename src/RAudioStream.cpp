#include "RAudioStream.h"
#include "RDebug.h"

using namespace Redopera;

RAudioStream::RAudioStream(RAudioStream::Api api):
    stream_(api)
{
    if(check(stream_.getDeviceCount() < 1, "No audio devices found!"))
        return;
    parameters_.deviceId = stream_.getDefaultOutputDevice();
}

RAudioStream::RAudioStream(const RMp3 &mp3, RAudioStream::Api api):
    RAudioStream(api)
{
    openStream(mp3);
}

RAudioStream::RAudioStream(const RAudioStream &audio):
    RAudioStream(audio.mp3_, audio.currentApi())
{
    volume_ = audio.volume_;
    repeat_ = audio.repeat_;
}

RAudioStream &RAudioStream::operator=(const RAudioStream &audio)
{
    stream_ = RtAudio(audio.currentApi());

    parameters_.deviceId = stream_.getDefaultOutputDevice();
    volume_ = audio.volume_;
    repeat_ = audio.repeat_;

    if(audio.mp3_.isValid())
        openStream(audio.mp3_);

    return *this;
}

RAudioStream::~RAudioStream()
{
    if(isRunning()) stopStream();
    if(isOpen()) closeStream();
}

RAudioStream::Api RAudioStream::currentApi() const
{
    return const_cast<RtAudio*>(&stream_)->getCurrentApi();
}

std::string RAudioStream::currentApiName() const
{
    return stream_.getApiName(const_cast<RtAudio*>(&stream_)->getCurrentApi());
}

bool RAudioStream::isOpen() const
{
    return stream_.isStreamOpen();
}

bool RAudioStream::isRunning() const
{
    return stream_.isStreamRunning();
}

double RAudioStream::getStreamTime() const
{
    return const_cast<RtAudio*>(&stream_)->getStreamTime();
}

float RAudioStream::getVolume() const
{
    return volume_;
}

void RAudioStream::setStreamTime(double time)
{
    stream_.setStreamTime(time);
}

float RAudioStream::setVolume(float volume)
{
    if(volume > 1.0f) volume = 1.0f;
    if(volume < 0.0f) volume = 0.0f;
    return volume_ = volume;
}

float RAudioStream::increaseVolume(float increase)
{
    return setVolume(volume_ + increase);
}

float RAudioStream::decreaseVolume(float decrease)
{
    return setVolume(volume_ - decrease);
}

bool RAudioStream::openStream(const RMp3 &mp3)
{
    static unsigned BUFFER_SIZE = 4096;

    if(stream_.isStreamOpen())
        stream_.closeStream();

    mp3_ = mp3;
    parameters_.nChannels = static_cast<unsigned>(mp3.channel());

    try {
        stream_.openStream(&parameters_, nullptr, RTAUDIO_SINT16, static_cast<unsigned>(mp3.hz()), &BUFFER_SIZE, &playback, this);
    }
    catch(RtAudioError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

void RAudioStream::closeStream()
{
    if(stream_.isStreamOpen())
        stream_.closeStream();
}

bool RAudioStream::startStream()
{
    return repeatStream(0);
}

bool RAudioStream::repeatStream(unsigned repeat)
{
    if(stream_.isStreamRunning())
        stream_.abortStream();

    setStreamTime();
    try {
        stream_.startStream();
        repeat_ = repeat;
    }
    catch(RtAudioError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

bool RAudioStream::stopStream()
{
    if(!stream_.isStreamRunning())
        return false;

    try {
        stream_.stopStream();
    }
    catch(RtAudioError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

bool RAudioStream::abortStream()
{
    if(!stream_.isStreamRunning())
        return false;

    try {
        stream_.abortStream();
    }
    catch(RtAudioError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

int RAudioStream::playback(void *outputBuffer, void *, unsigned nBufferFrames, double streamTime,
                           RtAudioStreamStatus status, void *userData)
{
    if(check(status, "Stream underflow detected!")) return 2;

    RMp3::Sample *buffer = static_cast<RMp3::Sample*>(outputBuffer);
    RAudioStream *rAudio = static_cast<RAudioStream*>(userData);
    unsigned channels = rAudio->parameters_.nChannels;

    unsigned offset = static_cast<unsigned>(streamTime * rAudio->mp3_.hz() * channels);
    if(offset + nBufferFrames*channels > rAudio->mp3_.samples())
    {
        if(rAudio->repeat_ == 0)
            return 1;
        else {
            --rAudio->repeat_;
            rAudio->setStreamTime();
        }
    }

    const RMp3::Sample* lastValues = rAudio->mp3_.data() + offset;
    float volume = rAudio->volume_;
    for(unsigned i = 0; i < nBufferFrames; ++i)
    {
        for(unsigned j = 0; j < channels; ++j)
            *buffer++ = static_cast<RMp3::Sample>(*lastValues++ * volume);
    }

    return 0;
}
