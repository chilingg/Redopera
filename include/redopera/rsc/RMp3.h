#ifndef RMP3_H
#define RMP3_H

#include "RResource.h"

namespace minimp3 {

#include "../dependents/minimp3.h"
#include "../dependents/minimp3_ex.h"

}

namespace Redopera {

class RMp3 : public RResource
{
    using Decoder = std::unique_ptr<minimp3::mp3dec_t>;
    friend void swap(RMp3 &img1, RMp3 &img2);

public:
    using Sample = minimp3::mp3d_sample_t;

    RMp3();
    explicit RMp3(const std::string &path, const std::string &name = "Mp3");
    RMp3(const RData *data, size_t size, const std::string &name = "Mp3");
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
