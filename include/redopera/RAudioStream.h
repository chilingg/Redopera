#ifndef RAUDIOSTREAM_H
#define RAUDIOSTREAM_H

#include <rtaudio/RtAudio.h>
#include "rsc/RMp3.h"

namespace Redopera {

class RAudioStream
{
public:
    using Api = RtAudio::Api;

    RAudioStream(Api api = Api::UNSPECIFIED);
    RAudioStream(const RMp3 &mp3, Api api = Api::UNSPECIFIED);
    RAudioStream(const RAudioStream &audio);
    RAudioStream& operator=(const RAudioStream &audio);
    ~RAudioStream();

    Api currentApi() const;
    std::string currentApiName() const;
    bool isOpen() const;
    bool isRunning() const;
    double getStreamTime() const;
    float getVolume() const;

    void setStreamTime(double time = 0.0);
    float setVolume(float volume);
    float increaseVolume(float increase = 0.1f);
    float decreaseVolume(float decrease = 0.1f);

    bool openStream(const RMp3 &mp3);
    void closeStream();
    bool startStream();
    bool repeatStream(unsigned repeat = ~0u);
    bool stopStream();
    bool abortStream();

private:
    static int playback(void *outputBuffer,
                        void *inputBuffer,
                        unsigned nBufferFrames,
                        double streamTime,
                        RtAudioStreamStatus status,
                        void *userData);

    RtAudio stream_;
    RtAudio::StreamParameters parameters_;
    RMp3 mp3_;
    float volume_ = 1.0f;
    unsigned repeat_ = 0;
};

} // Redopera

#endif // RAUDIOSTREAM_H
