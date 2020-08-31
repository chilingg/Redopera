#ifndef RGAME_H
#define RGAME_H

namespace Redopera {

class RWindow;

class RGame
{
public:
    static RWindow* getMainWindow(){ return mainWindow; }

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
    static RWindow *mainWindow;
};

} // ns Redopera

#endif //RGAME_H
