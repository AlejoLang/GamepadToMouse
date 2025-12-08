#ifndef GamepadController_h
#define GamepadController_h

#include "SDL3/SDL.h"
#include "SDL3/SDL_gamepad.h"
#include "VirtualDevice.hpp"
#include "VirtualKeyboard.hpp"
#include "VirtualMouse.hpp"
#include <filesystem>
#include <map>
#include <string>

#define ANALOG_MAX_VALUE 32767;
#define DEFAULT_CONFIG_ROUTE "./default_config.cfg";

class GamepadController {
public:
  struct Joystick {
    SDL_GamepadAxis X;
    SDL_GamepadAxis Y;
    bool operator==(const Joystick &joy) {
      return this->X == joy.X && this->Y == joy.Y;
    }
  };
  struct ParsingItem {
    SDL_GamepadButton button;
    std::string save_name;
    std::string icon_name_xbox;
    std::string icon_name_ps;
  };
  struct JoystickParsingItem {
    Joystick joystick;
    std::string save_name;
    std::string icon_name_xbox;
    std::string icon_name_ps;
  };

private:
  VirtualMouse *virtual_mouse;
  VirtualKeyboard *virtual_keyboard;
  SDL_Gamepad *gamepad;
  SDL_JoystickID gamepad_id;
  std::map<SDL_GamepadButton, VirtualDevice::Action> keymap;
  std::filesystem::path gamepad_config_path;
  Joystick mouse_joystick;

public:
  GamepadController(VirtualMouse *vm, VirtualKeyboard *vk, SDL_Gamepad *gp);
  void process_key_event(SDL_Event *event);
  void process_joysticks();
  SDL_Gamepad *get_gamepad();
  SDL_JoystickID get_gamepad_id();
  void add_keybind(SDL_GamepadButton gp_btn, VirtualDevice::Action act);
  void set_sensitivity(float sens);
  float get_sensitivity();
  void set_mouse_joystick(Joystick joystick);
  Joystick get_mouse_joystick();
  void save_config();
  void load_config();
  std::string get_button_icon(SDL_GamepadButton btn);
  SDL_GamepadButton get_binded_button_for_action(VirtualDevice::Action act);
  std::string get_binded_icon_for_action(VirtualDevice::Action act);
  std::vector<GamepadController::JoystickParsingItem> get_joystick_parsing_items();
  void set_keybind(SDL_GamepadButton btn, VirtualDevice::Action act);
};

#endif