#ifndef RINPUT_H
#define RINPUT_H

#include "RSigslot.h"
#include "RPoint.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_joystick.h>

#define R_RELEASED      -1
#define R_NONE          0
#define R_PRESSED       1
#define R_REPEAR        2
#define R_DOUBLECLICK   R_REPEAR

namespace Redopera {

class RInput
{
    struct Joystick
    {
        friend RInput;

        // 1 press, -1 release
        int status(int btn);
        bool press(int btn);
        bool release(int btn);

        // -32768 to * 32767
        int axis(int axis);

        // rumble value 0 to 0xFFFF
        int rumble(unsigned low, unsigned high, unsigned ms);
        int rumbleTriggers(unsigned left, unsigned right, unsigned ms);

    private:
        SDL_Joystick *joy_;
        std::unique_ptr<int[]> status_;
    };

    struct Key
    {
        friend RInput;

        // 1 press, -1 release, 0 none, 2 repeat
        int status(int key);
        bool press(int key);
        bool release(int key);
        bool repeat(int key);

    private:
        std::unordered_map<int, int> status_;
    };

    struct Mouse
    {
        friend RInput;

        // 1 left, 2 middle, 3 left
        int status(int *x, int *y);
        // 1 press, -1 release, 0 none, 2 double click
        int left();
        int middle();
        int right();

        bool move();
        const RPoint2& pos();

    private:
        bool moved_ = false;
        RPoint2 pos_;
        std::array<int, 3> status_ { 0, 0, 0 };
    };

public:
    static void updataClear();

    static void process(const SDL_Event &event);

    static int joystickNum() { return joysticks_.size(); }
    static int playerToJid(int player) { return SDL_JoystickInstanceID(SDL_JoystickFromPlayerIndex(player)); }
    static int jidToPlayer(int jid) { return SDL_JoystickGetPlayerIndex(SDL_JoystickFromInstanceID(jid)); }

    static Joystick* joystick(int player)
    {
        SDL_Joystick* joy = SDL_JoystickFromPlayerIndex(player);
        if(joy)
            return &joysticks_.at(joy);
        else
            return nullptr;
    }
    static Joystick* joystickFromID(int id)
    {
        SDL_Joystick* joy = SDL_JoystickFromInstanceID(id);
        if(joy)
            return &joysticks_.at(joy);
        else
            return nullptr;
    }

    static Key& key() { return key_; };

    static Mouse& mouse() { return mouse_; };

    // Signal
    static RSignal<bool, int> joyPresented;
    static RSignal<int> wheeled;

    RInput() = delete;

private:
    static SDL_Joystick* openJoystick(int device);

    static std::unordered_map<SDL_Joystick*, Joystick> joysticks_;
    static Key key_;
    static Mouse mouse_;
};

} // ns Redopera

#endif // RINPUT_H
