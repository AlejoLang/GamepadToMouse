#ifndef GamepadController_h
#define GamepadController_h

#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL.h"
#include <map>
#include "VirtualDevice.hpp"
#include "VirtualMouse.hpp"
#include "VirtualKeyboard.hpp"

#define ANALOG_MAX_VALUE 32767;

class GamepadController
{
private:
  VirtualMouse *virtual_mouse;
  VirtualKeyboard *virtual_keyboard;
  SDL_Gamepad *gamepad;
  SDL_JoystickID gamepad_id;
  std::map<SDL_GamepadButton, VirtualDevice::Action> keymap;
  SDL_GamepadAxis mouse_x_axis = SDL_GAMEPAD_AXIS_LEFTX;
  SDL_GamepadAxis mouse_y_axis = SDL_GAMEPAD_AXIS_LEFTY;

public:
  GamepadController(VirtualMouse *vm, VirtualKeyboard *vk, SDL_Gamepad *gp);
  void process_key_event(SDL_Event *event);
  void process_joysticks();
  SDL_Gamepad *get_gamepad();
  SDL_JoystickID get_gamepad_id();
};

#endif