#include "GamepadController.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <algorithm>

static const std::map<SDL_GamepadButton, VirtualDevice::Action> basic_default_only_error_mapping = {{SDL_GAMEPAD_BUTTON_SOUTH, VirtualMouse::LEFT_CLICK_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_EAST, VirtualMouse::RIGHT_CLICK_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, VirtualMouse::MIDDLE_CLICK_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_DPAD_UP, VirtualKeyboard::KEY_UP_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, VirtualKeyboard::KEY_RIGHT_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_DPAD_DOWN, VirtualKeyboard::KEY_DOWN_ACTION},
                                                                                                    {SDL_GAMEPAD_BUTTON_DPAD_LEFT, VirtualKeyboard::KEY_LEFT_ACTION}};

GamepadController::GamepadController(VirtualMouse *vm, VirtualKeyboard *vk, SDL_Gamepad *gp)
{
  this->gamepad = gp;
  this->virtual_keyboard = vk;
  this->virtual_mouse = vm;
  this->gamepad_id = gp ? SDL_GetGamepadID(gp) : 0;
  this->gamepad_config_name = "./" + std::string(SDL_GetGamepadName(gp)) + "_" + std::to_string(SDL_GetGamepadVendor(gp)) + "_" + std::to_string(SDL_GetGamepadProduct(gp)) + ".cfg";
  std::fstream config_file = std::fstream(this->gamepad_config_name, std::fstream::in);
  if (!config_file.is_open())
  {
    config_file = std::fstream("./default_config.cfg", std::fstream::in);
  }
  if (!config_file.is_open())
  {
    this->keymap = basic_default_only_error_mapping;
    this->mouse_x_axis = SDL_GAMEPAD_AXIS_LEFTX;
    this->mouse_y_axis = SDL_GAMEPAD_AXIS_LEFTY;
    std::cout << "Using default config" << std::endl;
    // No config file found: attempt to create one with defaults
    try
    {
      this->save_config();
      std::cout << "Created default config: " << this->gamepad_config_name << std::endl;
    }
    catch (...)
    {
      std::cerr << "Failed to create default config file: " << this->gamepad_config_name << std::endl;
    }
    return;
  }
  config_file.seekg(0);
  std::string config_line;
  while (std::getline(config_file, config_line))
  {
    size_t eq_del = config_line.find('=');
    if (eq_del == std::string::npos)
    {
      std::cout << "Error processing line " << config_line << " format should be KEY=VALUE" << std::endl;
      std::cout << "KEY being either a SDL_GamepadButton value, SENSITIVITY or MOUSE_JOY" << std::endl;
      std::cout << "VALUE being either a VirtualDevice action, a sensivity value or LEFT/RIGHT for joystick" << std::endl;
      this->keymap.clear();
      break;
    }
    std::string key = config_line.substr(0, eq_del);
    std::string value = config_line.substr(eq_del + 1);
    if (key.size() > 0 && value.size() > 0)
    {
      if (key == "SENSITIVITY")
      {
        try
        {
          float sens_val = std::stof(value);
          this->virtual_mouse->set_sensitivity(sens_val);
        }
        catch (const std::exception &e)
        {
          std::cerr << "Invalid SENSITIVITY value: '" << value << "' - skipping" << std::endl;
        }
      }
      else if (key == "MOUSE_JOY")
      {
        if (value == "RIGHT")
        {
          this->mouse_x_axis = SDL_GAMEPAD_AXIS_RIGHTX;
          this->mouse_y_axis = SDL_GAMEPAD_AXIS_RIGHTY;
        }
        else if (value == "LEFT")
        {
          this->mouse_x_axis = SDL_GAMEPAD_AXIS_LEFTX;
          this->mouse_y_axis = SDL_GAMEPAD_AXIS_LEFTY;
        }
      }
      else
      {
        // Expect key to be numeric (SDL button id). Validate before converting.
        try
        {
          int gp_key = std::stoi(key);
          size_t dev_key_limiter = value.find('_');
          std::string device = (dev_key_limiter == std::string::npos) ? value : value.substr(0, dev_key_limiter);
          std::string action_key = (dev_key_limiter == std::string::npos) ? std::string() : value.substr(dev_key_limiter + 1);
          if (device == "MOUSE" && !action_key.empty())
          {
            this->keymap.insert(std::make_pair(static_cast<SDL_GamepadButton>(gp_key), VirtualMouse::parser.at(action_key)));
          }
          else if (device == "KEYBOARD" && !action_key.empty())
          {
            this->keymap.insert(std::make_pair(static_cast<SDL_GamepadButton>(gp_key), VirtualKeyboard::parser.at(action_key)));
          }
          std::cout << "New keymap added " << gp_key << " " << device << " " << action_key << std::endl;
        }
        catch (const std::invalid_argument &e)
        {
          std::cerr << "Invalid key in config (not a number): '" << key << "' - skipping line" << std::endl;
        }
        catch (const std::out_of_range &e)
        {
          std::cerr << "Key value out of range in config: '" << key << "' - skipping line" << std::endl;
        }
      }
    }
  }
  config_file.close();
  this->save_config();
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
  virtual_mouse->moveMouseRelativeXY(x, y);
}

SDL_Gamepad *GamepadController::get_gamepad()
{
  return this->gamepad;
}
SDL_JoystickID GamepadController::get_gamepad_id()
{
  return this->gamepad_id;
}

void GamepadController::save_config()
{
  std::fstream config_file = std::fstream(this->gamepad_config_name, std::fstream::out);
  if (!config_file.is_open())
  {
    std::cerr << "Unable to open config file for writing: " << this->gamepad_config_name << std::endl;
    return;
  }
  config_file << "SENSITIVITY=" << std::to_string(this->virtual_mouse->get_sensitivity()) << '\n';
  config_file << "MOUSE_JOY=" << (this->mouse_x_axis == SDL_GAMEPAD_AXIS_LEFTX ? "LEFT" : "RIGHT") << '\n';
  for (auto keybind : this->keymap)
  {
    if (keybind.second.device == VirtualDevice::KEYBOARD)
    {
      auto it = std::find_if(VirtualKeyboard::parser.begin(), VirtualKeyboard::parser.end(), [&](const std::pair<std::string, VirtualDevice::Action> pair)
                             { return pair.second == keybind.second; });
      if (it != VirtualKeyboard::parser.end())
      {
        std::string value = "KEYBOARD_" + it->first;
        config_file << std::to_string(keybind.first) << "=" << value << '\n';
      }
    }
    if (keybind.second.device == VirtualDevice::MOUSE)
    {
      auto it = std::find_if(VirtualMouse::parser.begin(), VirtualMouse::parser.end(), [&](const std::pair<std::string, VirtualDevice::Action> pair)
                             { return pair.second == keybind.second; });
      if (it != VirtualMouse::parser.end())
      {
        std::string value = "MOUSE_" + it->first;
        config_file << std::to_string(keybind.first) << "=" << value << '\n';
      }
    }
  }
  config_file.close();
}