#include "GamepadController.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>

static const std::map<SDL_GamepadButton, VirtualDevice::Action> basic_default_only_error_mapping = {
    {SDL_GAMEPAD_BUTTON_SOUTH, VirtualMouse::LEFT_CLICK_ACTION},
    {SDL_GAMEPAD_BUTTON_EAST, VirtualMouse::RIGHT_CLICK_ACTION},
    {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, VirtualMouse::MIDDLE_CLICK_ACTION},
    {SDL_GAMEPAD_BUTTON_DPAD_UP, VirtualKeyboard::KEY_UP_ACTION},
    {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, VirtualKeyboard::KEY_RIGHT_ACTION},
    {SDL_GAMEPAD_BUTTON_DPAD_DOWN, VirtualKeyboard::KEY_DOWN_ACTION},
    {SDL_GAMEPAD_BUTTON_DPAD_LEFT, VirtualKeyboard::KEY_LEFT_ACTION}};
extern const std::filesystem::path config_path;

static const std::vector<GamepadController::ParsingItem> gamepad_parser = {
    {SDL_GAMEPAD_BUTTON_DPAD_UP, "DPAD_UP", "button_xbox_digital_dpad_up", "button_ps_digital_dpad_up"},
    {SDL_GAMEPAD_BUTTON_DPAD_DOWN, "DPAD_DOWN", "button_xbox_digital_dpad_down", "button_ps_digital_dpad_down"},
    {SDL_GAMEPAD_BUTTON_DPAD_LEFT, "DPAD_LEFT", "button_xbox_digital_dpad_left", "button_ps_digital_dpad_left"},
    {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "DPAD_RIGHT", "button_xbox_digital_dpad_right", "button_ps_digital_dpad_left"},
    {SDL_GAMEPAD_BUTTON_SOUTH, "BTN_SOUTH", "button_xbox_digital_a", "button_ps_digital_cross"},
    {SDL_GAMEPAD_BUTTON_EAST, "BTN_EAST", "button_xbox_digital_b", "button_ps_digital_circle"},
    {SDL_GAMEPAD_BUTTON_NORTH, "BTN_NORTH", "button_xbox_digital_y", "button_ps_digital_triangle"},
    {SDL_GAMEPAD_BUTTON_WEST, "BTN_WEST", "button_xbox_digital_x", "button_ps_digital_square"},
    {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "BTN_RB", "button_xbox_digital_bumper_r", "button_ps_digital_bumper_r"},
    {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "BTN_LB", "button_xbox_digital_bumper_l", "button_ps_digital_bumper_l"},
    {SDL_GAMEPAD_BUTTON_START, "BTN_START", "button_xbox_digital_menu", "button_ps_digital_options"},
    {SDL_GAMEPAD_BUTTON_BACK, "BTN_BACK", "button_xbox_digital_view", "button_ps_digital_share"},
    {SDL_GAMEPAD_BUTTON_GUIDE, "BTN_GUIDE", "button_xbox_digital_home", "button_ps_digital_home"},
    {SDL_GAMEPAD_BUTTON_LEFT_STICK, "BTN_LEFT_STICK", "button_xbox_digital_analog_l", "button_ps_digital_analog_l"},
    {SDL_GAMEPAD_BUTTON_RIGHT_STICK, "BTN_RIGHT_STICK", "button_xbox_digital_analog_r", "button_ps_digital_analog_r"}};

static const std::vector<GamepadController::JoystickParsingItem> gamepad_joystick_parser = {
    {{SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY}, "LEFT", "button_xbox_analog_l", "button_ps_analog_l"},
    {{SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY}, "RIGHT", "button_xbox_analog_r", "button_ps_analog_r"}};

GamepadController::GamepadController(VirtualMouse *vm, VirtualKeyboard *vk, SDL_Gamepad *gp) {
  this->gamepad = gp;
  this->virtual_keyboard = vk;
  this->virtual_mouse = vm;
  this->mouse_joystick = {SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY};
  this->gamepad_id = gp ? SDL_GetGamepadID(gp) : 0;
  std::string gamepad_config_name = "./" + std::string(SDL_GetGamepadName(gp)) + "_" +
                                    std::to_string(SDL_GetGamepadVendor(gp)) + "_" +
                                    std::to_string(SDL_GetGamepadProduct(gp)) + ".cfg";
  this->gamepad_config_path = config_path / gamepad_config_name;
  this->load_config();
}

void GamepadController::process_key_event(SDL_Event *event) {
  if (event->gbutton.which != this->gamepad_id ||
      !this->keymap.contains(static_cast<SDL_GamepadButton>(event->gbutton.button))) {
    return;
  }
  VirtualDevice::Action action = this->keymap.at(static_cast<SDL_GamepadButton>(event->gbutton.button));
  if (action.device == VirtualDevice::MOUSE) {
    virtual_mouse->do_key_action(action, event->gbutton.down);
  } else if (action.device == VirtualDevice::KEYBOARD) {
    virtual_keyboard->do_key_action(action, event->gbutton.down);
  }
}

void GamepadController::process_joysticks() {
  // Process joystick movement
  float x = (float)(SDL_GetGamepadAxis(this->gamepad, this->mouse_joystick.X)) / ANALOG_MAX_VALUE;
  float y = (float)(SDL_GetGamepadAxis(this->gamepad, this->mouse_joystick.Y)) / ANALOG_MAX_VALUE;
  float mag = sqrt(x * x + y * y);
  if (mag > 1.0f) {
    x /= mag;
    y /= mag;
  }
  virtual_mouse->moveMouseRelativeXY(x, y);
}

SDL_Gamepad *GamepadController::get_gamepad() {
  return this->gamepad;
}
SDL_JoystickID GamepadController::get_gamepad_id() {
  return this->gamepad_id;
}

void GamepadController::set_sensitivity(float sens) {
  this->virtual_mouse->set_sensitivity(sens);
}
float GamepadController::get_sensitivity() {
  return this->virtual_mouse->get_sensitivity();
}

void GamepadController::set_mouse_joystick(GamepadController::Joystick joystick) {
  this->mouse_joystick = joystick;
}
GamepadController::Joystick GamepadController::get_mouse_joystick() {
  return this->mouse_joystick;
}

void GamepadController::save_config() {
  std::fstream config_file = std::fstream(this->gamepad_config_path, std::fstream::out);
  if (!config_file.is_open()) {
    std::cerr << "Unable to open config file for writing: " << this->gamepad_config_path.filename() << std::endl;
    return;
  }
  config_file << "SENSITIVITY=" << std::to_string(this->virtual_mouse->get_sensitivity()) << '\n';

  auto it_joy_parser = std::find_if(
      gamepad_joystick_parser.begin(), gamepad_joystick_parser.end(),
      [&](const GamepadController::JoystickParsingItem pi) { return pi.joystick == this->mouse_joystick; });

  config_file << "MOUSE_JOY=" << (it_joy_parser != gamepad_joystick_parser.end() ? it_joy_parser->save_name : "")
              << '\n';
  for (auto keybind : this->keymap) {
    auto it_gp_parser =
        std::find_if(gamepad_parser.begin(), gamepad_parser.end(),
                     [&](const GamepadController::ParsingItem &pi) { return pi.button == keybind.first; });
    if (it_gp_parser == gamepad_parser.end()) {
      continue;
    }
    if (keybind.second.device == VirtualDevice::KEYBOARD) {
      auto it_kb_parser =
          std::find_if(VirtualKeyboard::parser.begin(), VirtualKeyboard::parser.end(),
                       [&](const VirtualDevice::ParsingItem &pi) { return pi.action == keybind.second; });
      if (it_kb_parser != VirtualKeyboard::parser.end()) {
        std::string value = "KEYBOARD_" + it_kb_parser->save_string;
        config_file << it_gp_parser->save_name << "=" << value << '\n';
      }
    }
    if (keybind.second.device == VirtualDevice::MOUSE) {
      auto it_value = std::find_if(VirtualMouse::parser.begin(), VirtualMouse::parser.end(),
                                   [&](const VirtualDevice::ParsingItem &pi) { return pi.action == keybind.second; });
      if (it_value != VirtualMouse::parser.end()) {
        std::string value = "MOUSE_" + it_value->save_string;
        config_file << it_gp_parser->save_name << "=" << value << '\n';
      }
    }
  }
  config_file.close();
}

void GamepadController::load_config() {
  std::fstream config_file = std::fstream(this->gamepad_config_path, std::fstream::in);
  if (!config_file.is_open()) {
    config_file = std::fstream("./default_config.cfg", std::fstream::in);
  }
  if (!config_file.is_open()) {
    this->keymap = basic_default_only_error_mapping;
    this->mouse_joystick = {SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY};

    std::cout << "Using default config" << std::endl;
    try {
      this->save_config();
      std::cout << "Created default config: " << this->gamepad_config_path.filename() << " using a basic config"
                << std::endl;
    }
    catch (...) {
      std::cerr << "Failed to create default config file: " << this->gamepad_config_path.filename() << std::endl;
    }
    return;
  }
  config_file.seekg(0);
  std::string config_line;
  while (std::getline(config_file, config_line)) {
    size_t eq_del = config_line.find('=');
    if (eq_del == std::string::npos) {
      std::cout << "Error processing line " << config_line << " format should be KEY=VALUE" << std::endl;
      std::cout << "KEY being either a SDL_GamepadButton value, SENSITIVITY or MOUSE_JOY" << std::endl;
      std::cout << "VALUE being either a VirtualDevice action, a sensivity value or LEFT/RIGHT for joystick"
                << std::endl;
      this->keymap.clear();
      break;
    }
    std::string config_key = config_line.substr(0, eq_del);
    std::string config_value = config_line.substr(eq_del + 1);
    if (config_key.size() > 0 && config_value.size() > 0) {
      if (config_key == "SENSITIVITY") {
        try {
          float sens_val = std::stof(config_value);
          this->virtual_mouse->set_sensitivity(sens_val);
        }
        catch (const std::exception &e) {
          std::cerr << "Invalid SENSITIVITY value: '" << config_value << "' - skipping" << std::endl;
          this->virtual_mouse->set_sensitivity(0.2);
        }
      } else if (config_key == "MOUSE_JOY") {
        auto it_joy_parser =
            std::find_if(gamepad_joystick_parser.begin(), gamepad_joystick_parser.end(),
                         [&](const GamepadController::JoystickParsingItem pi) { return pi.save_name == config_value; });
        if (it_joy_parser != gamepad_joystick_parser.end()) {
          this->mouse_joystick = it_joy_parser->joystick;
        }
      } else {
        try {
          auto it_gp_parser =
              std::find_if(gamepad_parser.begin(), gamepad_parser.end(),
                           [&](const GamepadController::ParsingItem &pi) { return pi.save_name == config_key; });
          SDL_GamepadButton gp_btn = it_gp_parser->button;
          size_t dev_key_limiter = config_value.find('_');
          std::string device =
              (dev_key_limiter == std::string::npos) ? config_value : config_value.substr(0, dev_key_limiter);
          std::string action_key =
              (dev_key_limiter == std::string::npos) ? std::string() : config_value.substr(dev_key_limiter + 1);
          if (device == "MOUSE" && !action_key.empty() && it_gp_parser != gamepad_parser.end()) {
            auto it_action =
                std::find_if(VirtualMouse::parser.begin(), VirtualMouse::parser.end(),
                             [&](const VirtualDevice::ParsingItem &pi) { return pi.save_string == action_key; });
            this->keymap.insert(std::make_pair(gp_btn, it_action->action));
          } else if (device == "KEYBOARD" && !action_key.empty() && it_gp_parser != gamepad_parser.end()) {
            auto it_action =
                std::find_if(VirtualKeyboard::parser.begin(), VirtualKeyboard::parser.end(),
                             [&](const VirtualDevice::ParsingItem &pi) { return pi.save_string == action_key; });
            this->keymap.insert(std::make_pair(gp_btn, it_action->action));
          }
          std::cout << "New keymap added " << config_key << " " << device << " " << action_key << std::endl;
        }
        catch (const std::out_of_range &e) {
          std::cerr << "Key value out of range in config: '" << config_key << "' - skipping line" << std::endl;
        }
      }
    }
  }
  config_file.close();
}

std::string GamepadController::get_button_icon(SDL_GamepadButton btn) {

  auto it_gp_parser = std::find_if(gamepad_parser.begin(), gamepad_parser.end(),
                                   [&](const GamepadController::ParsingItem &pi) { return pi.button == btn; });
  if (it_gp_parser == gamepad_parser.end()) {
    return "";
  }
  return "./assets/" + (SDL_GetGamepadType(this->gamepad) == SDL_GAMEPAD_TYPE_STANDARD ? it_gp_parser->icon_name_xbox
                                                                                       : it_gp_parser->icon_name_ps);
}

SDL_GamepadButton GamepadController::get_binded_button_for_action(VirtualDevice::Action act) {
  auto it_keybind =
      std::find_if(this->keymap.begin(), this->keymap.end(),
                   [&](const std::pair<SDL_GamepadButton, VirtualDevice::Action> pair) { return pair.second == act; });
  if (it_keybind == this->keymap.end()) {
    return static_cast<SDL_GamepadButton>(-1);
  }
  return it_keybind->first;
}

std::string GamepadController::get_binded_icon_for_action(VirtualDevice::Action act) {
  return this->get_button_icon(this->get_binded_button_for_action(act));
}

std::vector<GamepadController::JoystickParsingItem> GamepadController::get_joystick_parsing_items() {
  return gamepad_joystick_parser;
}

void GamepadController::set_keybind(SDL_GamepadButton btn, VirtualDevice::Action act) {
  auto it = std::find_if(gamepad_parser.begin(), gamepad_parser.end(),
                         [&](const GamepadController::ParsingItem pi) { return pi.button == btn; });
  if (it == gamepad_parser.end()) {
    return;
  }
  auto it_keybind =
      std::find_if(this->keymap.begin(), this->keymap.end(),
                   [&](const std::pair<SDL_GamepadButton, VirtualDevice::Action> pair) { return pair.second == act; });
  if (it_keybind != this->keymap.end()) {
    this->keymap.erase(it_keybind);
  }
  std::cout << (int)keymap[btn].key << std::endl;
  this->keymap[btn] = act;
  std::cout << (int)keymap[btn].key << std::endl;
}