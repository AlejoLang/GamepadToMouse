#include "GamepadController.hpp"
#include <math.h>

GamepadController::GamepadController(VirtualMouse *vm, VirtualKeyboard *vk, SDL_Gamepad *gp)
{
  this->gamepad = gp;
  this->virtual_keyboard = vk;
  this->virtual_mouse = vm;
  this->gamepad_id = gp ? SDL_GetGamepadID(gp) : 0;
  this->keymap = {{SDL_GAMEPAD_BUTTON_SOUTH, VirtualMouse::LEFT_CLICK_ACTION},
                  {SDL_GAMEPAD_BUTTON_EAST, VirtualMouse::RIGHT_CLICK_ACTION},
                  {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, VirtualMouse::MIDDLE_CLICK_ACTION},
                  {SDL_GAMEPAD_BUTTON_DPAD_UP, VirtualKeyboard::KEY_UP_ACTION},
                  {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, VirtualKeyboard::KEY_RIGHT_ACTION},
                  {SDL_GAMEPAD_BUTTON_DPAD_DOWN, VirtualKeyboard::KEY_DOWN_ACTION},
                  {SDL_GAMEPAD_BUTTON_DPAD_LEFT, VirtualKeyboard::KEY_LEFT_ACTION}};
}

void GamepadController::process_key_event(SDL_Event *event)
{
  if (event->gbutton.which != this->gamepad_id || !this->keymap.contains(static_cast<SDL_GamepadButton>(event->gbutton.button)))
  {
    return;
  }
  VirtualDevice::Action action = this->keymap.at(static_cast<SDL_GamepadButton>(event->gbutton.button));
  if (action.device == VirtualDevice::MOUSE)
  {
    virtual_mouse->do_key_action(action, event->gbutton.down);
  }
  else if (action.device == VirtualDevice::KEYBOARD)
  {
    virtual_keyboard->do_key_action(action, event->gbutton.down);
  }
}

void GamepadController::process_joysticks()
{
  // Process joystick movement
  float x = (float)(SDL_GetGamepadAxis(this->gamepad, this->mouse_x_axis)) / ANALOG_MAX_VALUE;
  float y = (float)(SDL_GetGamepadAxis(this->gamepad, this->mouse_y_axis)) / ANALOG_MAX_VALUE;
  float mag = sqrt(x * x + y * y);
  if (mag > 1.0f)
  {
    x /= mag;
    y /= mag;
  }
  int dx = static_cast<int>(x * 10.f);
  int dy = static_cast<int>(y * 10.f);
  virtual_mouse->moveMouseRelativeXY(dx, dy);
}

SDL_Gamepad *GamepadController::get_gamepad()
{
  return this->gamepad;
}
SDL_JoystickID GamepadController::get_gamepad_id()
{
  return this->gamepad_id;
}