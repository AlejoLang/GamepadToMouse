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
#include "GamepadController.hpp"

#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"

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
VirtualKeyboard *virtual_keyboard = nullptr;
GamepadController current_controller = {nullptr, nullptr, nullptr};
std::vector<GamepadController> controllers;

int main()
{
  SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK);
  virtual_mouse = new VirtualMouse("Test_Mouse");
  virtual_keyboard = new VirtualKeyboard("Test_Keyboard");

  if (virtual_mouse->get_file() == -1)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }
  if (virtual_keyboard->get_file() == -1)
  {
    std::cout << "Error creating virtual keyboard" << std::endl;
    return -1;
  }

  while (!exit_program)
  {
    process_gamepad_events();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (current_controller.get_gamepad() != nullptr)
    {
      current_controller.process_joysticks();
    }
  }

  for (auto controller : controllers)
  {
    SDL_CloseGamepad(controller.get_gamepad());
  }

  delete virtual_mouse;
  delete virtual_keyboard;
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
      controllers.push_back({virtual_mouse, virtual_keyboard, added_gamepad});
      if (current_controller.get_gamepad() == nullptr)
      {
        current_controller = GamepadController({virtual_mouse, virtual_keyboard, added_gamepad});
        SDL_RumbleGamepad(current_controller.get_gamepad(), 0x0100, 0x0100, 10);
      }
      break;
    }
    case SDL_EVENT_GAMEPAD_REMOVED:
    {
      SDL_JoystickID removed_gamepad_id = new_event.gdevice.which;
      for (std::vector<GamepadController>::iterator it = controllers.begin(); it != controllers.end(); it++)
      {
        if (it->get_gamepad_id() == removed_gamepad_id)
        {
          SDL_CloseGamepad(it->get_gamepad());
          controllers.erase(it);
          break;
        }
      }
      if (current_controller.get_gamepad_id() == removed_gamepad_id)
      {
        current_controller = controllers.empty() ? GamepadController(nullptr, nullptr, nullptr) : controllers[0];
      }
      break;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
    {
      if (new_event.gbutton.button == SDL_GAMEPAD_BUTTON_GUIDE)
      {
        exit_program = 1;
        return;
      }
      current_controller.process_key_event(&new_event);
      break;
    }
    default:
      break;
    }
    has_event = SDL_PollEvent(&new_event);
  }
}