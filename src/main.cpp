#include <iostream>
#include "VirtualMouse.hpp"
#include "Gamepad.hpp"
#include "math.h"

void process_event(js_event event);
void process_event_joystick(js_event event);
void process_event_button(js_event event);

bool exit_program = false;

VirtualMouse *virtual_mouse;
Gamepad *current_gamepad;

int main()
{
  virtual_mouse = new VirtualMouse("Test_Mouse", 0x1010, 0x0101);
  current_gamepad = new Gamepad();

  if (!virtual_mouse)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }
  if (!current_gamepad)
  {
    std::cout << "Error opening gamepad" << std::endl;
    return -1;
  }

  while (!exit_program)
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
  delete current_gamepad;
  delete virtual_mouse;
  return 0;
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