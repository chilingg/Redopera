#ifndef RMP3_H
#define RMP3_H

#include <string>
#include <memory>

typedef struct Mp3dec_t mp3dec_t;
typedef int16_t mp3d_sample_t;

namespace Redopera {

using RData = uint8_t;

class RMp3
{
    using Decoder = std::unique_ptr<mp3dec_t>;

public:
    using Sample = mp3d_sample_t;

    RMp3() = default;
    explicit RMp3(const std::string &path);
    RMp3(const RData *data, size_t size);
    RMp3(const RMp3 &mp3);
    RMp3(const RMp3 &&mp3);
    RMp3& operator=(RMp3 mp3);
    void swap(RMp3 &mp3);
    ~RMp3() = default;

    bool isValid() const;
    unsigned hz() const;
    size_t samples() const;
    int channel() const;
    Sample* data() const;

    bool load(const std::string &path);
    bool load(const RData *data, size_t size);
    void release();

private:
    static thread_local Decoder decoder;

    unsigned hz_ = 0;
    size_t samples_ = 0;
    int channel_ = 0;
    std::shared_ptr<Sample> data_;
};

} // Redopera

void swap(Redopera::RMp3 &img1, Redopera::RMp3 &img2);

#endif // RMP3_H
