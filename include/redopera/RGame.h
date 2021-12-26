#ifndef RGAME_H
#define RGAME_H

#include <cstdint>

namespace Redopera {

class RGame
{
public:
    static bool setHint(const char *name, const char *value);

    RGame();
    RGame(uint32_t flag);
    ~RGame();

    explicit operator bool() const { return init; }

private:
    static bool init;
};

} // ns Redopera

#endif //RGAME_H
