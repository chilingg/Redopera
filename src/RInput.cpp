#include <RInput.h>

using namespace Redopera;

RSignal<bool, int> RInput::joyPresented;
RSignal<int> RInput::wheeled;

std::unordered_map<SDL_Joystick*, RInput::Joystick> RInput::joysticks_;
RInput::Key RInput::key_;
RInput::Mouse RInput::mouse_;

void RInput::process(const SDL_Event &e)
{
    switch(e.type)
    {
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        joysticks_.at(SDL_JoystickFromInstanceID(e.jbutton.which)).status_[e.jbutton.button] =
                e.jbutton.state == SDL_PRESSED ? R_PRESSED : R_RELEASED;
        break;

    case SDL_KEYDOWN:
        key_.status_[e.key.keysym.scancode] = e.key.repeat ? R_REPEAR : R_PRESSED;
        break;
    case SDL_KEYUP:
        key_.status_[e.key.keysym.scancode] = R_RELEASED;
        break;

    case SDL_MOUSEMOTION:
        mouse_.moved_ = true;
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if(e.button.state == SDL_PRESSED)
            mouse_.status_[e.button.button - 1] = e.button.clicks == 1 ? R_PRESSED : R_DOUBLECLICK;
        else
            mouse_.status_[e.button.button - 1] = R_RELEASED;
        break;

    case SDL_JOYDEVICEADDED:
        if(openJoystick(e.jdevice.which))
            joyPresented.emit(true, SDL_JoystickGetDeviceInstanceID(e.jdevice.which));
        break;
    case SDL_JOYDEVICEREMOVED:
        joyPresented.emit(false, e.jdevice.which);
        break;
    case SDL_MOUSEWHEEL:
        wheeled.emit(e.wheel.y);
    }
}

void RInput::updataClear()
{
    for(auto &[jid, joy] : joysticks_)
        std::fill_n(joy.status_.get(), SDL_JoystickNumButtons(joy.joy_), R_NONE);

    std::for_each(key_.status_.begin(), key_.status_.end(), [](auto &it){ it.second = R_NONE; });
    auto &key = key_;

    mouse_.moved_ = false;
    mouse_.status_ = { R_NONE, R_NONE, R_NONE };
}

/*
void RInput::initJoysticks()
{
    for(int i = 0; i < SDL_NumJoysticks(); ++i)
        openJoystick(i);
}
*/

SDL_Joystick* RInput::openJoystick(int device)
{
    SDL_Joystick *joy = SDL_JoystickOpen(device);
    if(joy)
    {
        joysticks_[joy].joy_ = joy;
        joysticks_[joy].status_ = std::make_unique<int[]>(SDL_JoystickNumButtons(joy));
    }
    return joy;
}

int RInput::Mouse::status(int *x, int *y)
{
    return SDL_GetMouseState(&pos_.rx(), &pos_.ry());
}

int RInput::Mouse::left()
{
    return status_[0];
}

int RInput::Mouse::middle()
{
    return status_[1];
}

int RInput::Mouse::right()
{
    return status_[2];
}

bool RInput::Mouse::move()
{
    return moved_;
}

const RPoint2 &RInput::Mouse::pos()
{
    SDL_GetMouseState(&pos_.rx(), &pos_.ry());
    return pos_;
}

int RInput::Key::status(int key)
{
    static int numkeys;
    static const uint8_t *kStatus = SDL_GetKeyboardState(&numkeys);

    if(key > 0 || key < numkeys)
        return kStatus[key];
    else
        return R_NONE;
}

bool RInput::Key::press(int key)
{
    if(status_.contains(key))
        return status_[key] == R_PRESSED;
    else
        return false;
}

bool RInput::Key::release(int key)
{
    if(status_.contains(key))
        return status_[key] == R_RELEASED;
    else
        return false;
}

bool RInput::Key::repeat(int key)
{
    if(status_.contains(key))
        return status_[key] == R_REPEAR;
    else
        return false;
}

int RInput::Joystick::status(int btn)
{
    if(btn > 0 || btn < SDL_JoystickNumButtons(joy_))
        return status_[btn];
    return R_NONE;
}

bool RInput::Joystick::press(int btn)
{
    if(btn > 0 || btn < SDL_JoystickNumButtons(joy_))
        return status_[btn] == R_PRESSED;
    return R_NONE;
}

bool RInput::Joystick::release(int btn)
{
    if(btn > 0 || btn < SDL_JoystickNumButtons(joy_))
        return status_[btn] == R_RELEASED;
    return R_NONE;
}

int RInput::Joystick::axis(int axis)
{
    if(axis > 0 || axis < SDL_JoystickNumAxes(joy_))
        return SDL_JoystickGetAxis(joy_, axis);
    else
        return 0;
}

int RInput::Joystick::rumble(unsigned low, unsigned high, unsigned ms)
{
    return SDL_JoystickRumble(joy_, low, high, ms);
}

int RInput::Joystick::rumbleTriggers(unsigned left, unsigned right, unsigned ms)
{
    return SDL_JoystickRumbleTriggers(joy_, left, right, ms);
}
