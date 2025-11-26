#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include "VirtualMouse.hpp"
#include "math.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_mouse.h"

#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"
#define ANALOG_MAX_VALUE 32767

int scan_gamepads();
void process_gamepad_events();
void process_gamepad_button_event(SDL_Event *event);

bool exit_program = false;

struct Gamepad
{
  SDL_JoystickID id;
  SDL_Gamepad *gamepad;
};

VirtualMouse *virtual_mouse = nullptr;
Gamepad current_gamepad = {0, nullptr};
std::vector<Gamepad> gamepads = {};

int main()
{
  SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK);
  virtual_mouse = new VirtualMouse("Test_Mouse", 0x1010, 0x0101);

  if (!virtual_mouse)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }

  while (!exit_program)
  {
    process_gamepad_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (current_gamepad.gamepad != nullptr)
    {
      float x = ((float)SDL_GetGamepadAxis(current_gamepad.gamepad, SDL_GAMEPAD_AXIS_LEFTX) / ANALOG_MAX_VALUE);
      float y = ((float)SDL_GetGamepadAxis(current_gamepad.gamepad, SDL_GAMEPAD_AXIS_LEFTY) / ANALOG_MAX_VALUE);
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
  }

  for (auto gamepad : gamepads)
  {
    SDL_CloseGamepad(gamepad.gamepad);
  }

  delete virtual_mouse;
  return 0;
}

void process_gamepad_events()
{
  SDL_Event new_event;
  bool has_event = SDL_PollEvent(&new_event);
  while (has_event)
  {
    switch (new_event.type)
    {
    case SDL_EVENT_GAMEPAD_ADDED:
    {
      SDL_JoystickID added_gamepad_id = new_event.gdevice.which;
      SDL_OpenGamepad(added_gamepad_id);
      SDL_Gamepad *added_gamepad = SDL_GetGamepadFromID(added_gamepad_id);
      gamepads.push_back({added_gamepad_id, added_gamepad});
      if (current_gamepad.gamepad == nullptr)
      {
        current_gamepad = {added_gamepad_id, added_gamepad};
        SDL_RumbleGamepad(current_gamepad.gamepad, 0x0100, 0x0100, 10);
      }
      break;
    }
    case SDL_EVENT_GAMEPAD_REMOVED:
    {
      SDL_JoystickID removed_gamepad_id = new_event.gdevice.which;
      for (std::vector<Gamepad>::iterator it = gamepads.begin(); it != gamepads.end(); it++)
      {
        if (it->id == removed_gamepad_id)
        {
          SDL_CloseGamepad(it->gamepad);
          gamepads.erase(it);
          break;
        }
      }
      if (current_gamepad.id == removed_gamepad_id)
      {
        current_gamepad = gamepads.empty() ? Gamepad(0, nullptr) : gamepads[0];
      }
      break;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
    {
      process_gamepad_button_event(&new_event);
      break;
    }
    default:
      break;
    }
    has_event = SDL_PollEvent(&new_event);
  }
}
void process_gamepad_button_event(SDL_Event *event)
{
  switch (event->gbutton.button)
  {
  case SDL_GAMEPAD_BUTTON_SOUTH:
  {
    event->gbutton.down ? virtual_mouse->pressButton(VirtualMouse::LEFT_CLICK) : virtual_mouse->releaseButton(VirtualMouse::LEFT_CLICK);
    break;
  }
  case SDL_GAMEPAD_BUTTON_EAST:
  {
    event->gbutton.down ? virtual_mouse->pressButton(VirtualMouse::RIGHT_CLICK) : virtual_mouse->releaseButton(VirtualMouse::RIGHT_CLICK);
    break;
  }
  case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
  {
    event->gbutton.down ? virtual_mouse->pressButton(VirtualMouse::MIDDLE_CLICK) : virtual_mouse->releaseButton(VirtualMouse::MIDDLE_CLICK);
    break;
  }
  case SDL_GAMEPAD_BUTTON_GUIDE:
  {
    exit_program = true;
    break;
  }
  }
}
