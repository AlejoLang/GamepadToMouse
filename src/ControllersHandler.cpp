#include "ControllersHandler.hpp"
#include <SDL3/SDL_gamepad.h>
#include <algorithm>
#include <iostream>
ControllersHandler::ControllersHandler()
{
  this->virtual_keyboard = new VirtualKeyboard();
  if (this->virtual_keyboard->get_file() == -1)
  {
    std::__throw_runtime_error("Error creating virtual mouse");
  }
  this->virtual_mouse = new VirtualMouse();
  if (this->virtual_mouse->get_file() == -1)
  {
    std::__throw_runtime_error("Error creating virtual keyboard");
  }
  this->controllers = {};
  this->current_controller = nullptr;
}

void ControllersHandler::add_controller(SDL_JoystickID id)
{
  SDL_OpenGamepad(id);
  SDL_Gamepad *gamepad = SDL_GetGamepadFromID(id);
  if (!gamepad)
  {
    return;
  }
  GamepadController *new_controller = new GamepadController(this->virtual_mouse, this->virtual_keyboard, gamepad);
  this->controllers.push_back(new_controller);
  if (current_controller == nullptr)
  {
    current_controller = new_controller;
  }
}

void ControllersHandler::remove_controller(SDL_JoystickID id)
{
  std::vector<GamepadController *>::iterator it = std::find_if(this->controllers.begin(), this->controllers.end(), [id](GamepadController *cont)
                                                               { return cont->get_gamepad_id() == id; });
  if (it == this->controllers.end())
  {
    return;
  }
  this->controllers.erase(it);
  if (current_controller->get_gamepad_id() == id)
  {
    current_controller = nullptr;
    if (!this->controllers.empty())
    {
      current_controller = this->controllers[0];
    }
  }
}

GamepadController *ControllersHandler::get_current_controller()
{
  return this->current_controller;
}

std::vector<GamepadController *> ControllersHandler::get_controllers()
{
  return this->controllers;
}

ControllersHandler::~ControllersHandler()
{
  for (GamepadController *controller : this->controllers)
  {
    SDL_CloseGamepad(controller->get_gamepad());
    delete controller;
  }
  delete this->virtual_keyboard;
  delete this->virtual_mouse;
}