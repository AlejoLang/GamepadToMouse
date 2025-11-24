#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/uinput.h>
#include <linux/joystick.h>
#include "VirtualMouse.hpp"

void process_event(js_event event);

int16_t joy_x = 0;
int16_t joy_y = 0;
bool exit_program = false;

int mouse_file;
int gamepad_file;

VirtualMouse *virtual_mouse;

int main()
{
  virtual_mouse = new VirtualMouse("Test_Mouse", 0x1010, 0x0101);
  gamepad_file = open("/dev/input/js0", O_NONBLOCK);

  if (!virtual_mouse)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }
  if (!gamepad_file)
  {
    std::cout << "Error opening gamepad" << std::endl;
    return -1;
  }

  js_event event;
  while (!exit_program)
  {
    while (read(gamepad_file, &event, sizeof(event)) > 0)
    {
      process_event(event);
    }
    virtual_mouse->moveMouseRelative(VirtualMouse::X_AXIS, 10 * ((float)joy_x / 32767));
    virtual_mouse->moveMouseRelative(VirtualMouse::Y_AXIS, 10 * ((float)joy_y / 32767));
  }
  sleep(1);
  close(gamepad_file);
  return 0;
}

void process_event(js_event event)
{
  switch (event.type)
  {
  case JS_EVENT_AXIS:
    if (event.number == 0)
    {
      joy_x = event.value;
    }
    else if (event.number == 1)
    {
      joy_y = event.value;
    }
    std::cout << joy_x << ", " << joy_y << std::endl;
    break;
  case JS_EVENT_BUTTON:
    std::cout << (int)event.number << std::endl;
    if (event.number == 10)
    {
      exit_program = true;
    }
    if (event.number == 0)
    {
      event.value ? virtual_mouse->releaseButton(VirtualMouse::LEFT_CLICK) : virtual_mouse->releaseButton(VirtualMouse::LEFT_CLICK);
    }
    break;
  }
}