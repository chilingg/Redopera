#ifndef RGAME_H
#define RGAME_H

#include <RSigslot.h>

namespace Redopera {

class RWindow;

enum class JoystickID;
enum class JoystickPresent;

class RGame
{
public:
    static RSignal<JoystickID, JoystickPresent> joyPresented;

    RGame();
    ~RGame();

    RGame(const RGame&) = delete;
    RGame& operator=(const RGame&) = delete;

    bool updateGamepadMappings(const char *path);
    bool updateGamepadMappings();
    int exec(RWindow *window);

private:
    // 手柄连接回调
    static void joystickPresentCallback(int jid, int event);

    static bool once;
};

} // ns Redopera

#endif //RGAME_H
