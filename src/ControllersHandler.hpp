#ifndef CONTROLLERSHANDLER_H
#define CONTROLLERSHANDLER_H
#include "GamepadController.hpp"
#include "VirtualMouse.hpp"
#include "VirtualKeyboard.hpp"
#include <vector>

class ControllersHandler
{
private:
  VirtualMouse *virtual_mouse;
  VirtualKeyboard *virtual_keyboard;
  std::vector<GamepadController *> controllers;
  GamepadController *current_controller;

public:
  ControllersHandler();
  GamepadController *get_current_controller();
  void add_controller(SDL_JoystickID id);
  void remove_controller(SDL_JoystickID id);
  std::vector<GamepadController *> get_controllers();
  ~ControllersHandler();
};

#endif