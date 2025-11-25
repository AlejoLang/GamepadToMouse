#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include "VirtualMouse.hpp"
#include "Gamepad.hpp"
#include "math.h"

int scan_gamepads();
void process_event(js_event event);
void process_event_joystick(js_event event);
void process_event_button(js_event event);

bool exit_program = false;

VirtualMouse *virtual_mouse = nullptr;
Gamepad *current_gamepad = nullptr;
std::vector<Gamepad *> gamepads = {};

int main()
{
  int detected_new_gamepads = scan_gamepads();
  virtual_mouse = new VirtualMouse("Test_Mouse", 0x1010, 0x0101);

  if (!virtual_mouse)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }
  if (gamepads.size() == 0)
  {
    std::cout << "Waiting for a gamepad to be connected" << std::endl;
  }

  while (!exit_program)
  {
    detected_new_gamepads = scan_gamepads();
    if (detected_new_gamepads > 0)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if (current_gamepad)
    {
      js_event event = current_gamepad->get_event();
      while (event.type != static_cast<u_char>(-1))
      {
        process_event(event);
        event = current_gamepad->get_event();
      }
      float x = ((float)current_gamepad->joy1.x / Gamepad::MAX_ANALOG_VALUE);
      float y = ((float)current_gamepad->joy1.y / Gamepad::MAX_ANALOG_VALUE);
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

  for (Gamepad *gamepad : g > amepads)
  {
    delete gamepad;
  }

  delete virtual_mouse;
  return 0;
}

int scan_gamepads()
{
  int new_gamepads = 0;
  std::vector<std::string> devices_paths;
  // Scan the /dev/input/ files
  for (const auto &entry : std::filesystem::directory_iterator("/dev/input/"))
  {
    std::string device_path = entry.path().string();
    bool device_already_added = false;
    // for each file, process the joysticks
    if (device_path.find("js") != std::string::npos)
    {
      devices_paths.push_back(device_path);
      // Checks if the device was already added
      for (auto &gamepad : gamepads)
      {
        if (device_path == gamepad->get_path())
        {
          device_already_added = true;
          break;
        }
      }

      // If the device is new, adds it to the vector
      if (!device_already_added)
      {
        new_gamepads++;
        Gamepad *new_gamepad = new Gamepad(device_path);
        if (new_gamepad->get_file() != -1)
        {
          std::cout << "Gamepad " << entry.path() << " detected" << std::endl;
          new_gamepads++;
          gamepads.push_back(new_gamepad);
        }
        else
        {
          std::cout << "Error processing gamepad " << entry.path() << std::endl;
          delete new_gamepad;
        }
      }
    }
  }

  // Check for devices that have been disconnected
  for (std::vector<Gamepad *>::iterator it = gamepads.begin(); it != gamepads.end();)
  {
    if (find(devices_paths.begin(), devices_paths.end(), (*it)->get_path()) == devices_paths.end())
    {
      std::cout << "Device " << (*it)->get_path() << " removed" << std::endl;

      if (current_gamepad == *it)
      {
        current_gamepad = nullptr;
      }

      delete (*it);
      it = gamepads.erase(it);
    }
    else
    {
      it++;
    }
  }

  if (current_gamepad == nullptr && !gamepads.empty())
  {
    current_gamepad = gamepads[0];
  }
  else
  {
    current_gamepad = nullptr;
  }

  return new_gamepads;
}

void process_event(js_event event)
{
  switch (event.type)
  {
  case Gamepad::EV_JOYSTICK:
    process_event_joystick(event);
    break;
  case JS_EVENT_BUTTON:
    process_event_button(event);
  }
}

void process_event_joystick(js_event event)
{
  if (event.number == current_gamepad->JOY1_X)
  {
    current_gamepad->joy1.x = event.value;
  }
  else if (event.number == current_gamepad->JOY1_Y)
  {
    current_gamepad->joy1.y = event.value;
  }
  else if (event.number == current_gamepad->JOY2_X)
  {
    current_gamepad->joy2.x = event.value;
  }
  else if (event.number == current_gamepad->JOY2_Y)
  {
    current_gamepad->joy2.y = event.value;
  }
  else if (event.number == current_gamepad->DPAD_X)
  {
    current_gamepad->dpad.x = event.value;
  }
  else if (event.number == current_gamepad->DPAD_Y)
  {
    current_gamepad->dpad.y = event.value;
  }
}

void process_event_button(js_event event)
{
  if (event.number == current_gamepad->BTN_CR)
  {
    event.value ? virtual_mouse->pressButton(VirtualMouse::LEFT_CLICK) : virtual_mouse->releaseButton(VirtualMouse::LEFT_CLICK);
  }
  else if (event.number == current_gamepad->BTN_CI)
  {
    event.value ? virtual_mouse->pressButton(VirtualMouse::RIGHT_CLICK) : virtual_mouse->releaseButton(VirtualMouse::RIGHT_CLICK);
  }
  else if (event.number == current_gamepad->BTN_RB)
  {
    event.value ? virtual_mouse->pressButton(VirtualMouse::MIDDLE_CLICK) : virtual_mouse->releaseButton(VirtualMouse::MIDDLE_CLICK);
  }
}